#pragma once
// ivector2.hpp — 2D integer vector with arithmetic operators.

#include <cstdint>
#include <iostream>

namespace libcpp {
namespace math {

/// @brief 2D integer vector with arithmetic, dot, cross, and length.
struct IVector2 {
  int x = 0;
  int y = 0;

  IVector2() = default;
  IVector2(int x, int y);

  IVector2 operator+(const IVector2& rhs) const;
  IVector2 operator-(const IVector2& rhs) const;
  IVector2 operator*(int scalar) const;
  IVector2 operator/(int scalar) const;

  IVector2& operator+=(const IVector2& rhs);
  IVector2& operator-=(const IVector2& rhs);
  IVector2& operator*=(int scalar);
  IVector2& operator/=(int scalar);

  bool operator==(const IVector2& rhs) const;
  bool operator!=(const IVector2& rhs) const;

  [[nodiscard]] int dot(const IVector2& rhs) const;
  [[nodiscard]] int cross(
      const IVector2& rhs) const;  // 2D cross product(scalar)
  [[nodiscard]] double length() const;
  [[nodiscard]] int lengthSquared() const;

  friend std::ostream& operator<<(std::ostream& os, const IVector2& v);
};

}  // namespace math
}  // namespace libcpp
