//
// Created by WIXXEL on 07.10.2025.
//

#ifndef CORE_MATHS_MATRIX2_H
#define CORE_MATHS_MATRIX2_H

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

Contributors: Matteo
*/

#include <array>
#include <stdexcept>
#include <type_traits>
#include "vec2.h"

namespace core {
template <typename T>
class Matrix2 {
  static_assert(std::is_arithmetic_v<T>,
                "Matrix2 requires arithmetic value type");

 public:
  explicit constexpr Matrix2(const std::array<Vec2<T>, 2>& newMatrix)
      : matrix_(newMatrix) {}
  explicit constexpr Matrix2(Vec2<T> v1, Vec2<T> v2) noexcept : matrix_{v1, v2}{}
  explicit constexpr Matrix2() noexcept : matrix_() {}

  [[nodiscard]] static constexpr Matrix2 Identity() noexcept {
    return Matrix2(std::array<Vec2<T>, 2>{{ Vec2<T>{T(1), T(0)}, Vec2<T>{T(0), T(1)} }});
  }



  [[nodiscard]] constexpr T determinant() const noexcept {
    return matrix_[0].x * matrix_[1].y - matrix_[0].y * matrix_[1].x;
  }

  [[nodiscard]] Matrix2 Transpose() const noexcept {
    Matrix2 result;
    result.matrix_[0].x = matrix_[0].x;
    result.matrix_[0].y = matrix_[1].x;
    result.matrix_[1].x = matrix_[0].y;
    result.matrix_[1].y = matrix_[1].y;
    return result;
  }


  [[nodiscard]] Matrix2 Inverse() const {
    const T det = determinant();
    if (det == static_cast<T>(0))
      throw std::domain_error("Matrix not invertible (determinant = 0)");

    Matrix2 result;
    result(0, 0) = matrix_[1].y;
    result(0, 1) = -matrix_[0].y;
    result(1, 0) = -matrix_[1].x;
    result(1, 1) = matrix_[0].x;

    for (auto& row : result.matrix_) {
      row.x /= det;
      row.y /= det;
    }
    return result;
  }

  template <typename I>
 T& operator()(I i, I j) {
    static_assert(std::is_integral_v<I>, "Indices must be integral");
    auto& row = matrix_[static_cast<size_t>(i)];  // pas const ici !
    switch (static_cast<size_t>(j)) {
      case 0: return row.x;
      case 1: return row.y;
      default: throw std::out_of_range("Matrix2 index out of range");
    }
  }

  template <typename I>
  const T& operator()(I i, I j) const {
    static_assert(std::is_integral_v<I>, "Indices must be integral");
    const auto& row = matrix_[static_cast<size_t>(i)]; // ici c’est bien const
    switch (static_cast<size_t>(j)) {
      case 0: return row.x;
      case 1: return row.y;
      default: throw std::out_of_range("Matrix2 index out of range");
    }
  }


  [[nodiscard]] constexpr Matrix2 operator+(const Matrix2& other) const noexcept {
    Matrix2 result;
    for (size_t i = 0; i < 2; ++i)
      for (size_t j = 0; j < 2; ++j)
        result(i, j) = (*this)(i, j) + other(i, j);
    return result;
  }

  [[nodiscard]] constexpr Matrix2 operator-(const Matrix2& other) const noexcept {
    Matrix2 result;
    for (size_t i = 0; i < 2; ++i)
      for (size_t j = 0; j < 2; ++j)
        result(i, j) = (*this)(i, j) - other(i, j);
    return result;
  }

  [[nodiscard]] constexpr Matrix2 operator*(const Matrix2& other) const noexcept {
    Matrix2 result;
    for (size_t i = 0; i < 2; ++i) {
      for (size_t j = 0; j < 2; ++j) {
        result(i, j) = T(0);
        for (size_t k = 0; k < 2; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

 private:
  std::array<Vec2<T>, 2> matrix_;
};
}  // namespace core::maths

#endif  // CORE_MATHS_MATRIX2_H
