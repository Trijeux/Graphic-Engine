#ifndef CORE_MATH_MATRIX3_H
#define CORE_MATH_MATRIX3_H

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

Contributors: Jemoel
*/

#include <array>

#include "matrix2.h"
#include "vec3.h"

namespace core {
template <typename T>
class Matrix3 {
  static_assert(std::is_arithmetic_v<T>,
                "Matrix3 requires arithmetic value type");

 public:
  explicit constexpr Matrix3(const std::array<std::array<T, 3>, 3>& matrix) {
    new_matrix_ = {
      Vec3<T>(matrix[0][0], matrix[0][1], matrix[0][2]),
      Vec3<T>(matrix[1][0], matrix[1][1], matrix[1][2]),
      Vec3<T>(matrix[2][0], matrix[2][1], matrix[2][2])
    };
  }
  explicit constexpr Matrix3(Vec3<T> v1, Vec3<T> v2, Vec3<T> v3) : new_matrix_{v1, v2, v3} {}
  explicit constexpr Matrix3() noexcept : new_matrix_{} {}

  [[nodiscard]] static constexpr Matrix3 Identity() noexcept {
    return Matrix3(std::array<std::array<T, 3>, 3>{{
        { T(1), T(0), T(0) },
        { T(0), T(1), T(0) },
        { T(0), T(0), T(1) }
    }});
  }

  [[nodiscard]] constexpr T determinant() const noexcept {
    return new_matrix_[0].Dot(new_matrix_[1].Cross(new_matrix_[2]));
  }

  [[nodiscard]] constexpr Matrix3 Transpose() const noexcept {
    return Matrix3(
        Vec3<T>(new_matrix_[0].x, new_matrix_[1].x, new_matrix_[2].x),
        Vec3<T>(new_matrix_[0].y, new_matrix_[1].y, new_matrix_[2].y),
        Vec3<T>(new_matrix_[0].z, new_matrix_[1].z, new_matrix_[2].z)
    );
  }

  [[nodiscard]] Matrix3 Inverse() const {
    const T det = determinant();
    if (det == static_cast<T>(0))
      throw std::domain_error("Matrix not invertible (determinant = 0)");

    Matrix3 cof;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        Matrix2<T> minor;
        int mi = 0, mj = 0;
        for (int r = 0; r < 3; ++r) {
          if (r == static_cast<int>(i)) continue;
          mj = 0;
          for (int c = 0; c < 3; ++c) {
            if (c == static_cast<int>(j)) continue;
            minor(mi, mj++) = (*this)(r, c);
          }
          mi++;
        }

        T sign = (i + j) % 2 == 0 ? static_cast<T>(1) : static_cast<T>(-1);

        cof(i, j) = sign * minor.determinant();
      }
    }

    Matrix3 adj = cof.Transpose();

    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        adj(i, j) /= det;

    return adj;
  }

  template <typename I>
  constexpr T& operator()(I i, I j) {
    static_assert(std::is_integral_v<I>, "Indices must be integral");
    auto& row = new_matrix_[static_cast<size_t>(i)];
    switch (j) {
      case 0: return row.x;
      case 1: return row.y;
      case 2: return row.z;
      default: throw std::out_of_range("Vec3 index");
    }
  }

  template <typename I>
  constexpr const T& operator()(I i, I j) const {
    static_assert(std::is_integral_v<I>, "Indices must be integral");
    const auto& row = new_matrix_[static_cast<size_t>(i)];
    switch (j) {
      case 0: return row.x;
      case 1: return row.y;
      case 2: return row.z;
      default: throw std::out_of_range("Vec3 index");
    }
  }

  [[nodiscard]] Matrix3 operator+(const Matrix3& other) const noexcept {
    Matrix3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result(i, j) = (*this)(i, j) + other(i, j);
      }
    }
    return result;
  }

  [[nodiscard]] Matrix3 operator-(const Matrix3& other) const noexcept {
    Matrix3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        result(i, j) = (*this)(i, j) - other(i, j);
      }
    }
    return result;
  }

  [[nodiscard]] Matrix3 operator*(const Matrix3& other) const noexcept {
    Matrix3 result;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        for (int k = 0; k < 3; ++k)
          result(i, j) += (*this)(i, k) * other(k, j);
      }
    }
    return result;
  }

 private:
  std::array<Vec3<T>, 3> new_matrix_;
};
}  // namespace core::maths

#endif  // CORE_MATH_MATRIX3_H