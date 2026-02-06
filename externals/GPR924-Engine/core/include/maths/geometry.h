#ifndef CORE_MATH_GEOMETRY_H
#define CORE_MATH_GEOMETRY_H
#include <algorithm>

#include "vec2.h"

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

Contributors: Jemoel Ablay, Hugo Zeidan
*/

namespace core {
constexpr bool CircleContainsPoint(const Vec2F& point, const Vec2F& center,
                                   const float radius) {
  const Vec2F diff = point - center;
  return diff.magnitude_sqr() <= radius * radius;
}
constexpr bool CircleOverlap(const Vec2F& center1, const float radius1,
                             const Vec2F& center2, const float radius2) {
  const Vec2F diff = center1 - center2;
  const float radiusSum = radius1 + radius2;
  return diff.magnitude_sqr() <= radiusSum * radiusSum;
}

constexpr bool AABBOverlap(const Vec2F pos1, const Vec2F size1,
                           const Vec2F pos2, const Vec2F size2) {
  // Half sizes of both AABBs
  const float halfWidth1 = size1.x * 0.5f;
  const float halfHeight1 = size1.y * 0.5f;
  const float halfWidth2 = size2.x * 0.5f;
  const float halfHeight2 = size2.y * 0.5f;

  // Difference between centers
  float dx = pos1.x - pos2.x;
  float dy = pos1.y - pos2.y;

  if (dx < 0) dx *= -1;
  if (dy < 0) dy *= -1;

  // Check for overlap on both axes
  const bool overlapX = dx <= (halfWidth1 + halfWidth2);
  const bool overlapY = dy <= (halfHeight1 + halfHeight2);

  return overlapX && overlapY;
}

constexpr bool AABBCircleOverlap(const Vec2F& rectCenter, const Vec2F rectSize,
                                 const Vec2F& circleCenter,
                                 const float radius) {
  // Half extents of the AABB
  const float halfWidth = rectSize.x * 0.5f;
  const float halfHeight = rectSize.y * 0.5f;

  // Compute the vector from AABB center to circle center
  const Vec2F diff = circleCenter - rectCenter;

  // Clamp that vector to the box's extents (find the closest point on box)
  const float clampedX = std::clamp(diff.x, -halfWidth, halfWidth);
  const float clampedY = std::clamp(diff.y, -halfHeight, halfHeight);

  // Closest point on the AABB to the circle center
  const Vec2F closestPoint = rectCenter + core::Vec2F(clampedX, clampedY);

  // Distance squared between circle center and closest point
  const Vec2F delta = circleCenter - closestPoint;
  const float distanceSqr = delta.magnitude_sqr();

  // Overlap if distance ≤ radius
  return distanceSqr <= (radius * radius);
}
}  // namespace core

#endif  // CORE_MATH_GEOMETRY_H
