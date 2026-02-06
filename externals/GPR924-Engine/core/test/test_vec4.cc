//
// Contributor Yanis Fornari
//
#include <gtest/gtest.h>

#include "maths/vec4.h"

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

Contributors: Yanis Fornari
Contributors: Anthony Barman
*/

TEST(Vec4, Constructor) {
  constexpr core::Vec4I vec_i(1, -2, 3, 6);
  EXPECT_EQ(vec_i.x, 1);
  EXPECT_EQ(vec_i.y, -2);
  EXPECT_EQ(vec_i.z, 3);
  EXPECT_EQ(vec_i.w, 6);
  constexpr core::Vec4F vec_f(1.5f, 2.5f, 3.5f, 6.5f);
  EXPECT_FLOAT_EQ(vec_f.x, 1.5f);
  EXPECT_FLOAT_EQ(vec_f.y, 2.5f);
  EXPECT_FLOAT_EQ(vec_f.z, 3.5f);
  EXPECT_FLOAT_EQ(vec_f.w, 6.5f);
  constexpr core::Vec4U vec_u(1, 2, 3, 6);
  EXPECT_EQ(vec_u.x, 1);
  EXPECT_EQ(vec_u.y, 2);
  EXPECT_EQ(vec_u.z, 3);
  EXPECT_EQ(vec_u.w, 6);

  // La conversion est à l’inférieur.
  core::Vec4I vec_i2(0, 0, 0, 0);
  constexpr core::Vec4F vec_f2(1.5f, 2.5f, 3.5f, 4.5f);
  vec_i2 = vec_f2;
  EXPECT_EQ(vec_i2.x, 1);
  EXPECT_EQ(vec_i2.y, 2);
  EXPECT_EQ(vec_i2.z, 3);
  EXPECT_EQ(vec_i2.w, 4);
}

TEST(Vec4, add) {
  constexpr core::Vec4I vec1(1, 2, 3, 6);
  constexpr core::Vec4I vec2(4, 5, 6, 7);
  constexpr core::Vec4I vec3 = vec1 + vec2;
  EXPECT_EQ(vec3.x, 5);
  EXPECT_EQ(vec3.y, 7);
  EXPECT_EQ(vec3.z, 9);
  EXPECT_EQ(vec3.w, 13);
  constexpr core::Vec4F vec_f1(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f2(4.3f, 5.3f, 6.3f, 7.3f);
  constexpr core::Vec4F vec_f3 = vec_f1 + vec_f2;
  EXPECT_FLOAT_EQ(vec_f3.x, 5.5f);
  EXPECT_FLOAT_EQ(vec_f3.y, 7.5f);
  EXPECT_FLOAT_EQ(vec_f3.z, 9.5f);
  EXPECT_FLOAT_EQ(vec_f3.w, 13.5f);
}

TEST(Vec4, Sub) {
  constexpr core::Vec4I vec1(4, 5, 6, 7);
  constexpr core::Vec4I vec2(1, 2, 3, 6);
  constexpr core::Vec4I vec3 = vec1 - vec2;
  EXPECT_EQ(vec3.x, 3);
  EXPECT_EQ(vec3.y, 3);
  EXPECT_EQ(vec3.z, 3);
  EXPECT_EQ(vec3.w, 1);
  constexpr core::Vec4F vec_f1(4.5f, 5.5f, 6.5f, 7.5f);
  constexpr core::Vec4F vec_f2(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f3 = vec_f1 - vec_f2;
  EXPECT_FLOAT_EQ(vec_f3.x, 3.3f);
  EXPECT_FLOAT_EQ(vec_f3.y, 3.3f);
  EXPECT_FLOAT_EQ(vec_f3.z, 3.3f);
  EXPECT_FLOAT_EQ(vec_f3.w, 1.3f);
}

TEST(Vec4, Muli) {
  constexpr core::Vec4I vec1(4, 5, 6, 7);

  constexpr auto result_int = vec1 * 2;
  EXPECT_EQ(result_int.x, 4 * 2);
  EXPECT_EQ(result_int.y, 5 * 2);
  EXPECT_EQ(result_int.z, 6 * 2);
  EXPECT_EQ(result_int.w, 7 * 2);

  constexpr core::Vec4I vec2(1, 2, 3, 6);
  constexpr core::Vec4I vec12 = vec1 * vec2;
  EXPECT_EQ(vec12.x, 1 * 4);
  EXPECT_EQ(vec12.y, 2 * 5);
  EXPECT_EQ(vec12.z, 3 * 6);
  EXPECT_EQ(vec12.w, 7 * 6);

  constexpr core::Vec4I vec3 = 2 * vec2;
  EXPECT_EQ(vec3.x, 1 * 2);
  EXPECT_EQ(vec3.y, 2 * 2);
  EXPECT_EQ(vec3.z, 3 * 2);
  EXPECT_EQ(vec3.w, 6 * 2);

  constexpr core::Vec4F vec_f1(4.3f, 5.3f, 6.3f, 7.3f);

  constexpr auto result_float = vec_f1 * 2;
  EXPECT_FLOAT_EQ(result_float.x, 4.3f * 2);
  EXPECT_FLOAT_EQ(result_float.y, 5.3f * 2);
  EXPECT_FLOAT_EQ(result_float.z, 6.3f * 2);
  EXPECT_FLOAT_EQ(result_float.w, 7.3f * 2);

  constexpr core::Vec4F vec_f2(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f12 = vec_f1 * vec_f2;
  EXPECT_FLOAT_EQ(vec_f12.x, 4.3f * 1.2f);
  EXPECT_FLOAT_EQ(vec_f12.y, 5.3f * 2.2f);
  EXPECT_FLOAT_EQ(vec_f12.z, 6.3f * 3.2f);
  EXPECT_FLOAT_EQ(vec_f12.w, 7.3f * 6.2f);

  constexpr core::Vec4F vec_f3 = 2 * vec_f2;
  EXPECT_FLOAT_EQ(vec_f3.x, 1.2f * 2);
  EXPECT_FLOAT_EQ(vec_f3.y, 2.2f * 2);
  EXPECT_FLOAT_EQ(vec_f3.z, 3.2f * 2);
  EXPECT_FLOAT_EQ(vec_f3.w, 6.2f * 2);
}

TEST(Vec4, Dot) {
  constexpr core::Vec4I vec1(4, 5, 6, 7);
  constexpr core::Vec4I vec2(1, 2, 3, 6);
  EXPECT_EQ(vec1.Dot(vec2), 4 * 1 + 5 * 2 + 6 * 3 + 7 * 6);
  EXPECT_EQ(core::Vec4I::Dot(vec1, vec2), 4 * 1 + 5 * 2 + 6 * 3 + 7 * 6);

  constexpr core::Vec4F vec_f1(4.3f, 5.3f, 6.3f, 7.3f);
  constexpr core::Vec4F vec_f2(1.2f, 2.2f, 3.2f, 6.2f);
  EXPECT_FLOAT_EQ(vec_f1.Dot(vec_f2),
                  4.3f * 1.2f + 5.3f * 2.2f + 6.3f * 3.2f + 7.3f * 6.2f);
  EXPECT_FLOAT_EQ(core::Vec4F::Dot(vec_f1, vec_f2),
                  4.3f * 1.2f + 5.3f * 2.2f + 6.3f * 3.2f + 7.3f * 6.2f);
}

TEST(Vec4, Div) {
  constexpr core::Vec4I vec1(4, 5, 6, 7);

  constexpr auto result_int = vec1 / 2;
  EXPECT_EQ(result_int.x, 4 / 2);
  EXPECT_EQ(result_int.y, 5 / 2);
  EXPECT_EQ(result_int.z, 6 / 2);
  EXPECT_EQ(result_int.w, 7 / 2);

  constexpr core::Vec4I vec2(1, 2, 3, 6);
  constexpr core::Vec4I vec3 = vec1 / vec2;
  EXPECT_EQ(vec3.x, 4 / 1);
  EXPECT_EQ(vec3.y, 5 / 2);
  EXPECT_EQ(vec3.z, 6 / 3);
  EXPECT_EQ(vec3.w, 7 / 6);

  constexpr core::Vec4F vec_f1(4.3f, 5.3f, 6.3f, 7.3f);

  constexpr auto result_float = vec_f1 / 2;
  EXPECT_FLOAT_EQ(result_float.x, 4.3f / 2);
  EXPECT_FLOAT_EQ(result_float.y, 5.3f / 2);
  EXPECT_FLOAT_EQ(result_float.z, 6.3f / 2);
  EXPECT_FLOAT_EQ(result_float.w, 7.3f / 2);

  constexpr core::Vec4F vec_f2(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f3 = vec_f1 / vec_f2;
  EXPECT_FLOAT_EQ(vec_f3.x, 4.3f / 1.2f);
  EXPECT_FLOAT_EQ(vec_f3.y, 5.3f / 2.2f);
  EXPECT_FLOAT_EQ(vec_f3.z, 6.3f / 3.2f);
  EXPECT_FLOAT_EQ(vec_f3.w, 7.3f / 6.2f);
}

TEST(Vec4, MagnitudeSqu) {
  constexpr core::Vec4F vec_1(1.2f, 2.2f, 3.2f, 6.2f);
  const auto result_float = vec_1.MagnitudeSqr();
  EXPECT_FLOAT_EQ(result_float,
                  1.2f * 1.2f + 2.2f * 2.2f + 3.2f * 3.2f + 6.2f * 6.2f);
}

TEST(Vec4, Magnitude) {
  constexpr core::Vec4F vec_1(1.2f, 2.2f, 3.2f, 6.2f);
  const auto result_float = vec_1.Magnitude();
  EXPECT_FLOAT_EQ(result_float, std::sqrt(vec_1.MagnitudeSqr()));
}

TEST(Vec4, Normalize) {
  constexpr core::Vec4F vec_1(1.2f, 2.2f, 3.2f, 6.2f);
  const auto result_float = vec_1.Normalize();

  const float result_test = vec_1.Magnitude();
  core::Vec4F vec_1_test(0, 0, 0, 0);
  if (result_test != 0.0f) {
    vec_1_test = vec_1 / result_test;
  }

  EXPECT_FLOAT_EQ(result_float.x, vec_1_test.x);
  EXPECT_FLOAT_EQ(result_float.y, vec_1_test.y);
  EXPECT_FLOAT_EQ(result_float.z, vec_1_test.z);
  EXPECT_FLOAT_EQ(result_float.w, vec_1_test.w);
}

TEST(Vec4, Lerp) {
  constexpr core::Vec4F vec_1(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_2(4.3f, 5.3f, 6.3f, 7.3f);
  constexpr auto result_ = core::Vec4F::Lerp(vec_1, vec_2, 0.5f);
  EXPECT_FLOAT_EQ(result_.x, 1.2f * (1 - 0.5f) + 4.3f * 0.5f);
  EXPECT_FLOAT_EQ(result_.y, 2.2f * (1 - 0.5f) + 5.3f * 0.5f);
  EXPECT_FLOAT_EQ(result_.z, 3.2f * (1 - 0.5f) + 6.3f * 0.5f);
  EXPECT_FLOAT_EQ(result_.w, 6.2f * (1 - 0.5f) + 7.3f * 0.5f);
}

TEST(Vec4, Projection) {
  constexpr core::Vec4I vec1(1, 2, 3, 6);
  constexpr core::Vec4I vec2(4, 5, 6, 7);

  constexpr int factor = vec1.Dot(vec2) / vec2.Dot(vec2);

  constexpr core::Vec4I vec12 =
      core::Vec4I::Projection(vec1, vec2);
  constexpr core::Vec4I vec_test(4 * factor, 5 * factor, 6 * factor,
                                        7 * factor);
  EXPECT_EQ(vec12.x, vec_test.x);
  EXPECT_EQ(vec12.y, vec_test.y);
  EXPECT_EQ(vec12.z, vec_test.z);
  EXPECT_EQ(vec12.w, vec_test.w);

  constexpr core::Vec4F vec_f1(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f2(4.3f, 5.3f, 6.3f, 7.3f);

  constexpr float factor_f = vec_f1.Dot(vec_f2) / vec_f2.Dot(vec_f2);

  constexpr core::Vec4F vec_f12 =
      core::Vec4F::Projection(vec_f1, vec_f2);
  constexpr core::Vec4F vec__f_test(4.3f * factor_f, 5.3f * factor_f,
                                           6.3f * factor_f, 7.3f * factor_f);
  EXPECT_EQ(vec_f12.x, vec__f_test.x);
  EXPECT_EQ(vec_f12.y, vec__f_test.y);
  EXPECT_EQ(vec_f12.z, vec__f_test.z);
  EXPECT_EQ(vec_f12.w, vec__f_test.w);
}

TEST(Vec4, Reflection) {
  constexpr core::Vec4I vec1(1, 2, 3, 6);
  constexpr core::Vec4I vec2(4, 5, 6, 7);

  constexpr int factor = 2 * vec1.Dot(vec2);

  constexpr core::Vec4I vec12 =
      core::Vec4I::Reflection(vec1, vec2);
  constexpr core::Vec4I vec_test(1 - 4 * factor, 2 - 5 * factor,
                                        3 - 6 * factor, 6 - 7 * factor);
  EXPECT_EQ(vec12.x, vec_test.x);
  EXPECT_EQ(vec12.y, vec_test.y);
  EXPECT_EQ(vec12.z, vec_test.z);
  EXPECT_EQ(vec12.w, vec_test.w);

  constexpr core::Vec4F vec_f1(1.2f, 2.2f, 3.2f, 6.2f);
  constexpr core::Vec4F vec_f2(4.3f, 5.3f, 6.3f, 7.3f);

  constexpr float factor_f = 2 * vec_f1.Dot(vec_f2);

  constexpr core::Vec4F vec_f12 =
      core::Vec4F::Reflection(vec_f1, vec_f2);
  constexpr core::Vec4F vec_f_test(
      1.2f - 4.3f * factor_f, 2.2f - 5.3f * factor_f, 3.2f - 6.3f * factor_f,
      6.2f - 7.3f * factor_f);
  EXPECT_EQ(vec_f12.x, vec_f_test.x);
  EXPECT_EQ(vec_f12.y, vec_f_test.y);
  EXPECT_EQ(vec_f12.z, vec_f_test.z);
  EXPECT_EQ(vec_f12.w, vec_f_test.w);
}