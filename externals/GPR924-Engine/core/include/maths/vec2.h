#ifndef CORE_MATHS_VEC2_H
#define CORE_MATHS_VEC2_H

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

Contributors: Maximilien
Contributors: Anthony Barman
Contributors: Jemoel
*/

#include <cmath>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <string>

#include "angle.h"

namespace core {
// Templates allow to write code for any type of data
// Typename is a parameter for templates that takes in any type of data
template <typename Vector2T, typename T>
// Concept is a named set of requirements that a type must satisfy to be used in
// a given context This concept checks if the type is a vector 2 and if both
// points of the vectors match to the type T
concept IsVector2 = requires(Vector2T v) {
  { v.x } -> std::same_as<T&>;
  { v.y } -> std::same_as<T&>;
};

template <typename Vector2T, typename T>
// This concept checks if both points of the vector 2 can be converted to the
// type T
concept IsConvertibleVector2 = requires(Vector2T v) {
  { v.x } -> std::convertible_to<T&>;
  { v.y } -> std::convertible_to<T&>;
};

template <typename T>
  requires std::is_arithmetic_v<T>
// Sets up the struct of Vector 2 by giving it two points
struct Vec2 {
  T x{}, y{};
  // Compiler gives the default body in case none is given
  constexpr Vec2() = default;

  // Constructor for the vec 2
  constexpr Vec2(T x, T y) : x(x), y(y) {}

  template <typename VectorT>
    requires IsVector2<VectorT, T>
  constexpr explicit Vec2(VectorT v) : x(v.x), y(v.y) {}

  // Converts the vec2 to a given new data type
  template <typename VectorT>
  // Verify beforehand if it is possible to convert by using previous concept
    requires(IsConvertibleVector2<VectorT, T> && !IsVector2<VectorT, T>)
  constexpr explicit Vec2(VectorT v)
      : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

  // nodiscard makes it so the result of the function is not ignored (Safety
  // purposes) constexpr makes it so that the result can be determined during
  // compile time operator overloads the normal use of the operator in the case
  // it is used for a Vec2 struct
  [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const {
    return Vec2(x + other.x, y + other.y);
  }

  [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const {
    return Vec2(x - other.x, y - other.y);
  }

  [[nodiscard]] constexpr Vec2 operator-() const { return Vec2(-x, -y); }

  [[nodiscard]] constexpr Vec2 operator*(const Vec2& other) const {
    return {x * other.x, y * other.y};
  }
  [[nodiscard]] constexpr Vec2 operator*(const T t) const {
    return {x * t, y * t};
  }
  [[nodiscard]] friend constexpr Vec2 operator*(const T t, const Vec2& other) {
    return other * t;
  }

  constexpr Vec2& operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  constexpr Vec2& operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  constexpr Vec2& operator*=(const T t) {
    x *= t;
    y *= t;
    return *this;
  }

  [[nodiscard]] bool operator==(const Vec2& vec2) const { return x == vec2.x && y == vec2.y; }

  template <typename OtherT>
  constexpr Vec2& operator=(const Vec2<OtherT>& other) {
    x = static_cast<T>(other.x);
    y = static_cast<T>(other.y);
    return *this;
  }

  [[nodiscard]] constexpr T Dot(const Vec2& other) const {
    return x * other.x + y * other.y;
  }

  [[nodiscard]] static constexpr T Dot(const Vec2& v1, const Vec2& v2) {
    return v1.x * v2.x + v1.y * v2.y;
  }

  [[nodiscard]] constexpr Vec2 operator/(const Vec2& other) const {
    return {x / other.x, y / other.y};
  }
  [[nodiscard]] constexpr Vec2 operator/(const T t) const {
    return {x / t, y / t};
  }

  [[nodiscard]] constexpr T magnitude_sqr() const
    requires std::is_floating_point_v<T>
  {
    return x * x + y * y;
  }
  [[nodiscard]] T magnitude() const
    requires std::is_floating_point_v<T>
  {
    return std::sqrt(magnitude_sqr());
  }

  [[nodiscard]] Vec2 Normalize() const
    requires std::is_floating_point_v<T>
  {
    const T man = magnitude();
    if (man == 0.0f) {
      return Vec2(0, 0);
    }
    return *this / man;
  }

  [[nodiscard]] Vec2 Rotate(const Radian angle) const
    requires std::floating_point<T>
  {
    const T c = std::cos(static_cast<float>(angle));
    const T s = std::sin(static_cast<float>(angle));
    const T new_x = x * c - y * s;
    const T new_y = x * s + y * c;
    return Vec2(new_x, new_y);
  }

  [[nodiscard]] static constexpr Vec2 Lerp(const Vec2 v0, const Vec2 v1,
                                           const float t)
    requires std::is_floating_point_v<T>
  {
    return Vec2{v0.x * (1 - t) + v1.x * t, v0.y * (1 - t) + v1.y * t};
  }

  [[nodiscard]] static constexpr Vec2 Projection(const Vec2 v,
                                                 const Vec2 onto) {
    T dotVV = onto.Dot(onto);
    if (dotVV == 0) return Vec2{0, 0};
    T factor = v.Dot(onto) / dotVV;
    return Vec2{onto.x * factor, onto.y * factor};
  }

  [[nodiscard]] static constexpr Vec2 Reflection(const Vec2 v,
                                                 const Vec2 normal) {
    T factor = 2 * v.Dot(normal);
    return Vec2{v.x - normal.x * factor, v.y - normal.y * factor};
  }

  [[nodiscard]] constexpr bool Perpendicular(const Vec2 v){
  return Dot(v) == 0;
}

  [[nodiscard]] Vec2 constexpr PerpendicularClockWise() const {
    return Vec2(y, -x);
  }
  [[nodiscard]] Vec2 constexpr PerpendicularCounterClockWise() const {
    return Vec2(-y, x);
  }
};

using Vec2F = Vec2<float>;
using Vec2I = Vec2<int32_t>;
using Vec2U = Vec2<uint32_t>;

}  // namespace core
#endif  // CORE_MATHS_VEC2_H
