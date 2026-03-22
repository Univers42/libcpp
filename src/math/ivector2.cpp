// ivector2.cpp — IVector2 implementation.

#include "libcpp/math/ivector2.hpp"
#include <cmath>
#include <stdexcept>

namespace libcpp {
namespace math {

IVector2::IVector2(int x, int y) : x(x), y(y) {}

IVector2 IVector2::operator+(const IVector2& rhs) const {
  return {x + rhs.x, y + rhs.y};
}
IVector2 IVector2::operator-(const IVector2& rhs) const {
  return {x - rhs.x, y - rhs.y};
}
IVector2 IVector2::operator*(int s) const { return {x * s, y * s}; }
IVector2 IVector2::operator/(int s) const {
  if (s == 0) throw std::runtime_error("IVector2: division by zero");
  return {x / s, y / s};
}

IVector2& IVector2::operator+=(const IVector2& rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}
IVector2& IVector2::operator-=(const IVector2& rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}
IVector2& IVector2::operator*=(int s) {
  x *= s;
  y *= s;
  return *this;
}
IVector2& IVector2::operator/=(int s) {
  if (s == 0) throw std::runtime_error("IVector2: division by zero");
  x /= s;
  y /= s;
  return *this;
}

bool IVector2::operator==(const IVector2& rhs) const {
  return x == rhs.x && y == rhs.y;
}
bool IVector2::operator!=(const IVector2& rhs) const { return !(*this == rhs); }

int IVector2::dot(const IVector2& rhs) const { return x * rhs.x + y * rhs.y; }
int IVector2::cross(const IVector2& rhs) const { return x * rhs.y - y * rhs.x; }
double IVector2::length() const {
  return std::sqrt(static_cast<double>(lengthSquared()));
}
int IVector2::lengthSquared() const { return x * x + y * y; }

std::ostream& operator<<(std::ostream& os, const IVector2& v) {
  return os << "(" << v.x << ", " << v.y << ")";
}

}  // namespace math
}  // namespace libcpp
