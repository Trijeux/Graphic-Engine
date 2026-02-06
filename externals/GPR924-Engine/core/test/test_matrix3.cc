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

#include "gtest/gtest.h"
#include "maths/matrix3.h"

TEST(Matrix3, Determinant) {
  constexpr std::array<std::array<float, 3>, 3> matrix = {{
       {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}
  }};

  constexpr core::Matrix3 m(matrix);
  EXPECT_EQ(m.determinant(), 0);
}

TEST(Matrix3, ZeroMatrix) {
  constexpr core::Matrix3<int> m;
  EXPECT_EQ(m.determinant(), 0);
}

TEST(Matrix3, Transpose) {
  constexpr std::array<std::array<float, 3>, 3> matrix = {{
       {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}
  }};

  constexpr core::Matrix3 m(matrix);
  const auto t = m.Transpose();

  EXPECT_EQ(t(0,1), m(1,0));
  EXPECT_NE(t(0,1), m(0,1));
  EXPECT_EQ(m.determinant(), t.determinant());
}

TEST(Matrix3, Identity) {
  constexpr auto m = core::Matrix3<int>::Identity();

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      EXPECT_EQ(m(i,j), i==j ? 1 : 0);
}

TEST(Matrix3, InverseDeterminantToZero) {
  constexpr core::Matrix3<int> m;
  EXPECT_THROW((void)m.Inverse();, std::domain_error);
}

TEST(Matrix3, InverseOfIdentityIsIdentity) {
  constexpr auto m = core::Matrix3<int>::Identity();

  const auto inv = m.Inverse();
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      EXPECT_EQ(m(i, j), inv(i, j));
}

TEST(Matrix3, Multiplication) {
  constexpr std::array<std::array<float, 3>, 3> a = {{
      {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}
  }};

  constexpr std::array<std::array<float, 3>, 3> b = {{
    {9, 8, 7},
    {6, 5, 4},
    {3, 2, 1}
  }};

  constexpr std::array<std::array<float, 3>, 3> expected = {{
    {30, 24, 18},
    {84, 69, 54},
    {138, 114, 90}
  }};

  constexpr core::Matrix3 A(a);
  constexpr core::Matrix3 B(b);
  const core::Matrix3 result = A * B;
  constexpr core::Matrix3 E(expected);

  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      EXPECT_EQ(result(i, j), E(i, j));
}

TEST(Matrix3, Addition) {
  constexpr std::array<std::array<float, 3>, 3> a = {{
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
  }};

  constexpr std::array<std::array<float, 3>, 3> b = {{
    {9, 8, 7},
    {6, 5, 4},
    {3, 2, 1}
  }};

  constexpr core::Matrix3 A(a);
  constexpr core::Matrix3 B(b);
  const core::Matrix3 result = A + B;

  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      EXPECT_EQ(result(i, j), 10);
}

TEST(Matrix3, Subtraction) {
  constexpr std::array<std::array<float, 3>, 3> a = {{
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
  }};

  constexpr std::array<std::array<float, 3>, 3> b = {{
    {9, 8, 7},
    {6, 5, 4},
    {3, 2, 1}
  }};

  constexpr std::array<std::array<float, 3>, 3> expected = {{
    {1-9,  2-8,  3-7},
    {4-6,  5-5,  6-4},
    {7-3,  8-2,  9-1}
}};

  constexpr core::Matrix3 A(a);
  constexpr core::Matrix3 B(b);
  const core::Matrix3 result = A - B;
  constexpr core::Matrix3 E(expected);

  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      EXPECT_EQ(result(i, j), E(i, j));
}