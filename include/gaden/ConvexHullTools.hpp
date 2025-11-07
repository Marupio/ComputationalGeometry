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

    // Converts the nDimensions returned by convex hull calculation functions into a user-facing
    // word
    static std::string nDimsToWord(int nDims) {
        switch (nDims) {
            case -1:
                return "Nothing";
            case 0:
                return "Point";
            case 1:
                return "Line";
            case 2:
                return "Area";
            case 3:
                return "Volume";
            default:
                return "Error";
        }
    }

    // Given a 2d point cloud, identify the edge and associated angle of rotation that gives the
    // minimum enclosing rectangle. Rotating calipers navigates the outer edges of the point cloud
    // and calculates the size of the bound box aligned to that edge. Returns MinRect, a POD class
    // designed with this function in mind, containing:
    //  * valid (bool)     - false if point cloud does not define a 2d area
    //  * area (double)    - area of the associated bound box
    //  * psi (double)     - rotation angle (angle of associated edge with horizontal)
    //  * width (double)   - 'horizontal' aligned dimension of the minimum rectangle
    //  * height (double)  - 'vertical' aligned dimension of the minimum rectangle
    //  * parentEdge (int) - associated outer edge that resulted in the minimum rectangle
    static MinRect rotatingCalipers(const IndexedVector2Field& ptsIn);

};

} // end namespace gaden