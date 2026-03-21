#pragma once
// ivector3.hpp — 3D integer vector with arithmetic operators.

#include <cstdint>
#include <iostream>

namespace libcpp {
namespace math {

struct IVector3 {
    int x = 0;
    int y = 0;
    int z = 0;

    IVector3() = default;
    IVector3(int x, int y, int z);

    IVector3 operator+(const IVector3& rhs) const;
    IVector3 operator-(const IVector3& rhs) const;
    IVector3 operator*(int scalar) const;
    IVector3 operator/(int scalar) const;

    IVector3& operator+=(const IVector3& rhs);
    IVector3& operator-=(const IVector3& rhs);
    IVector3& operator*=(int scalar);
    IVector3& operator/=(int scalar);

    bool operator==(const IVector3& rhs) const;
    bool operator!=(const IVector3& rhs) const;

    int      dot(const IVector3& rhs) const;
    IVector3 cross(const IVector3& rhs) const;
    double   length() const;
    int      lengthSquared() const;

    friend std::ostream& operator<<(std::ostream& os, const IVector3& v);
};

} // namespace math
} // namespace libcpp
