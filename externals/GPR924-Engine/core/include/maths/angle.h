#ifndef CORE_MATHS_ANGLE_H
#define CORE_MATHS_ANGLE_H

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

#include <numbers>

namespace core {
struct Degree;

struct Radian {
  constexpr explicit Radian(const float value) : value(value) {}

  explicit Radian(const Degree& degree);

  explicit constexpr operator float() const { return value; }

  [[nodiscard]] float Value() const { return value; };

 private:
  float value;
};

struct Degree {
  constexpr explicit Degree(const float value) : value(value) {}

  explicit Degree(const Radian& radian)
      : value(radian.Value() * 180.0f / std::numbers::pi_v<float>) {}

  explicit constexpr operator float() const { return value; }

  [[nodiscard]] float Value() const { return value; }

 private:
  float value;
};

inline Radian::Radian(const Degree& degree)
    : value(degree.Value() * std::numbers::pi_v<float> / 180.0f) {}
}  // namespace core::maths

#endif  // CORE_MATHS_ANGLE_H