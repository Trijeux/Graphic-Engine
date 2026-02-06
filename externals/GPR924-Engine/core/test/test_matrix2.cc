//
// Created by WIXXEL on 07.10.2025.
//

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

#include "gtest/gtest.h"
#include "maths/matrix2.h"
#include "maths/vec2.h"

using core::Matrix2;
using core::Vec2;

TEST(Matrix2, Determinant) {
    constexpr Matrix2<float> m(Vec2<float>{1, 2}, Vec2<float>{3, 4});
    EXPECT_EQ(m.determinant(), -2);
}

TEST(Matrix2, ZeroMatrix) {
    constexpr Matrix2<int> m;
    EXPECT_EQ(m.determinant(), 0);
}

TEST(Matrix2, Transpose) {
    constexpr Matrix2<float> m(Vec2<float>{1, 2}, Vec2<float>{3, 4});
    const auto t = m.Transpose();

    EXPECT_EQ(t(0, 1), m(1, 0));
    EXPECT_NE(t(0, 1), m(0, 1));
    EXPECT_EQ(m.determinant(), t.determinant());
}

TEST(Matrix2, Identity) {
    constexpr auto m = Matrix2<int>::Identity();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            EXPECT_EQ(m(i,j), i == j ? 1 : 0);
}

TEST(Matrix2, InverseDeterminantToZero) {
    constexpr Matrix2<int> m;
    EXPECT_THROW((void)m.Inverse(), std::domain_error);
}

TEST(Matrix2, InverseOfIdentityIsIdentity) {
    constexpr auto m = Matrix2<int>::Identity();
    const auto inv = m.Inverse();

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            EXPECT_EQ(m(i, j), inv(i, j));
}

TEST(Matrix2, Multiplication) {
    constexpr Matrix2<float> A(Vec2<float>{1, 2}, Vec2<float>{3, 4});
    constexpr Matrix2<float> B(Vec2<float>{2, 0}, Vec2<float>{1, 2});
    constexpr Matrix2<float> expected(Vec2<float>{4, 4}, Vec2<float>{10, 8});

    const auto result = A * B;

    for (size_t i = 0; i < 2; ++i)
        for (size_t j = 0; j < 2; ++j)
            EXPECT_EQ(result(i, j), expected(i, j));
}

TEST(Matrix2, Addition) {
    constexpr Matrix2<float> A(Vec2<float>{1, 2}, Vec2<float>{3, 4});
    constexpr Matrix2<float> B(Vec2<float>{4, 3}, Vec2<float>{2, 1});
    constexpr Matrix2<float> expected(Vec2<float>{5, 5}, Vec2<float>{5, 5});

    const auto result = A + B;

    for (size_t i = 0; i < 2; ++i)
        for (size_t j = 0; j < 2; ++j)
            EXPECT_EQ(result(i, j), expected(i, j));
}

TEST(Matrix2, Subtraction) {
    constexpr Matrix2<float> A(Vec2<float>{1, 2}, Vec2<float>{3, 4});
    constexpr Matrix2<float> B(Vec2<float>{4, 3}, Vec2<float>{2, 1});
    constexpr Matrix2<float> expected(Vec2<float>{-3, -1}, Vec2<float>{1, 3});

    const auto result = A - B;

    for (size_t i = 0; i < 2; ++i)
        for (size_t j = 0; j < 2; ++j)
            EXPECT_EQ(result(i, j), expected(i, j));
}
