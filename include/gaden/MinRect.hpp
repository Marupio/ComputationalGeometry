#pragma once

#include <limits>
#include <vector>

#include "gaden/Vector2.hpp"

namespace gaden {

struct MinRect {
    // Plain old data container, metadata for rotating calipers algorithm (determines final angle of
    //  rotation, psi)

    // rectangle area
    double m_area;

    // 3rd angle of rotation in 3d space:
    //  * theta (heading / yaw)
    //  * phi (declination / pitch)
    //  * psi (roll) <-- this one
    double m_psi;

    // horizontal (total width along this axis, defined by edge on hull that makes psi angle with
    //  makes angle with u axis)
    double m_width;

    // vertical
    double m_height;

    // hull edge index that defined psi
    int m_parentEdge;

public:

    // Construct null
    MinRect() { clear(); }

    // Accessors

    double area() const { return m_area; }
    double& area() { return m_area; }
    double psi() const { return m_psi; }
    double& psi() { return m_psi; }
    double width() const { return m_width; }
    double& width() { return m_width; }
    double height() const { return m_height; }
    double& height() { return m_height; }
    int parentEdge() const { return m_parentEdge; }
    int& parentEdge() { return m_parentEdge; }

    // Functionality

    void clear() {
        m_area = std::numeric_limits<double>::infinity();
        m_psi = 0.0;
        m_width = 0.0;
        m_height = 0.0;
        m_parentEdge = -1;
    }

    // Calculate axial edges from the i'th edge in a planar polygon, return as output parameters
    static void calculateEdgeFrame(
        // inputs
        int i, int hullSize, const std::vector<Vector2>& polygon,

        // outputs
        Vector2& ue, Vector2& ve
    ) {
        const Vector2 e = polygon[(i+1)%hullSize] - polygon[i];
        double lengthSqr = e.magSqr();
        if (lengthSqr <= 0.0) {
            ue = Vector2(1.0, 0.0);
            ve = Vector2(0.0, 1.0);
            return;
        }
        const double length = std::sqrt(lengthSqr);
        ue = Vector2(e.x() / length, e.y() / length);
        ve = Vector2(-ue.y(), ue.x()); // +90 degrees
    }

};

} // end namespace gaden
