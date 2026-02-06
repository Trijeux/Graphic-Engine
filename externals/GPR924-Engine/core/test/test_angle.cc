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
#include <numbers>
#include "maths/angle.h"

TEST(AngleConversionTest, DegreeToRadian)
{
  constexpr core::Degree deg90(90.0f);
  const auto rad = core::Radian(deg90);
  EXPECT_NEAR(static_cast<float>(rad), std::numbers::pi_v<float> / 2.0f, 1e-5f);
}

TEST(AngleConversionTest, RadianToDegree)
{
  constexpr core::Radian rad_pi(std::numbers::pi_v<float>);
  const auto deg = core::Degree(rad_pi);
  EXPECT_NEAR(static_cast<float>(deg), 180.0f, 1e-5f);
}

TEST(AngleConversionTest, RoundTripConversion)
{
  constexpr core::Degree original(123.45f);
  const core::Radian rad(original);
  const core::Degree converted(rad);
  EXPECT_NEAR(static_cast<float>(converted), static_cast<float>(original), 1e-5f);
}