//
// Created by unite on 03.12.2025.
//

#ifndef GPR924_ENGINE_RESOURCE_H
#define GPR924_ENGINE_RESOURCE_H

#include <utility>

namespace core {

template <typename DestructorT, typename ResourceT>
concept is_resource_destructible =
    requires(DestructorT destructor, ResourceT resource) {
      { DestructorT(resource) };
      { destructor() };
    };

/**
 *
 * @tparam T the resource type
 * @tparam Destructor the destructor of the type
 */
template <typename T, typename Destructor>
class Resource {
  static_assert(is_resource_destructible<Destructor, T>,
                "Resource requires a Destructor type");

 public:
  Resource() = default;

  explicit Resource(const T& data) : data_(data) {}

  explicit Resource(T&& data) : data_(std::move(data)) {}
  virtual ~Resource() { Clear(); };
  Resource(const Resource& other) = delete;
  Resource& operator=(const Resource& other) = delete;

  Resource(Resource&& other) noexcept { std::swap(data_, other.data_); }

  Resource& operator=(Resource&& other) noexcept {
    std::swap(data_, other.data_);
    return *this;
  }

  [[nodiscard]] const T& get() const { return data_; }
  [[nodiscard]] T& get() { return data_; }

  auto operator->() {
    if constexpr (std::is_pointer_v<T>) {
      return data_;
    } else {
      return &data_;
    }
  }
  auto operator->() const {
    if constexpr (std::is_pointer_v<T>) {
      return data_;
    } else {
      return &data_;
    }
  }

  void Clear() {
    Destructor destructor(data_);
    destructor();
    data_ = {};
  }

 private:
  T data_{};
};
}  // namespace core

#endif  // GPR924_ENGINE_RESOURCE_H
