#pragma once

#include "gaden/Edge.hpp"
#include "gaden/Face.hpp"
#include "gaden/MinRect.hpp"
#include "gaden/VectorNField.hpp"

namespace gaden {

// A collection of algorithms that create 2d and 3d convex hulls
class ConvexHullTools {

public:

    // Calculates which vertices belong in the convex hull, fills m_convexHullIndices
    // Returns number of valid topological dimensions carved out by the hull, -1 for insufficient
    // points
    static int calculateConvexHull3d(
        // Inputs
        const Vector3Field& ptsIn,
        double toleranceIn,

        // Outputs
        Vector3Field& ptsOut,
        IntField& chVerticesOut,
        std::vector<Face> chFacesOut
    );

    // Create a 2D convex hull for the supplied ptsIn.  Outputs:
    //  * ptsOut - points incident to the output hull
    //  * verticesOut - indices of ptsOut in the original ptsIn
    // returns number of topological dimensions carved out by convex hull:
    //  -1  - no valid points
    //  0   - a single point
    //  1   - a line
    //  2   - a polygon in 2 dimensions
    static int calculateConvexHull2d(
        // Inputs
        const IndexedVector2Field& ptsIn,

        // Outputs
        IndexedVector2Field& ptsOut,
        IntField& verticesOut
    );

    static MinRect rotatingCalipers(
        const Vector3& u,
        const Vector3& v,
        const IndexedVector2Field& ptsIn
    );

};

} // end namespace gaden