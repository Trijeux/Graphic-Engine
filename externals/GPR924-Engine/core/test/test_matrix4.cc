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

Contributors: Hugo
Contributors: Bajamin
*/

#include "gtest/gtest.h"
#include "maths/matrix4.h"

TEST(Matrix4DeterminantTest, IdentityMatrix) {
  std::array<std::array<float, 4>, 4> mat = {{
    {{1, 0, 0, 0}},
    {{0, 1, 0, 0}},
    {{0, 0, 1, 0}},
    {{0, 0, 0, 1}}
  }};
  core::Matrix4<float> m(mat);
  EXPECT_FLOAT_EQ(m.determinant(), 1.0f);
}

TEST(Matrix4DeterminantTest, ZeroMatrix) {
  std::array<std::array<float, 4>, 4> mat = {{
    {{0, 0, 0, 0}},
    {{0, 0, 0, 0}},
    {{0, 0, 0, 0}},
    {{0, 0, 0, 0}}
  }};
  core::Matrix4<float> m(mat);
  EXPECT_FLOAT_EQ(m.determinant(), 0.0f);
}

TEST(Matrix4DeterminantTest, DiagonalMatrix) {
  std::array<std::array<float, 4>, 4> mat = {{
    {{2, 0, 0, 0}},
    {{0, 3, 0, 0}},
    {{0, 0, 4, 0}},
    {{0, 0, 0, 5}}
  }};
  core::Matrix4<float> m(mat);
  EXPECT_FLOAT_EQ(m.determinant(), 120.0f);
}

TEST(Matrix4DeterminantTest, KnownMatrix) {
  std::array<std::array<size_t, 4>, 4> mat = {{
    {{3, 2, 0, 1}},
    {{4, 0, 1, 2}},
    {{3, 0, 2, 1}},
    {{9, 2, 3, 1}}
  }};
  core::Matrix4<size_t> m(mat);
  EXPECT_EQ(m.determinant(), 24);
}

TEST(Matrix4Test, InverseOfIdentityIsIdentity) {
  std::array<std::array<float, 4>, 4> identity = {{
    {{1, 0, 0, 0}},
    {{0, 1, 0, 0}},
    {{0, 0, 1, 0}},
    {{0, 0, 0, 1}}
  }};
  core::Matrix4<float> mat(identity);
  auto inv = mat.inverse();
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      EXPECT_FLOAT_EQ(inv.GetMatrix()[i][j], identity[i][j]);
}

TEST(Matrix4Test, InverseMultipliedByOriginalGivesIdentity) {
  std::array<std::array<float, 4>, 4> data = {{
    {{4, 7, 2, 0}},
    {{3, 6, 1, 0}},
    {{2, 5, 1, 0}},
    {{0, 0, 0, 1}}
  }};
  core::Matrix4<float> mat(data);
  auto inv = mat.inverse();
  auto result = mat * inv;
  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      EXPECT_NEAR(result.GetMatrix()[i][j], (i == j) ? 1.0f : 0.0f, 1e-5f);
}

TEST(Matrix4Test, TransposeWorks) {
  std::array<std::array<float, 4>, 4> arr = {{
    {{1, 2, 3, 4}},
    {{5, 6, 7, 8}},
    {{9, 10, 11, 12}},
    {{13, 14, 15, 16}}
  }};
  core::Matrix4<float> mat(arr);
  auto transposed = mat.transpose();

  for (size_t i = 0; i < 4; ++i)
    for (size_t j = 0; j < 4; ++j)
      EXPECT_FLOAT_EQ(transposed.GetMatrix()[i][j], arr[j][i]);
}
