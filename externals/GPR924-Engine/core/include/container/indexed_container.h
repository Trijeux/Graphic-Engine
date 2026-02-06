#ifndef GPR924_ENGINE_INDEXED_CONTAINER_H
#define GPR924_ENGINE_INDEXED_CONTAINER_H

/*
Copyright 2025 SAE Institute Switzerland SA

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Contributors: Anthony Barman
Contributors: Elias Farhan, Jemoel Ablay
*/

#include <imgui.h>

#include <algorithm>
#include <span>
#include <stdexcept>
#include <vector>

namespace core {
template <typename T>
concept CanBeInvalid = requires(const T value) {
  { value.IsInvalid() } -> std::same_as<bool>;
  { T::GenerateInvalidValue() } -> std::convertible_to<T>;
};

template <typename T, typename IndexType = int,
          typename GenerationIndexType = int>
class Index {
 public:
  using index_type = IndexType;
  using generation_index_type = GenerationIndexType;
  explicit Index(int index, int generationIndex = 0)
      : index_(index), generationIndex_(generationIndex) {}
  bool operator==(const Index& index) const {
    return index_ == index.index_ && generationIndex_ == index.generationIndex_;
  }
  IndexType index() const { return index_; }
  GenerationIndexType generationIndex() const { return generationIndex_; }

  static constexpr Index GenerateInvalidIndex() noexcept {
    return Index(-1);
  }

 private:
  template <typename U>
  friend class IndexedContainer;
  IndexType index_ = -1;
  GenerationIndexType generationIndex_ = 0;
};

template <typename T>
class IndexedContainer {
 public:
  Index<T> Add(T&& new_value) {
    auto it = std::find_if(values_.begin(), values_.end(),
                           [](const auto& v) { return v.first.IsInvalid(); });
    if (it == values_.end()) {
      Index<T> index{static_cast<Index<T>::index_type>(std::ssize(values_))};
      values_.push_back({std::move(new_value), 0});
      return index;
    }
    Index<T> index{
        static_cast<Index<T>::index_type>(std::distance(values_.begin(), it)),
        it->second};
    it->first = std::move(new_value);
    return index;
  }

  Index<T> Add(const T& new_value) {
    auto it = std::find_if(values_.begin(), values_.end(),
                           [](const auto& v) { return v.first.IsInvalid(); });
    if (it == values_.end()) {
      Index<T> index{static_cast<Index<T>::index_type>(std::ssize(values_))};
      values_.push_back({new_value, 0});
      return index;
    }
    Index<T> index{
        static_cast<Index<T>::index_type>(std::distance(values_.begin(), it)),
        it->second};
    it->first = new_value;
    return index;
  }

  Index<T> Add() {
    auto it = std::find_if(values_.begin(), values_.end(),
                           [](const auto& v) { return v.first.IsInvalid(); });
    if (it == values_.end()) {
      Index<T> index{static_cast<Index<T>::index_type>(std::ssize(values_))};
      values_.emplace_back(T{}, 0);
      return index;
    }
    Index<T> index{
        static_cast<Index<T>::index_type>(std::distance(values_.begin(), it)),
        it->second};
    it->first = {};
    return index;
  }
  [[nodiscard]] const T& At(Index<T> index) const {
    const auto& pair = values_.at(index.index_);
    if (index.generationIndex_ != pair.second) {
      throw std::runtime_error(
          "Trying to get value at index with invalid generation_index");
    }
    return pair.first;
  }
  [[nodiscard]] T& At(Index<T> index) {
    auto& pair = values_.at(static_cast<size_t>(index.index_));
    if (index.generationIndex_ != pair.second) {
      throw std::runtime_error(
          "Trying to get value at index with invalid generation_index");
    }
    return pair.first;
  }

  void Remove(Index<T> index) {
    auto& pair = values_.at(static_cast<size_t>(index.index_));
    if (index.generationIndex_ != pair.second) {
      throw std::runtime_error(
          "Trying to remove value at index with invalid generation_index");
    }
    pair.first = T::GenerateInvalidValue();
    ++pair.second;
  }

  [[nodiscard]] size_t size() const noexcept {
    return static_cast<size_t>(
      std::count_if(values_.begin(), values_.end(),
                         [](const auto& v) { return !v.first.IsInvalid(); }));
  }

  class Iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using pair_type = std::pair<T, typename Index<T>::generation_index_type>;

    explicit Iterator(pair_type* ptr) : m_ptr(ptr) {}

    reference operator*() const { return m_ptr->first; }
    pointer operator->() const { return &m_ptr->first; }

    Iterator& operator++() {
      ++m_ptr;
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    Iterator& operator--() {
      --m_ptr;
      return *this;
    }

    Iterator operator--(int) {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }

    Iterator operator+(difference_type n) const {
      return MyIterator(m_ptr + n);
    }
    Iterator operator-(difference_type n) const {
      return MyIterator(m_ptr - n);
    }
    difference_type operator-(const Iterator& other) const {
      return m_ptr - other.m_ptr;
    }
    bool operator==(const Iterator& other) const {
      return m_ptr == other.m_ptr;
    }
    bool operator!=(const Iterator& other) const {
      return m_ptr != other.m_ptr;
    }
    bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
    bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
    bool operator<=(const Iterator& other) const {
      return m_ptr <= other.m_ptr;
    }
    bool operator>=(const Iterator& other) const {
      return m_ptr >= other.m_ptr;
    }

   private:
    pair_type* m_ptr;
  };

  [[nodiscard]] auto begin() { return Iterator{values_.data()}; }
  [[nodiscard]] auto end() { return Iterator{values_.data() + values_.size()}; }

  [[nodiscard]] std::span<
      const std::pair<T, typename Index<T>::generation_index_type>>
  values() const {
    return values_;
  }
  [[nodiscard]] std::span<
      std::pair<T, typename Index<T>::generation_index_type>>
  values() {
    return values_;
  }

 private:
  static_assert(
      CanBeInvalid<T>,
      "requires function bool IsInvalid() && GenerateInvalidValue();");
  std::vector<std::pair<T, typename Index<T>::generation_index_type>> values_;
};
}  // namespace core

#endif  // GPR924_ENGINE_INDEXED_CONTAINER_H
