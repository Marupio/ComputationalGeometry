#pragma once

#include "gaden/Vector3Field.hpp"
#include "gaden/Vector2Field.hpp"

namespace gaden {

// A collection of algorithms that operate on a point cloud (Vector3Field)
class PointCloudTools {

public:

    // Identify mergable points and return the map from old index to new index
    //  Inputs
    //      * ptsIn - the point cloud
    //      * mergeTol - the maximum distance between mergable points
    //      * estimatedSize - estimate of the size of unique points after merging
    //  Outputs
    //      * mapOut - map from old to new: mapOut[ptsInIndex] = subsetIndex
    static void mergePointsMap(
        const Vector3Field& ptsIn,
        double mergeTol,
        int estimatedSize,
        IntField& mapOut
    );

    // Identify mergable points and return the merged subset as well as the map from old to new
    //  Inputs
    //      * ptsIn - the point cloud
    //      * mergeTol - the maximum distance between mergable points
    //      * estimatedSize - estimate of the size of unique points after merging
    //  Outputs
    //      * mapOut - map from old to new: mapOut[ptsInIndex] = subsetIndex
    //      * ptsOut - copy of the ptsIn, with the duplicate (merged) points removed
    static void mergePointsSubset(
        const Vector3Field& ptsIn,
        double mergeTol,
        int estimatedSize,
        IntField& mapOut,
        Vector3Field& ptsOut
    );

    // Project to plane with rotation:
    //  thetaIn (heading / yaw, rotation about z axis)
    //  phiIn   (declination / pitch)
    // toleranceIn is used in determining if two projected points overlapp and can be merged
    static void projectPointsToPlane(
        const Vector3Field& ptsIn,
        double thetaIn,
        double phiIn,
        double toleranceIn,
        Vector2Field& ptsOut
    );
};

} // end namespace gaden