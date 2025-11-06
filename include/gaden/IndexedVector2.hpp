#pragma once

#include "Vector2.hpp"

namespace gaden {

class IndexedVector2 {
    // 2D vector value
    Vector2 m_v;

    // Optional index tag
    int m_idx;

public:

    // Null constructor
    IndexedVector2() : m_v(), m_idx(-1) {}

    // Construct from components
    IndexedVector2(double x, double y, int idx = -1) : m_v(x, y), m_idx(idx) {}

    // Construct from Vector2 and index
    IndexedVector2(const Vector2& v, int idx = -1) : m_v(v), m_idx(idx) {}

    // Accessors
    Vector2& v() { return m_v; }
    const Vector2& v() const { return m_v; }

    double x() const { return m_v.x(); }
    double& x() { return m_v.x(); }
    double y() const { return m_v.y(); }
    double& y() { return m_v.y(); }

    int idx() const { return m_idx; }
    int& idx() { return m_idx; }

    // Cross product (2D vector math)
    double crossProduct(const IndexedVector2& rhs) const { return m_v.crossProduct(rhs.m_v); }

    // Dot product (inner product)
    double dotProduct(const IndexedVector2& rhs) const { return m_v.dotProduct(rhs.m_v); }

    // Magnitude, squared
    double magSqr() const { return m_v.magSqr(); }

    // Magnitude (length)
    double mag() const { return m_v.mag(); }

    // Turn into a unit vector, returns false if magnitude is near-zero
    bool normalise() { return m_v.normalise(); }

    // Binary operations

    IndexedVector2& operator*=(double val) {
        m_v *= val;
        return *this;
    }

    IndexedVector2& operator/=(double val) {
        m_v /= val;
        return *this;
    }

    // * Vectors

    IndexedVector2& operator+=(const IndexedVector2& vec) {
        // index policy: keep lhs index
        m_v += vec.m_v;
        return *this;
    }

    IndexedVector2& operator-=(const IndexedVector2& vec) {
        // index policy: keep lhs index
        m_v -= vec.m_v;
        return *this;
    }

    IndexedVector2 operator+(const IndexedVector2& vec) const {
        return IndexedVector2(*this) += vec;
    }

    IndexedVector2 operator-(const IndexedVector2& vec) const {
        return IndexedVector2(*this) -= vec;
    }

    IndexedVector2 operator*(double scalar) const {
        IndexedVector2 tmp(*this);
        tmp *= scalar;
        return tmp;
    }

    IndexedVector2 operator/(double scalar) const {
        IndexedVector2 tmp(*this);
        tmp /= scalar;
        return tmp;
    }

    // Precedence (optional): delegate to value; tie-break on idx if desired
    bool operator<(const IndexedVector2& rhs) const {
        if (m_v < rhs.m_v) { return true; }
        if (rhs.m_v < m_v) { return false; }
        return m_idx < rhs.m_idx;
    }
};

// Symmetric scalar multiply (scalar * vec)
inline IndexedVector2 operator*(double scalar, const IndexedVector2& v) {
    return v * scalar;
}

} // end namespace gaden
