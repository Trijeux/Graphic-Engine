#ifndef CORE_MATHS_VEC3_H
#define CORE_MATHS_VEC3_H

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
*/

#include <cmath>
#include <concepts>
#include <cstdint>

#include "maths/angle.h"
#include "maths/constant.h"

namespace core {
template <typename Vector3T, typename T>
concept IsVector3 = requires(Vector3T v) {
  { v.x } -> std::same_as<T&>;
  { v.y } -> std::same_as<T&>;
  { v.z } -> std::same_as<T&>;
};

template <typename Vector3T, typename T>
concept IsConvertibleVector3 = requires(Vector3T v) {
  { v.x } -> std::convertible_to<T&>;
  { v.y } -> std::convertible_to<T&>;
  { v.z } -> std::convertible_to<T&>;
};

template <typename T>
  requires std::is_arithmetic_v<T>
struct Vec3 {
  T x{}, y{}, z{};
  constexpr Vec3() = default;

  constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

  template <typename VectorT>
    requires IsVector3<VectorT, T>
  constexpr explicit Vec3(VectorT v) : x(v.x), y(v.y), z(v.z) {}

  template <typename VectorT>
    requires(IsConvertibleVector3<VectorT, T> && !IsVector3<VectorT, T>)
  constexpr explicit Vec3(VectorT v)
      : x(static_cast<T>(v.x)),
        y(static_cast<T>(v.y)),
        z(static_cast<T>(v.z)) {}

  [[nodiscard]] constexpr Vec3 operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }

  [[nodiscard]] constexpr Vec3 operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }

  [[nodiscard]] constexpr Vec3 operator*(const Vec3& other) const {
    return {x * other.x, y * other.y, z * other.z};
  }

  [[nodiscard]] constexpr Vec3 operator*(const T t) const {
    return {x * t, y * t, z * t};
  }

  [[nodiscard]] friend constexpr Vec3 operator*(const T t, const Vec3& other) {
    return other * t;
  }

  template <typename OtherT>
  constexpr Vec3& operator=(const Vec3<OtherT>& other) {
    x = static_cast<T>(other.x);
    y = static_cast<T>(other.y);
    z = static_cast<T>(other.z);
    return *this;
  }

  [[nodiscard]] constexpr T Dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
  }

  [[nodiscard]] static constexpr T Dot(const Vec3& v1, const Vec3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  [[nodiscard]] constexpr Vec3 Cross(const Vec3& other) const {
    return Vec3{y * other.z - z * other.y, z * other.x - x * other.z,
                x * other.y - y * other.x};
  }

  [[nodiscard]] static constexpr Vec3 Cross(const Vec3& v1, const Vec3& v2) {
    return Vec3{v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
                v1.x * v2.y - v1.y * v2.x};
  }

  [[nodiscard]] constexpr Vec3 operator/(const Vec3& other) const {
    return {x / other.x, y / other.y, z / other.z};
  }

  [[nodiscard]] constexpr Vec3 operator/(const T t) const {
    return {x / t, y / t, z / t};
  }

  [[nodiscard]] constexpr T magnitude_sqr() const
    requires std::is_floating_point_v<T>
  {
    return x * x + y * y + z * z;
  }

  [[nodiscard]] T magnitude() const
    requires std::is_floating_point_v<T>
  {
    return std::sqrt(magnitude_sqr());
  }

  [[nodiscard]] Vec3 Normalize() const
    requires std::is_floating_point_v<T>
  {
    const T man = magnitude();
    if (man == 0.0f) {
      return Vec3(0, 0, 0);
    }
    return *this / man;
  }

  [[nodiscard]] Vec3 Pitch(const Radian angle) const
    requires std::floating_point<T>
  {
    const T c = std::cos(static_cast<float>(angle));
    const T s = std::sin(static_cast<float>(angle));
    const T new_y = y * c - z * s;
    const T new_z = y * s + z * c;
    return Vec3(x, new_y, new_z);
  }

  [[nodiscard]] Vec3 Yaw(const Radian angle) const
    requires std::floating_point<T>
  {
    const T c = std::cos(static_cast<float>(angle));
    const T s = std::sin(static_cast<float>(angle));
    const T new_x = x * c + s * z;
    const T new_z = -x * s + z * c;
    return Vec3(new_x, y, new_z);
  }

  [[nodiscard]] Vec3 Roll(const Radian angle) const
    requires std::floating_point<T>
  {
    const T c = std::cos(static_cast<float>(angle));
    const T s = std::sin(static_cast<float>(angle));
    const T new_x = x * c - y * s;
    const T new_y = x * s + y * c;
    return Vec3(new_x, new_y, z);
  }

  // [[nodiscard]] Vec3 RotateEuler(const float x, const float y, const float z)
  // const requires std::floating_point<T>
  // {
  //
  // }

  [[nodiscard]] static constexpr Vec3 Lerp(const Vec3 v0, const Vec3 v1,
                                           const float t)
    requires std::is_floating_point_v<T>
  {
    return Vec3{
        v0.x * (1 - t) + v1.x * t,
        v0.y * (1 - t) + v1.y * t,
        v0.z * (1 - t) + v1.z * t,
    };
  }

  [[nodiscard]] static Vec3 Slerp(const Vec3 v0, const Vec3 v1, const float t)
    requires std::is_floating_point_v<T>
  {
    T dot = v0.Dot(v1);

    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    const float theta = std::acos(dot);
    const float sinTheta = std::sin(theta);

    if (sinTheta < Epsilon) {
      return Vec3{
          v0.x * (1 - t) + v1.x * t,
          v0.y * (1 - t) + v1.y * t,
          v0.z * (1 - t) + v1.z * t,
      };
    }

    float w0 = std::sin((1 - t) * theta) / sinTheta;
    float w1 = std::sin(t * theta) / sinTheta;

    return Vec3{
        v0.x * w0 + v1.x * w1,
        v0.y * w0 + v1.y * w1,
        v0.z * w0 + v1.z * w1,
    };
  }

  [[nodiscard]] static constexpr Vec3 Projection(const Vec3 v,
                                                 const Vec3 onto) {
    T dotVV = onto.Dot(onto);
    if (dotVV == 0) return Vec3{0, 0, 0};
    T factor = v.Dot(onto) / dotVV;
    return Vec3{
        onto.x * factor,
        onto.y * factor,
        onto.z * factor,
    };
  }

  [[nodiscard]] static constexpr Vec3 Reflection(const Vec3 v,
                                                 const Vec3 normal) {
    T factor = 2 * v.Dot(normal);
    return Vec3{
        v.x - normal.x * factor,
        v.y - normal.y * factor,
        v.z - normal.z * factor,
    };
  }
};
using Vec3F = Vec3<float>;
using Vec3I = Vec3<int32_t>;
using Vec3U = Vec3<uint32_t>;
}  // namespace core::maths

#endif  // CORE_MATHS_VEC3_H
