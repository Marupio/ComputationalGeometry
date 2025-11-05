#pragma once

#include <vector>

#include "gaden/Constants.hpp"
#include "gaden/Vector3.hpp"
#include "gaden/Vector3Field.hpp"

namespace gaden {

class Face {
    // Point indices, CCW from exterior
    int m_ptA;
    int m_ptB;
    int m_ptC;

    // Normal
    Vector3 m_normal;

    // Planar offset, normal*x + offset = 0
    double m_offset;

    // Non encapsulated metadata for Convex Hull

    // Point indices strictly outside (n·p + d > eps)
    std::vector<int> m_outside;

    // Status
    bool m_alive;

public:
    Face() {};

    // Construct from components and reference to point list
    Face(int a, int b, int c, const Vector3Field& pts, double eps):
        m_ptA(a),
        m_ptB(b),
        m_ptC(c),
        m_alive(true)
    {
        const Vector3& pA = pts[a];
        const Vector3& pB = pts[b];
        const Vector3& pC = pts[c];

        // n = (B - A) x (C - A)
        m_normal = (pB - pA).crossProduct(pC - pA);
        double nMag = m_normal.mag();
        if (nMag > constants::doubleSmall) {
            // normalise and compute plane offset so: n·x + d = 0
            m_normal.normalise();
            m_offset = -m_normal.dotProduct(pA);
        } else if (nMag <= eps) {
            m_alive = false;
            m_offset = 0.0;
        } else {
            m_offset = 0.0;
        }
    }

    double signedDistance(const Vector3& p) const {
        return m_normal.dotProduct(p) + m_offset;
    }

    bool visibleFrom(const Vector3& p, double eps) const {
        return signedDistance(p) > eps;
    }

    // Accessors
    int a() const { return m_ptA; }
    int b() const { return m_ptB; }
    int c() const { return m_ptC; }

    const Vector3& normal() const { return m_normal; }

    double offset() const { return m_offset; }

    const std::vector<int>& outside() const { return m_outside; }
    std::vector<int>& outside() { return m_outside; }

    bool alive() const { return m_alive; }
    bool& alive() { return m_alive; }

};

} // end namespace gaden