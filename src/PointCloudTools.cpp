#include "gaden/PointCloudTools.hpp"

#include <iterator>

#include "gaden/AutoMergingPointCloud.hpp"

void gaden::PointCloudTools::mergePointsMap(
    const Vector3Field& ptsIn,
    double mergeTol,
    int estimatedSize,
    IntField& mapOut
) {
    Vector3Field unused;
    mergePointsSubset(
        ptsIn, mergeTol, estimatedSize, mapOut, unused
    );
}


void gaden::PointCloudTools::mergePointsSubset(
    const Vector3Field& ptsIn,
    double mergeTol,
    int estimatedSize,
    IntField& mapOut,
    Vector3Field& ptsOut
) {
    mapOut.clear();
    mapOut.reserve(ptsIn.size());
    std::string mergedName = ptsIn.name() + "_merged";
    AutoMergingPointCloud amp(estimatedSize, mergeTol, mergedName);
    for (const Vector3& pt : ptsIn) {
        mapOut.push_back(amp.append(pt).second);
    }
    ptsOut = std::move(amp).transfer();
}


void gaden::PointCloudTools::projectPointsToPlane(
    const Vector3Field& ptsIn,
    double thetaIn,
    double phiIn,
    double toleranceIn,
    IndexedVector2Field& ptsOut
) {
    // Build 3D orthonormal frame (u,v,w) from (thetaIn,phiIn)
    //  * thetaIn = heading (yaw) about +Z;
    //  * phiIn = declination from XY plane (positive looks "up" toward +Z)
    // Unit look-direction:
    const double cth = std::cos(thetaIn);
    const double sth = std::sin(thetaIn);
    const double cph = std::cos(phiIn);
    const double sph = std::sin(phiIn);

    // w points where we "look": in spherical-like terms with elevation=phiIn from XY
    Vector3 w(cth*cph, sth*cph, sph);
    w.normalise();

    // Build u perpendicular to w (helper axis t arbitrary, not parallel to w)
    Vector3 t = (std::fabs(w.z()) < 0.9) ? Vector3(0.0, 0.0, 1.0) : Vector3(1.0, 0.0, 0.0);
    // subtract component along w
    Vector3 u = t - w*w.dotProduct(t);
    u.normalise();

    // v completes right-handed frame
    // v perpendicular to u,w
    Vector3 v = w.crossProduct(u);
    v.normalise();

    // Project all points into (u,v) coordinates
    const int nPts = static_cast<int>(ptsIn.size());
    ptsOut.reserve(nPts);
    for (int i = 0; i < nPts; ++i) {
        const Vector3& p = ptsIn[i];
        const double x = p.dotProduct(u);
        const double y = p.dotProduct(v);
        ptsOut.emplace_back(x, y, i);
    }

    // TODO - pull this out as a 2d point merge algorithm
    // 2D point merging - lexicographic(x,y) merge method to remove duplicates (if any)
    std::sort(ptsOut.begin(), ptsOut.end());
    IndexedVector2Field::const_iterator readIter = ptsOut.cbegin();
    IndexedVector2Field::iterator writeIter = ptsOut.begin();
    if (writeIter == ptsOut.end() || ++writeIter == ptsOut.end()) {
        // There are zero or one points
        return;
    }
    // Confirmed at least two points exist
    readIter += 2;
    // writeIter starts at index 1 (second element)
    // readIter starts at index 2 (third element)
    double tolSqr = toleranceIn*toleranceIn;
    while (readIter != ptsOut.cend()) {
        const IndexedVector2& rd(*readIter);
        IndexedVector2& wr(*writeIter);
        IndexedVector2 delta = rd-wr;
        if (delta.magSqr() > tolSqr) {
            // Confirmed two points are too far apart to overlap
            wr = rd;
            ++writeIter;
        }
        ++readIter;
    }
}
