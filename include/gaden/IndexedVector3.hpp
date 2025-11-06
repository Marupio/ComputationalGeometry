#pragma once

#include "Vector3.hpp"

namespace gaden {

class IndexedVector3 {
    Vector3 m_v;
    int m_idx;

public:

    // Construct from components
    IndexedVector3(double x_in, double y_in, double z_in, int idx_in = -1):
        m_v(x_in, y_in, z_in),
        m_idx(idx_in)
    {}

    // Null constructor
    IndexedVector3():
        m_v(),
        m_idx(-1)
    {}

    // Construct from Vector3 and index
    IndexedVector3(const Vector3& v_in, int idx_in = -1):
        m_v(v_in),
        m_idx(idx_in)
    {}

    // Accessors

    Vector3& v() { return m_v; }
    const Vector3& v() const { return m_v; }

    double x() const { return m_v.x(); }
    double& x() { return m_v.x(); }
    double y() const { return m_v.y(); }
    double& y() { return m_v.y(); }
    double z() const { return m_v.z(); }
    double& z() { return m_v.z(); }

    int idx() const { return m_idx; }
    int& idx() { return m_idx; }

    // Unary operations

    // Magnitude, squared x^2,y^2,z^2
    double magSqr() const {
        return m_v.magSqr();
    }

    // Magnitude (i.e. length)
    double mag() const {
        return m_v.mag();
    }

    // Turn into a unit vector, returns false if magnitude is near-zero
    bool normalise() {
        return m_v.normalise();
    }

    // *** Operations

    // * Scalars

    IndexedVector3& operator*=(double val) {
        m_v *= val;
        return *this;
    }

    IndexedVector3& operator/=(double val) {
        m_v /= val;
        return *this;
    }

    // * Vectors

    IndexedVector3& operator+=(const IndexedVector3& vec) {
        m_v += vec.m_v;        // index policy: keep lhs index
        return *this;
    }

    IndexedVector3& operator-=(const IndexedVector3& vec) {
        m_v -= vec.m_v;        // index policy: keep lhs index
        return *this;
    }

    IndexedVector3 operator+(const IndexedVector3& vec) const {
        return IndexedVector3(*this) += vec;
    }

    IndexedVector3 operator-(const IndexedVector3& vec) const {
        return IndexedVector3(*this) -= vec;
    }

    IndexedVector3 operator*(double scalar) const {
        IndexedVector3 tmp(*this);
        tmp *= scalar;
        return tmp;
    }

    IndexedVector3 operator/(double scalar) const {
        IndexedVector3 tmp(*this);
        tmp /= scalar;
        return tmp;
    }

    // Returns result=(*this) crossProduct rhs, carrying lhs index
    IndexedVector3 crossProduct(const IndexedVector3& rhs) const {
        return IndexedVector3(m_v.crossProduct(rhs.m_v), m_idx);
    }

    // Returns result=(*this) dotProduct rhs
    double dotProduct(const IndexedVector3& rhs) const {
        return m_v.dotProduct(rhs.m_v);
    }
};

// Symmetric scalar multiply (scalar * vec)
inline IndexedVector3 operator*(double scalar, const IndexedVector3& v) {
    return v * scalar;
}

} // end namespace gaden
