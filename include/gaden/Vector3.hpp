#pragma once

#include <iostream>

#include "gaden/Constants.hpp"

namespace gaden {

class Vector3 {
    double m_x, m_y, m_z;
public:

    Vector3(double x_in, double y_in, double z_in):
        m_x(x_in),
        m_y(y_in),
        m_z(z_in)
    {}
    Vector3():
        m_x(0.0),
        m_y(0.0),
        m_z(0.0)
    {}

    double x() const { return m_x; }
    double& x() { return m_x; }
    double y() const { return m_y; }
    double& y() { return m_y; }
    double z() const { return m_z; }
    double& z() { return m_z; }

    // Unary operations

    // Magnitude, squared x^2,y^2,z^2
    double magSqr() const {
        return m_x*m_x + m_y*m_y + m_z*m_z;
    }

    // Magnitude (i.e. length)
    double mag() const {
        return std::sqrt(magSqr());
    }

    // Turn into a unit vector, returns false if magnitude is near-zero
    bool normalise() {
        double m = mag();
        if (abs(1.0 - m) < constants::doubleSmall) {
            return true;
        }
        if (m < constants::doubleSmall) {
            return false;
        }
        double invMag = 1.0/m;
        operator*=(invMag);
        return true;
    }

    // *** Operations

    // * Scalars

    Vector3& operator*=(double val) {
        m_x *= val;
        m_y *= val;
        m_z *= val;
        return *this;
    }

    Vector3& operator/=(double val) {
        m_x /= val;
        m_y /= val;
        m_z /= val;
        return *this;
    }

    // * Vectors

    Vector3& operator+=(const Vector3& vec) {
        m_x += vec.m_x;
        m_y += vec.m_y;
        m_z += vec.m_z;
        return *this;
    }

    Vector3& operator-=(const Vector3& vec) {
        m_x -= vec.m_x;
        m_y -= vec.m_y;
        m_z -= vec.m_z;
        return *this;
    }

    Vector3 operator+(const Vector3& vec) const {
        return Vector3(*this) += vec;
    }

    Vector3 operator-(const Vector3& vec) const {
        return Vector3(*this) -= vec;
    }

    Vector3 operator*(double scalar) const {
        return Vector3(m_x*scalar, m_y*scalar, m_z*scalar);
    }

    Vector3 operator/(double scalar) const {
        return Vector3(m_x/scalar, m_y/scalar, m_z/scalar);
    }

    // Returns result=(*this) crossProduct rhs
    Vector3 crossProduct(const Vector3& rhs) const {
        return Vector3(
            m_y*rhs.m_z - m_z*rhs.m_y,
            m_z*rhs.m_x - m_x*rhs.m_z,
            m_x*rhs.m_y - m_y*rhs.m_x
        );
    }

    // Returns result=(*this) dotProduct rhs
    double dotProduct(const Vector3& rhs) const {
        return m_x*rhs.m_x + m_y*rhs.m_y + m_z*rhs.m_z;
    }

    // *** Stream

    friend std::ostream& operator<<(std::ostream& os, const Vector3& c) {
        return os << "(" << c.m_x << "," << c.m_y<< "," << c.m_z << ")";
    }

    friend std::istream& operator>>(std::istream& is, Vector3& c) {
        char comma;
        return (is >> c.m_x >> comma >> c.m_y >> comma >> c.m_z);
    }
};

} // end namespace gaden
