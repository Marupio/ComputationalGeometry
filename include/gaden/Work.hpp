#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "gaden/Axes.hpp"
#include "gaden/BoundBox.hpp"
#include "gaden/MinRect.hpp"
#include "gaden/Vector3.hpp"

namespace gaden {


class Work {
    std::string m_fileName;
    std::vector<int> m_faceNumber;
    std::vector<Vector3> m_points;
    std::vector<Vector3> m_normals;

    // Reduce CPU effort (1) merge points
    std::vector<Vector3> m_mergedPoints;

    // m_mergedPointsMap[originalIndex] = newIndex
    std::vector<int> m_mergedPointsMap;

    // Reduce CPU effort (2) convex hull

    // List of principal points from the point list that was supplied to the convex hull algorithm
    std::vector<int> m_convexHullIndices;

    // List of principal points on the convex hull
    std::vector<Vector3> m_convexHull3dPoints;

    // Bound box calculations (1) axis-aligned
    BoundBox m_axisAlignedBoundBox;

    // Bound box calculations (2) transient data - 2d convex hull after theta/phi rotation
    std::vector<int> m_convexHull2dIndices;

    // Bound box calculations (3) transient data - rectangle on projected hull with optimal psi
    MinRect m_optimalRect;



public:
    Work(std::string fileName);

    // Accessors
    const std::string& fileName() const { return m_fileName; }
    const std::vector<int>& faceNumber() const { return m_faceNumber; }
    const std::vector<Vector3>& points() const { return m_points; }
    const std::vector<Vector3>& normals() const { return m_normals; }
    const std::vector<Vector3>& mergedPoints() const { return m_mergedPoints; }
    const std::vector<int>& mergedPointsMap() const { return m_mergedPointsMap; }
    const BoundBox& axisAlignedBoundBox() const { return m_axisAlignedBoundBox; }
    const std::vector<int>& convexHullIndices() const { return m_convexHullIndices; }
    const std::vector<Vector3>& convexHull3dPoints() const { return m_convexHull3dPoints; }


    // Functionality

    // Reads in data from fileName, fills:
    //  * m_faceNumber, m_points, m_normals
    bool readData();

    // Helper for above
    bool isNumber(char c) const;

    int mergePoints(double eps = 1e-9);

    // Simple min/max on all m_points, constructs m_axisAlignedBoundBox, returns its 'valid()' flag
    bool calculateAxisAlignedBoundBox(const std::vector<Vector3>& pts);

    // Calculates which vertices belong in the convex hull, fills m_convexHullIndices
    // Returns number of valid topological dimensions carved out by the hull, -1 for insufficient
    // points
    int calculateConvexHull3dVertexIndices(const std::vector<Vector3>& pts, double eps = 1e-9);

    // Calculates 2d convex hull from projection of 3d points, given theta (heading) & phi
    //  (declination), returns false if too few points or degenerate
    bool calculateConvexHull2dVertexIndices(
        const std::vector<Vector3>& pts, double theta, double phi
    );

    // For a given 2d convex hull, solves optimum psi angle for given framing vectors, u and v.
    //  Result gets written to m_optimalRect.  Returns false if too few elements or degenerate.
    bool solvePsiOnProjectedHull(
        const Vector3& u,
        const Vector3& v,
        const std::vector<Vector3>& pts
    );

    // Perform iterative calculations over 'steps' on theta and phi, giving steps^2 as total
    //  iterations for each pass, and 'passes' increasing the accuracy, narrowing to the solution
    //  each time.
    //
    //  Returns:
    //      * output parameter 'resultBb' - minimum BoundBox
    //      * output parameter 'resultAxes' - rotated axes, aligned to minimum BoundBox
    //      * output parameter 'resultRotations' - a Vector3 containing:
    //          resultRotations = (theta, phi, psi)
    //      * return value false on error
    bool calculateRotatedBoundBox(
        // outputs
        BoundBox& resultBb, Axes& resultAxes, Vector3& resultRotations,

        // inputs
        const std::vector<Vector3>& pts, int steps=7, int passes=1
    );


    // Debug output
    void report() const;

};

} // end namespace gaden