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

#include <gtest/gtest.h>

#include "maths/function.h"

class TestOther {
public:
  int numb = 0;

  bool operator==(const TestOther& other) const {
    return numb == other.numb;
  }
  bool operator<(const TestOther& other) const {
    return numb < other.numb;
  }
  bool operator>(const TestOther& other) const {
    return numb > other.numb;
  }
  bool operator<=(const TestOther& other) const {
    return numb <= other.numb;
  }
  bool operator>=(const TestOther& other) const {
    return numb >= other.numb;
  }
};

TEST(Function, GreaterArithmetic) {
  constexpr int first = 5;
  constexpr int second = 6;
  EXPECT_EQ(core::Greater(first, second),6);
  EXPECT_EQ(core::Greater(second * 2, first), 12);
  EXPECT_EQ(core::Greater(first, first), 5);

  constexpr float first_f = 5.f;
  constexpr float second_f = 6.f;
  EXPECT_FLOAT_EQ(core::Greater(first_f, second_f), 6.f);
  EXPECT_FLOAT_EQ(core::Greater(second_f * 2.f, first_f), 12.f);
  EXPECT_FLOAT_EQ(core::Greater(first_f, first_f), 5.f);
}

TEST(Function, GreaterOther) {
  TestOther other1;
  other1.numb = 5;
  TestOther other2;
  other2.numb = 6;

  EXPECT_EQ(core::Greater(other1, other2), other2);

  other2.numb = 5;
  EXPECT_EQ(core::Greater(other1, other2), other1);
}

TEST(Function, LesserArithmetic) {
  constexpr int first = 5;
  constexpr int second = 6;
  EXPECT_EQ(core::Lesser(first, second),5);
  EXPECT_EQ(core::Lesser(second * 2, first), 5);
  EXPECT_EQ(core::Lesser(second, second), 6);

  constexpr float first_f = 5.f;
  constexpr float second_f = 6.f;
  EXPECT_FLOAT_EQ(core::Lesser(first_f, second_f), 5.f);
  EXPECT_FLOAT_EQ(core::Lesser(second_f * 2.f, first_f), 5.f);
  EXPECT_FLOAT_EQ(core::Greater(second_f, second_f), 6.f);
}

TEST(Function, LesserOther) {
  TestOther other1;
  other1.numb = 5;
  TestOther other2;
  other2.numb = 6;

  EXPECT_EQ(core::Lesser(other1, other2), other1);

  other2.numb = 5;

  EXPECT_EQ(core::Lesser(other1, other2), other1);
}