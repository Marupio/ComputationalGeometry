#pragma once

#include <cmath>
#include <iostream>
#include <sstream>

namespace gaden {

class Vector2 {
    // X coord
    double m_x;

    // Y coord
    double m_y;

    // Optional index into point list
    // TODO remove this, put into 'IndexedVector2' class
    int m_idx;

public:

    // Null constructor
    Vector2() : m_x(0.0), m_y(0.0), m_idx(-1) {}

    // Construct from components
    Vector2(double x, double y, int i=-1) : m_x(x), m_y(y), m_idx(i) {}

    Vector2(std::istream& is) {
        fromCsv(is);
    }

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

    // Binary operations

    Vector2& operator*=(double val) {
        m_x *= val;
        m_y *= val;
        return *this;
    }

    Vector2& operator/=(double val) {
        m_x /= val;
        m_y /= val;
        return *this;
    }

    // * Vectors

    Vector2& operator+=(const Vector2& vec) {
        m_x += vec.m_x;
        m_y += vec.m_y;
        return *this;
    }

    Vector2& operator-=(const Vector2& vec) {
        m_x -= vec.m_x;
        m_y -= vec.m_y;
        return *this;
    }

    Vector2 operator+(const Vector2& vec) const {
        return Vector2(*this) += vec;
    }

    Vector2 operator-(const Vector2& vec) const {
        return Vector2(*this) -= vec;
    }

    Vector2 operator*(double scalar) const {
        return Vector2(m_x*scalar, m_y*scalar, m_idx);
    }

    Vector2 operator/(double scalar) const {
        return Vector2(m_x/scalar, m_y/scalar, m_idx);
    }

    // Precedence for convex hull algorithms
    // TODO - Remove if this becomes part of a larger library, easy to confuse
    bool operator<(const Vector2& rhs) const {
        return (m_x < rhs.m_x) || (m_x == rhs.m_x && m_y < rhs.m_y);
    }

    // *** I/O

    // Csv format has no parentheses
    std::string toCsv() const {
        std::ostringstream oss;
        oss << m_x << "," << m_y;
        return oss.str();
    }

    // Csv format has no parentheses
    void fromCsv(std::istream& is) {
        char comma;
        is >> m_x >> comma >> m_y;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2& c) {
        return os << "(" << c.m_x << "," << c.m_y << ")";
    }

    friend std::istream& operator>>(std::istream& is, Vector2& c) {
        // char comma;
        char lparen, comma, rparen;
        return (is >> lparen >> c.m_x >> comma >> c.m_y >> rparen);
    }
};

} // end namespace gaden