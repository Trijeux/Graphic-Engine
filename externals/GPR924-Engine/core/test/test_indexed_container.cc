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

Contributors: Elias Farhan
Contributors: Anthony Barman
*/

#include <gtest/gtest.h>

#include "container/indexed_container.h"

struct Value {
  int value;
  [[nodiscard]] constexpr static Value GenerateInvalidValue() noexcept {
    return {-1};
  }
  [[nodiscard]] constexpr bool IsInvalid() const noexcept {
    return value == -1;
  }
};

TEST(IndexBasedContainer, Construction) {
  core::IndexedContainer<Value> values = {};
}

TEST(IndexBasedContainer, AddValue) {
  core::IndexedContainer<Value> values = {};
  auto index = values.Add();
  EXPECT_FALSE(values.At(index).IsInvalid());
}

TEST(IndexBasedContainer, RemoveValue) {
  core::IndexedContainer<Value> values = {};
  const auto index = values.Add();
  values.Remove(index);
  Value v{};
  EXPECT_THROW(v = values.At(index), std::runtime_error);
}

TEST(IndexBasedContainer, RangeBasedFor) {
  core::IndexedContainer<Value> values = {};
  int init_count = 0;
  for (int i = 0; i < 10; i++) {
    if (auto index = values.Add({i}); index.index() != i) {
      FAIL();
    }
    init_count += i;
  }
  int result_count = 0;
  for (const auto& v : values) {
    if (v.IsInvalid())
      continue;
    result_count += v.value;
  }
  EXPECT_EQ(init_count, result_count);
}

TEST(IndexBasedContainer, RemoveAndAddValue) {
  core::IndexedContainer<Value> values = {};
  const auto index_1 = values.Add();
  const auto index_2 = values.Add();
  EXPECT_EQ(index_1.index(), 0);
  EXPECT_EQ(index_1.generationIndex(), 0);
  EXPECT_EQ(index_2.index(), 1);
  EXPECT_EQ(index_2.generationIndex(), 0);

  values.Remove(index_1);

  const auto index_3 = values.Add();
  EXPECT_EQ(index_3.index(), 0);
  EXPECT_EQ(index_3.generationIndex(), 1);
}

class MoveOnlyValue {
 public:
  int value = 0;
  MoveOnlyValue() = default;
  MoveOnlyValue(MoveOnlyValue&&) noexcept = default;
  MoveOnlyValue& operator=(MoveOnlyValue&&) noexcept = default;
  MoveOnlyValue(const MoveOnlyValue&) = delete;
  MoveOnlyValue& operator=(const MoveOnlyValue&) = delete;
  ~MoveOnlyValue() = default;
  [[nodiscard]] constexpr static MoveOnlyValue GenerateInvalidValue() noexcept {
    MoveOnlyValue value;
    value.value = -1;
    return value;
  }
  [[nodiscard]] constexpr bool IsInvalid() const noexcept {
    return value == -1;
  }
};

TEST(IndexBasedContainer, MoveValue) {
  core::IndexedContainer<MoveOnlyValue> values = {};
  MoveOnlyValue m;
  m.value = 3;
  const auto index = values.Add(std::move(m));
  values.Remove(index);
  [[maybe_unused]] const auto default_index = values.Add();
}

TEST(IndexBasedContainer, GenerateInvalidIndex) {
  const auto invalid_value = core::Index<Value>::GenerateInvalidIndex();
  EXPECT_EQ(invalid_value.index(), -1);
}