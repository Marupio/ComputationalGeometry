#pragma once

#include "gaden/Axes.hpp"
#include "gaden/Constants.hpp"
#include "gaden/Vector3.hpp"
#include "gaden/VectorNField.hpp"

namespace gaden {

class BoundBox {
    Vector3 m_min;
    Vector3 m_max;
public:
    BoundBox():
        m_min(constants::doubleGreat, constants::doubleGreat, constants::doubleGreat),
        m_max(constants::doubleNegGreat, constants::doubleNegGreat, constants::doubleNegGreat)
    {}
    BoundBox(Vector3 minIn, Vector3 maxIn):
        m_min(minIn),
        m_max(maxIn)
    {
        rationalise();
    }

    Vector3 minPt() const { return m_min; }
    Vector3& minPt() { return m_min; }
    Vector3 maxPt() const { return m_max; }
    Vector3& maxPt() { return m_max; }

    bool valid() const {
        return (
            m_min.x() <= m_max.x() &&
            m_min.y() <= m_max.y() &&
            m_min.z() <= m_max.z()
        );
    }

    // Empty has min > max
    bool empty() const { return !valid(); }

    // Confirmed empty is min and max are exactly equal to the default empty values
    bool confirmEmpty() const {
        return (
            m_min.x() == constants::doubleGreat &&
            m_min.y() == constants::doubleGreat &&
            m_min.z() == constants::doubleGreat &&
            m_max.x() == constants::doubleNegGreat &&
            m_max.y() == constants::doubleNegGreat &&
            m_max.z() == constants::doubleNegGreat
        );
    }

    bool rationalise() {
        if (confirmEmpty()) {
            // Avoid attempting to rationalise an empty boundbox
            return false;
        }
        Vector3 minIn = m_min;
        Vector3 maxIn = m_max;
        m_min = Vector3(
            std::min(minIn.x(), m_min.x()),
            std::min(minIn.y(), m_min.y()),
            std::min(minIn.z(), m_min.z())
        );
        m_max = Vector3(
            std::max(maxIn.x(), m_max.x()),
            std::max(maxIn.y(), m_max.y()),
            std::max(maxIn.z(), m_max.z())
        );
        return true;
    }

    // Query
    bool contains(const Vector3& pt) {
        if (empty()) {
            return false;
        }

        // TODO add tolerance - grow boundbox by tolerance before doing this
        return (
            pt.x() <= m_max.x() &&
            pt.y() <= m_max.y() &&
            pt.z() <= m_max.z() &&

            pt.x() >= m_min.x() &&
            pt.y() >= m_min.y() &&
            pt.z() >= m_min.z()
        );
    }

    // Append a given point to the BoundBox - if inside, do nothing, otherwise grow to include it
    // Returns true if this has changed the BoundBox
    bool append(const Vector3& pt) {
        if (empty()) {
            m_min = pt;
            m_max = pt;
            return true;
        }
        if (contains(pt)) {
            return false;
        }
        m_min = Vector3(
            std::min(m_min.x(), pt.x()),
            std::min(m_min.y(), pt.y()),
            std::min(m_min.z(), pt.z())
        );
        m_max = Vector3(
            std::max(m_max.x(), pt.x()),
            std::max(m_max.y(), pt.y()),
            std::max(m_max.z(), pt.z())
        );
        return true;
    }

    // bool solvePsiOnProjectedHull(
    //     const Vector3& u,
    //     const Vector3& v,
    //     const std::vector<Vector3>& pts
    // );

    // Tools

    // Calculate axis-aligned boundBox from given points
    static BoundBox calculateAxisAlignedBoundBox(const Vector3Field& ptsIn);

    // Return the minimum BoundBox resulting from an iterative search through steps x steps
    // variations on rotations, passes times
    static BoundBox solveMinimumRotatedBoundBox(
        // outputs
        Axes& resultAxes, Vector3& resultRotations,

        // inputs
        const Vector3Field& pts, int steps, int passes, double epsilon
    );

    friend std::ostream& operator<<(std::ostream& os, const BoundBox& c) {
        return os << "[" << c.minPt() << "->" << c.maxPt() << "]";
    }

};

} // end namespace gaden
