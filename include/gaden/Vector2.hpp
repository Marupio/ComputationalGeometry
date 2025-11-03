#pragma once

#include <cmath>

namespace gaden {

class Vector2 {
    // X coord
    double m_x;

    // Y coord
    double m_y;

    // Optional index into point list
    int m_idx;

public:

    // Null constructor
    Vector2() : m_x(0), m_y(0), m_idx(-1) {}

    // Construct from components
    Vector2(double x, double y, int i=-1) : m_x(x), m_y(y), m_idx(i) {}

    // Accessors
    double x() const { return m_x; }
    double& x() { return m_x; }
    double y() const { return m_y; }
    double& y() { return m_y; }
    int idx() const { return m_idx; }
    int& idx() { return m_idx; }

    // Cross product (2D vector math)
    double crossProduct(const Vector2& rhs) const { return m_x*rhs.m_y - m_y*rhs.m_x; }

    // Dot product (inner product)
    double dotProduct(const Vector2& rhs) const { return m_x*rhs.m_x + m_y*rhs.m_y; }

    // Magnitude, squared
    double magSqr() const { return m_x*m_x + m_y*m_y; }

    // Magnitude (length)
    double mag() const { return std::sqrt(magSqr()); }

    // Binary operations

    Vector2 operator+(const Vector2& rhs) const {
        return Vector2(m_x + rhs.m_x, m_y + rhs.m_y, m_idx);
    }

    Vector2 operator-(const Vector2& rhs) const {
        return Vector2(m_x - rhs.m_x, m_y - rhs.m_y, m_idx);
    }

    Vector2 operator*(double scalar) const {
        return Vector2(m_x*scalar, m_y*scalar, m_idx);
    }

    Vector2 operator/(double scalar) const {
        double invScalar = 1.0/scalar;
        return Vector2(m_x*invScalar, m_y*invScalar, m_idx);
    }

    // Precedence for convex hull algorithms
    // TODO - Remove if this becomes part of a larger library, easy to confuse
    bool operator<(const Vector2& rhs) const {
        return (m_x < rhs.m_x) || (m_x == rhs.m_x && m_y < rhs.m_y);
    }

};

} // end namespace gaden