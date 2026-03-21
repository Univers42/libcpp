// ivector3.cpp — IVector3 implementation.

#include "libcpp/math/ivector3.hpp"
#include <cmath>
#include <stdexcept>

namespace libcpp {
namespace math {

IVector3::IVector3(int x, int y, int z) : x(x), y(y), z(z) {}

IVector3 IVector3::operator+(const IVector3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
IVector3 IVector3::operator-(const IVector3& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
IVector3 IVector3::operator*(int s) const { return {x * s, y * s, z * s}; }
IVector3 IVector3::operator/(int s) const
{
    if (s == 0)
        throw std::runtime_error("IVector3: division by zero");
    return {x / s, y / s, z / s};
}

IVector3& IVector3::operator+=(const IVector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
IVector3& IVector3::operator-=(const IVector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
IVector3& IVector3::operator*=(int s) { x *= s; y *= s; z *= s; return *this; }
IVector3& IVector3::operator/=(int s)
{
    if (s == 0)
        throw std::runtime_error("IVector3: division by zero");
    x /= s;
    y /= s;
    z /= s;
    return *this;
}

bool IVector3::operator==(const IVector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
bool IVector3::operator!=(const IVector3& rhs) const { return !(*this == rhs); }

int      IVector3::dot(const IVector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
IVector3 IVector3::cross(const IVector3& rhs) const
{
    return {y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x};
}
double IVector3::length() const { return std::sqrt(static_cast<double>(lengthSquared())); }
int    IVector3::lengthSquared() const { return x * x + y * y + z * z; }

std::ostream& operator<<(std::ostream& os, const IVector3& v)
{
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace math
} // namespace libcpp
