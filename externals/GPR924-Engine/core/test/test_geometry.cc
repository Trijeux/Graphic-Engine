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

Contributors: Jemoel Ablay
*/

#include "gtest/gtest.h"
#include "maths/geometry.h"
#include "maths/matrix3.h"

TEST(Geometry, CircleContainsPoint) {
  constexpr core::Vec2F center{0.0f, 0.0f};
  constexpr float radius = 5.0f;

  EXPECT_TRUE(CircleContainsPoint(core::Vec2F{3.0f, 4.0f}, center, radius));
  EXPECT_TRUE(CircleContainsPoint(core::Vec2F{5.0f, 0.0f}, center, radius));
  EXPECT_FALSE(CircleContainsPoint(core::Vec2F{6.0f, 0.0f}, center, radius));
}

TEST(Geometry, CircleOverlap) {
  constexpr core::Vec2F c1{0.0f, 0.0f};
  constexpr float r1 = 3.0f;
  core::Vec2F c2{4.0f, 0.0f};
  constexpr float r2 = 2.0f;

  EXPECT_TRUE(CircleOverlap(c1, r1, c2, r2));

  c2 = core::Vec2F{2.0f, 0.0f};
  EXPECT_TRUE(CircleOverlap(c1, r1, c2, r2));

  c2 = core::Vec2F{10.0f, 0.0f};
  EXPECT_FALSE(CircleOverlap(c1, r1, c2, r2));
}

TEST(Geometry, AABBOverlap) {
  constexpr core::Vec2F c1{0.0f, 0.0f};
  constexpr core::Vec2F s1 = {10.f,10.f};
  core::Vec2F c2{4.0f, 0.0f};
  constexpr core::Vec2F s2 = {10.f,10.f};

  EXPECT_TRUE(AABBOverlap(c1, s1, c2, s2));

  c2 = core::Vec2F{2.0f, 0.0f};
  EXPECT_TRUE(AABBOverlap(c1, s1, c2, s2));

  c2 = core::Vec2F{30.0f, 0.0f};
  EXPECT_FALSE(AABBOverlap(c1, s1, c2, s2));
}

TEST(Geometry, CircleAABBOverlap) {
  constexpr core::Vec2F c1{0.0f, 0.0f};
  constexpr core::Vec2F s1 = {10.f,10.f};
  core::Vec2F c2{4.0f, 0.0f};
  constexpr float radius = 10.f;

  EXPECT_TRUE(AABBCircleOverlap(c1, s1, c2, radius));

  c2 = core::Vec2F{2.0f, 0.0f};
  EXPECT_TRUE(AABBCircleOverlap(c1, s1, c2, radius));

  c2 = core::Vec2F{30.0f, 0.0f};
  EXPECT_FALSE(AABBCircleOverlap(c1, s1, c2, radius));
}