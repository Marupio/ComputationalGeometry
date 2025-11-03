#include <cmath>
#include <fstream>
#include <limits>
#include <numeric>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
// #include <stdlib.h>

#include "gaden/Constants.hpp"
#include "gaden/Edge.hpp"
#include "gaden/Face.hpp"
#include "gaden/Logger.hpp"
#include "gaden/AutoMergingPointsArray.hpp"
#include "gaden/Vector2.hpp"
#include "gaden/Work.hpp"

gaden::Work::Work(std::string fileName) :
    m_fileName(fileName)
{}


bool gaden::Work::readData() {
    std::string buffer;
    std::ifstream iss(m_fileName);
    if (!iss.is_open()) {
        return false;
    }
    while (std::getline(iss, buffer)) {
        Log_Debug4("Line=[" << buffer << "]");
        std::istringstream lineIss(buffer);
        char c = lineIss.peek();
        if (!isNumber(c)) {
            Log_Debug("First character is not a number, c=" << int(c));
            continue;
        }

        int faceNumber;
        lineIss >> faceNumber;
        m_faceNumber.push_back(faceNumber);

        char comma;
        lineIss >> comma;

        Vector3 position;
        lineIss >> position;
        m_points.push_back(position);

        lineIss >> comma;

        Vector3 normal;
        lineIss >> normal;
        m_normals.push_back(normal);
    }

    // #ifdef GADEN_DEBUG
    //     report();
    // #endif
    return true;
}


bool gaden::Work::isNumber(char c) const {
    return c <= '9' && c >= '0';

}


int gaden::Work::mergePoints(double epsilon) {
    int nPoints = m_points.size();
    int reserve = nPoints * 7 / 8;
    m_mergedPoints.clear();
    m_mergedPointsMap.clear();
    m_mergedPointsMap.reserve(reserve);

    AutoMergingPointsArray smp(reserve, epsilon);
    for (int i = 0; i < nPoints; ++i) {
        const Vector3& pt(m_points[i]);
        m_mergedPointsMap.push_back(smp.append(pt));
    }
    m_mergedPoints = smp.transfer();
    return m_points.size() - m_mergedPoints.size();
}


bool gaden::Work::calculateAxisAlignedBoundBox(const std::vector<Vector3>& pts) {
    BoundBox& bb = m_axisAlignedBoundBox;
    for (Vector3 pt : pts) {
        bb.append(pt);
    }
    return bb.valid();
}


int gaden::Work::calculateConvexHull3dVertexIndices(const std::vector<Vector3>& pts, double eps)
{
    // Allias for readability
    const std::vector<Vector3>& P = pts;
    const int n = P.size();

    std::vector<int>& out = m_convexHullIndices;
    out.clear();
    if (n == 0) {
        // No points => no hull
        return -1;
    }
    if (n <= 3) {
        // 1–3 points: hull is the set itself (degenerate)
        out.resize(n);
        std::iota(out.begin(), out.end(), 0);
        return -1;
    }

    // *** Build initial simplex (tet), choose (p0, p1, p2, p3)
    int p0 = 0;
    // Find leftmost vertex, linear search, ignore ties
    for (int i = 1; i < n; ++i) {
        if (P[i].x() < P[p0].x()) {
            p0 = i;
        }
    }

    // p1: farthest from p0 (by squared distance)
    int p1 = p0;
    double best = -1.0;
    for (int i = 0; i < n; ++i) {
        // ||P[i] - P[p0]||^2
        const Vector3 d = P[i] - P[p0];
        const double d2 = d.magSqr();

        if (d2 > best) {
            best = d2;
            p1 = i;
        }
    }
    if (p1 == p0) {
        // All points coincide with p0 (completely degenerate)
        out.push_back(p0);
        return 0;
    }

    // p2: maximises triangle area with (p0,p1) via |(p1-p0) x (P[i]-p0)|^2
    int p2 = p0; best = -1.0;
    const Vector3 u = P[p1] - P[p0];
    for (int i = 0; i < n; ++i) {
        if (i != p0 && i != p1) {
            // ||u x w||^2 (proportional to area^2)
            const Vector3 w = P[i] - P[p0];
            const Vector3 cx = u.crossProduct(w);
            const double a2 = cx.dotProduct(cx);

            if (a2 > best) {
                best = a2; p2 = i;
            }
        }
    }
    if (p2 == p0 || best <= eps) {
        // Points nearly colinear: return extreme endpoints along u (line hull)
        int lo = 0, hi = 0;
        double loP = (P[0] - P[p0]).dotProduct(u), hiP = loP;
        for (int i = 1; i < n; ++i) {
            // scalar projection on u
            const double pr = (P[i] - P[p0]).dotProduct(u);
            if (pr < loP) {
                loP = pr;
                lo = i;
            }
            if (pr > hiP) {
                hiP = pr;
                hi = i;
            }
        }
        if (lo == hi) {
            // not even a line, this is a single unique point
            out = { lo };
            return 0;
        }
        // the 'hull' is a line - two endpoints define the line hull
        out = { lo, hi };
        std::sort(out.begin(), out.end());
        return 1;
    }

    // Seed face from (p0,p1,p2); computes plane normal/offset, orientation, etc.
    Face seed(static_cast<int>(p0), static_cast<int>(p1), static_cast<int>(p2), P, eps);

    // p3: farthest (by |signed distance|) from the seed plane => maximises tet volume
    int p3 = p0; best = -1.0;
    for (int i = 0; i < n; ++i) if (i != p0 && i != p1 && i != p2) {
        // |n·p + d|
        const double sd = std::fabs(seed.signedDistance(P[i]));
        if (sd > best) {
            best = sd;
            p3 = i;
        }
    }
    if (p3 == p0 || best <= eps) {
        // Points are coplanar(-ish). Build a safe superset of potential hull vertices by
        // taking extrema in a few independent directions (normal, two tangents, and an edge).
        out = { p0, p1, p2 };

        // plane normal
        const Vector3 nrm = seed.normal();

        // tangent direction 1
        const Vector3 n01 = u.crossProduct(nrm);

        // tangent direction 2
        const Vector3 n02 = (P[p2] - P[p0]).crossProduct(nrm);

        const Vector3 dirs[4] = { nrm, n01, n02, (P[p2] - P[p1]) };
        for (const Vector3& d : dirs) {
            if (d.magSqr() <= eps*eps) {
                // skip near-zero directions
                continue;
            }
            // Find extremes along d across all points; add both indices
            int lo = 0, hi = 0;
            double loP = P[0].dotProduct(d), hiP = loP;
            for (int i = 1; i < n; ++i) {
                const double pr = P[i].dotProduct(d);
                if (pr < loP) {
                    loP = pr;
                    lo = i;
                }
                if (pr > hiP) {
                    hiP = pr;
                    hi = i;
                }
            }
            out.push_back(lo);
            out.push_back(hi);
        }
        // Unique-sort to remove duplicates; caller may further process this coplanar set
        std::sort(out.begin(), out.end());
        out.erase(std::unique(out.begin(), out.end()), out.end());
        return 2;
    }

    // Ensure seed faces point outward (p3 is inside side)
    if (seed.visibleFrom(P[p3], 0.0)) {
        // If p3 sees the seed as front-facing, flip winding to make it outward
        seed = Face(static_cast<int>(p0), static_cast<int>(p2), static_cast<int>(p1), P, eps);
    }

    // Initial tet faces, CCW as seen from outside
    std::vector<Face> faces;
    faces.reserve(32);
    faces.emplace_back(static_cast<int>(p0), static_cast<int>(p1), static_cast<int>(p2), P, eps);
    faces.emplace_back(static_cast<int>(p0), static_cast<int>(p2), static_cast<int>(p3), P, eps);
    faces.emplace_back(static_cast<int>(p2), static_cast<int>(p1), static_cast<int>(p3), P, eps);
    faces.emplace_back(static_cast<int>(p1), static_cast<int>(p0), static_cast<int>(p3), P, eps);

    // Assign outside sets
    for (int i = 0; i < n; ++i) {
        if (i == p0 || i == p1 || i == p2 || i == p3) {
            // skip tet vertices
            continue;
        }
        // tolerance to ignore near-boundary points (i.e. <= eps)
        double bestDist = eps;
        int bestFace = -1;
        int nFaces = static_cast<int>(faces.size());
        for (int f = 0; f < nFaces; ++f) if (faces[f].alive()) {
            // positive means in front/outside
            const double sd = faces[f].signedDistance(P[i]);
            if (sd > bestDist) {
                bestDist = sd;
                bestFace = f;
            }
        }
        if (bestFace >= 0) {
            faces[bestFace].outside().push_back(i);
        }
    }

    // *** Quickhull main loop - incremental: iteratively add points & retriangulate
    while (true) {
        // Pick a face that still has outside points (greedy: largest far distance)
        int fIdx = -1; double fMax = -1.0;
        int nFaces = static_cast<int>(faces.size());
        for (int f = 0; f < nFaces; ++f) {
            if (faces[f].alive() && !faces[f].outside().empty()) {
                double localMax = -1.0;
                for (int idx : faces[f].outside()) {
                    const double sd = faces[f].signedDistance(P[idx]);
                    if (sd > localMax) {
                        localMax = sd;
                    }
                }
                if (localMax > fMax) {
                    fMax = localMax;
                    fIdx = f;
                }
            }
        }
        if (fIdx < 0) {
            // No faces with outside points remain => hull complete
            break;
        }

        // perspectivePoint is farthest view (point) from that face
        Face& f = faces[fIdx];
        int perspectivePoint = f.outside().front();
        double farBest = -1.0;
        for (int idx : f.outside()) {
            const double sd = f.signedDistance(P[idx]);
            if (sd > farBest) {
                farBest = sd;
                perspectivePoint = idx;
            }
        }

        // 1) Mark all faces visible from perspectivePoint
        std::vector<int> visible;
        visible.reserve(16);
        for (int i = 0; i < faces.size(); ++i) {
            if (faces[i].alive()) {
                if (faces[i].visibleFrom(P[perspectivePoint], eps)) {
                    faces[i].alive() = false;
                    visible.push_back(i);
                }
            }
        }

        // 2) Build the horizon: directed edges that appear exactly once among visible faces
        //    (edges bordering the "hole" after removing visible faces)
        std::unordered_map<Edge, int, EdgeHash> count;
        for (int vi : visible) {
            const Face& vf = faces[vi];
            ++count[Edge(vf.a(), vf.b())];
            ++count[Edge(vf.b(), vf.c())];
            ++count[Edge(vf.c(), vf.a())];
        }

        std::vector<Edge> horizon;
        horizon.reserve(count.size());
        for (const auto& kv : count) {
            const Edge& e = kv.first;
            const int c_uv = kv.second;

            // reverse-directed edge
            const auto it = count.find(Edge(e.v(), e.u()));
            const int c_vu = (it == count.end()) ? 0 : it->second;
            if (c_uv == 1 && c_vu == 0) {
                // Edge used once in forward direction and never in reverse => boundary edge
                horizon.push_back(e);
            }
        }

        // 3) Stitch new faces from horizon to perspectivePoint (maintain outward orientation)
        std::vector<int> newFaces;
        newFaces.reserve(horizon.size());
        for (const Edge& e : horizon) {
            // triangle (u -> v -> perspectivePoint)
            Face nf(e.u(), e.v(), perspectivePoint, P, eps);
            if (!nf.alive()) {
                // skip degenerate tris
                continue;
            }
            faces.push_back(nf);
            int newIndex = static_cast<int>(faces.size()) - 1;
            newFaces.push_back(newIndex);
        }

        // 4) Reassign outside points that belonged to removed faces (exclude perspectivePoint)
        std::vector<int> pool;
        for (int vi : visible) {
            for (int idx : faces[vi].outside()) {
                if (idx != perspectivePoint) {
                    pool.push_back(idx);
                }
            }
            faces[vi].outside().clear();
        }
        // Deduplicate pool to avoid repeated tests
        std::sort(pool.begin(), pool.end());
        pool.erase(std::unique(pool.begin(), pool.end()), pool.end());

        // Re-bin pooled points to the newly created faces (only if strictly outside)
        for (int idx : pool) {
            double bestD = eps;
            int bf = -1;
            for (int nf : newFaces) if (faces[nf].alive()) {
                const double sd = faces[nf].signedDistance(P[idx]);
                if (sd > bestD) {
                    bestD = sd;
                    bf = nf;
                }
            }
            if (bf >= 0) {
                faces[bf].outside().push_back(idx);
            }
        }
    }

    // *** Collect unique vertex indices
    // Here we throw away the hull, keeping only the points.
    // TODO - make this function actually return a convexHull3d and caller can discard
    std::unordered_set<int> verts;
    for (const Face& f : faces) if (f.alive()) {
        verts.insert(f.a());
        verts.insert(f.b());
        verts.insert(f.c());
    }
    out.assign(verts.begin(), verts.end());
    std::sort(out.begin(), out.end());

    // Cache actual hull points (for downstream steps)
    int nPts = out.size();
    m_convexHull3dPoints.clear();
    m_convexHull3dPoints.reserve(nPts);
    for (int i : out) {
        m_convexHull3dPoints.push_back(pts[i]);
    }
    return 3;
}


bool gaden::Work::calculateConvexHull2dVertexIndices(
    const std::vector<Vector3>& pts, double theta, double phi
) {
    // 1) Build 3D orthonormal frame (u,v,w) from (theta,phi)
    //  * theta = heading (yaw) about +Z;
    //  * phi = declination from XY plane (positive looks "up" toward +Z)
    // Unit look-direction:
    const double cth = std::cos(theta);
    const double sth = std::sin(theta);
    const double cph = std::cos(phi);
    const double sph = std::sin(phi);

    // w points where we "look": in spherical-like terms with elevation=phi from XY
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

    // 2) Project all points into (u,v) coordinates
    const int nPts = static_cast<int>(pts.size());
    std::vector<Vector2> P2;
    P2.reserve(nPts);
    for (int i = 0; i < nPts; ++i) {
        const Vector3& p = pts[i];
        const double x = p.dotProduct(u);
        const double y = p.dotProduct(v);
        P2.emplace_back(x, y, i);
    }

    // Degenerate cases
    m_convexHull2dIndices.clear();
    if (nPts <= 1) {
        if (nPts == 1) {
            m_convexHull2dIndices.push_back(0);
        }
        return false;
    }

    // 3) Monotone chain convex hull in 2D (O(n log n))
    // lexicographic (x,y)
    std::sort(P2.begin(), P2.end());
    // Remove duplicates if any
    std::vector<Vector2> A;
    A.reserve(P2.size());
    int nP2 = static_cast<int>(P2.size());
    for (int i = 0; i < nP2; ++i) {
        if (i == 0 || P2[i].x() != P2[i-1].x() || P2[i].y() != P2[i-1].y()) {
            A.push_back(P2[i]);
        }
    }
    if (A.size() == 1) {
        m_convexHull2dIndices.push_back(A[0].idx());
        return false;
    }
    if (A.size() == 2) {
        m_convexHull2dIndices.push_back(A[0].idx());
        m_convexHull2dIndices.push_back(A[1].idx());
        return false;
    }

    // Lower hull
    std::vector<Vector2> H;
    int nA = static_cast<int>(A.size());
    H.reserve(nA*2);
    for (int i = 0; i < nA; ++i) {
        while (H.size() >= 2) {
            const Vector2 a = H[H.size()-2];
            const Vector2 b = H[H.size()-1];
            const Vector2 c = A[i];
            const Vector2 ab = b - a;
            const Vector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                // keep CCW turns; collinear -> pop to keep outermost
                H.pop_back();
            } else {
                break;
            }
        }
        H.push_back(A[i]);
    }

    // Upper hull
    const int lowerSize = static_cast<int>(H.size());
    for (int i = A.size()-2; i >= 0; --i) {
        while (H.size() > lowerSize) {
            const Vector2 a = H[H.size()-2];
            const Vector2 b = H[H.size()-1];
            const Vector2 c = A[i];
            const Vector2 ab = b - a;
            const Vector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                H.pop_back();
            } else {
                break;
            }
        }
        H.push_back(A[i]);
    }

    // Last point equals first; remove it
    if (!H.empty()) H.pop_back();

    // 4) Convert back to original indices (CCW polygon)
    m_convexHull2dIndices.reserve(H.size());
    int nH = static_cast<int>(H.size());
    for (int i = 0; i < nH; ++i) {
        m_convexHull2dIndices.push_back(H[i].idx());
    }
    return true;
}


bool gaden::Work::solvePsiOnProjectedHull(
    const Vector3& u,
    const Vector3& v,
    const std::vector<Vector3>& pts
) {
    MinRect& mr(m_optimalRect);
    mr.clear();
    const int m = static_cast<int>(m_convexHull2dIndices.size());
    if (m <= 0) {
        return false;
    }
    if (m == 1) {
        mr.area() = 0.0;
        mr.width() = 0.0;
        mr.height() = 0.0;
        mr.psi() = 0.0;
        mr.parentEdge() = 0;
        return false;
    }

    // 1) Build the 2D polygon H2 by projecting hull vertices to (u,v)
    std::vector<Vector2> H2;
    H2.reserve(m);
    for (int i = 0; i < m; ++i) {
        const Vector3& p = pts[m_convexHull2dIndices[i]];
        const double x = p.dotProduct(u);
        const double y = p.dotProduct(v);
        H2.emplace_back(x, y);
    }

    // 2) Handle the trivial 2-vertex case (a segment)
    if (m == 2) {
        const Vector2 e = H2[1] - H2[0];
        const double lengthSqr = e.magSqr();
        if (lengthSqr > 0.0) {
            const double length = std::sqrt(lengthSqr);
            mr.width() = length;
            mr.height() = 0.0;
            mr.area() = 0.0;
            mr.psi() = std::atan2(e.y(), e.x());
            mr.parentEdge() = 0;
        } else {
            mr.area() = 0.0;
            mr.width() = mr.height() = 0.0;
            mr.psi() = 0.0;
            mr.parentEdge() = 0;
        }
        return false;
    }

    // 3) Initial edge (i=0): find extreme indices by a single scan
    int iUmax = 0, iUmin = 0, iVmax = 0, iVmin = 0;
    {
        Vector2 ue0;
        Vector2 ve0;
        MinRect::calculateEdgeFrame(0, m, H2, ue0, ve0);

        double minU = H2[0].dotProduct(ue0);
        double maxU = minU;
        double minV = H2[0].dotProduct(ve0);
        double maxV = minV;

        for (int k = 1; k < m; ++k) {
            const double su = H2[k].dotProduct(ue0);
            const double sv = H2[k].dotProduct(ve0);
            if (su < minU) {
                minU = su; iUmin = k;
            }
            if (su > maxU) {
                maxU = su; iUmax = k;
            }
            if (sv < minV) {
                minV = sv; iVmin = k;
            }
            if (sv > maxV) {
                maxV = sv; iVmax = k;
            }
        }

        mr.width() = (maxU - minU);
        mr.height() = (maxV - minV);
        mr.area() = mr.width() * mr.height();
        mr.psi() = std::atan2(ue0.y(), ue0.x());
        mr.parentEdge() = 0;
    }

    // 4) Sweep all edges; advance support points while their projection improves.
    for (int i = 1; i < m; ++i) {
        Vector2 ue;
        Vector2 ve;
        MinRect::calculateEdgeFrame(i, m, H2, ue, ve);

        // Advance each support index as long as the next vertex increases the projection.

        // Umax (maximize dot with ue)
        for (;;) {
            const int nxt = (iUmax + 1) % m;
            const double cur = H2[iUmax].dotProduct(ue);
            const double nxtv= H2[nxt].dotProduct(ue);
            if (nxtv > cur) {
                iUmax = nxt;
            } else {
                break;
            }
        }
        // Umin (minimize dot with ue)
        for (;;) {
            const int nxt = (iUmin + 1) % m;
            const double cur = H2[iUmin].dotProduct(ue);
            const double nxtv= H2[nxt].dotProduct(ue);
            if (nxtv < cur) {
                iUmin = nxt;
            } else {
                break;
            }
        }
        // Vmax (maximize dot with ve)
        for (;;) {
            const int nxt = (iVmax + 1) % m;
            const double cur = H2[iVmax].dotProduct(ve);
            const double nxtv= H2[nxt ].dotProduct(ve);
            if (nxtv > cur) {
                iVmax = nxt;
            } else {
                break;
            }
        }
        // Vmin (minimize dot with ve)
        for (;;) {
            const int nxt = (iVmin + 1) % m;
            const double cur = H2[iVmin].dotProduct(ve);
            const double nxtv= H2[nxt].dotProduct(ve);
            if (nxtv < cur) {
                iVmin = nxt;
            } else {
                break;
            }
        }

        const double minU = H2[iUmin].dotProduct(ue);
        const double maxU = H2[iUmax].dotProduct(ue);
        const double minV = H2[iVmin].dotProduct(ve);
        const double maxV = H2[iVmax].dotProduct(ve);

        const double width = (maxU - minU);
        const double height = (maxV - minV);
        const double area = width * height;

        if (area < mr.area()) {
            mr.area() = area;
            mr.width() = width;
            mr.height() = height;
            mr.parentEdge() = i;
            // angle in the (u,v) plane
            mr.psi() = std::atan2(ue.y(), ue.x());
        }
    }

    return true;
}


bool gaden::Work::calculateRotatedBoundBox(
    // outputs
    BoundBox& resultBb, Axes& resultAxes, Vector3& resultRotations,

    // inputs
    const std::vector<Vector3>& pts, int steps, int passes
) {
    // Only need to rotate pi/2 on each axis
    // No need to resolve psi axis, we use projection and solve min rectangle
    double thetaMin = 0.0;
    double thetaMax = constants::piByTwo;
    double thetaDelta = (thetaMax - thetaMin)/steps;
    double phiMin = 0.0;
    double phiMax = constants::piByTwo;
    double phiDelta = (phiMax - phiMin)/steps;

    // Best-so-far
    double bestTheta = 0.0;
    double bestPhi = 0.0;
    double bestPsi = 0.0;
    double bestVol = std::numeric_limits<double>::infinity();

    // min/max in rotated (u',v',w) frame
    BoundBox bestLocalBb;

    // world-space orthonormal basis (u', v', w)
    Axes bestAxes;

    for (int passI = 0; passI < passes; ++passI) {
        Log_Debug(""
            << "pass " << passI << ", theta=(" << thetaMin << "," << thetaMax
            << ", phi=(" << phiMin << "," << phiMax << ")"
        );
        bool finalPass = passI == (passes - 1);

        // Grid search over (theta, phi)
        for (int thetaI = 0; thetaI < steps; ++thetaI) {
            const double theta = thetaMin + thetaI*thetaDelta;

            for (int phiI = 0; phiI < steps; ++phiI) {
                const double phi = phiMin + phiI*phiDelta;
                // *** Build (u, v, w) from (theta, phi)
                // w is the look direction: yaw around +Z by theta, then pitch (declination) by phi
                const double cth = std::cos(theta);
                const double sth = std::sin(theta);
                const double cph = std::cos(phi);
                const double sph = std::sin(phi);

                Vector3 w(cth*cph, sth*cph, sph);
                if (!w.normalise()) {
                    continue;
                }

                // Choose helper t not parallel to w, form u perpendicular to w
                Vector3 t = (std::fabs(w.z()) < 0.9) ? Vector3(0.0, 0.0, 1.0) : Vector3(1.0, 0.0, 0.0);
                Vector3 u = t - w*w.dotProduct(t);
                if (!u.normalise()) {
                    continue;
                }

                Vector3 v = w.crossProduct(u);
                if (!v.normalise()) {
                    continue;
                }

                // *** Project to 2D and compute convex hull in that (u,v) plane
                if (!calculateConvexHull2dVertexIndices(pts, theta, phi)) {
                    // Degenerate projection; skip
                    continue;
                }

                // *** Solve optimal in-plane roll psi using rotating calipers over projected hull
                if (!solvePsiOnProjectedHull(u, v, pts)) {
                    // m_optimalRect still carries results for degenerate small hulls; we can proceed
                }

                const double psi = m_optimalRect.psi();

                // Rotate (u, v) by psi around w to align with rectangle sides: (u', v')
                const double cps = std::cos(psi);
                const double sps = std::sin(psi);

                // u' =  cos(psi) u + sin(psi) v
                const Vector3 uprime = u*cps + v*sps;

                // v' = -sin(psi) u + cos(psi) v
                const Vector3 vprime = u*-sps + v*cps;

                // unchanged
                const Vector3 wprime = w;

                // *** Compute min/max along (u', v', w') for current orientation
                double minU =  1e300, maxU = -1e300;
                double minV =  1e300, maxV = -1e300;
                double minW =  1e300, maxW = -1e300;

                const int n = static_cast<int>(pts.size());
                for (int i = 0; i < n; ++i) {
                    const Vector3& p = pts[i];
                    const double pu = p.dotProduct(uprime);
                    const double pv = p.dotProduct(vprime);
                    const double pw = p.dotProduct(wprime);

                    if (pu < minU) {
                        minU = pu;
                    }
                    if (pu > maxU) {
                        maxU = pu;
                    }
                    if (pv < minV) {
                        minV = pv;
                    }
                    if (pv > maxV) {
                        maxV = pv;
                    }
                    if (pw < minW) {
                        minW = pw;
                    }
                    if (pw > maxW) {
                        maxW = pw;
                    }
                }

                // matches m_optimalRect.width() numerically
                const double width  = (maxU - minU);

                // matches m_optimalRect.height() numerically
                const double height = (maxV - minV);
                const double depth  = (maxW - minW);
                const double volume = width * height * depth;

                if (volume < bestVol) {
                    bestVol   = volume;
                    bestTheta = theta;
                    bestPhi   = phi;
                    bestPsi   = psi;

                    // Store min/max in the rotated frame as a BoundBox
                    bestLocalBb = BoundBox(Vector3(minU, minV, minW), Vector3(maxU, maxV, maxW));

                    // Store axes that define that rotated frame in world coordinates
                    bestAxes = Axes(uprime, vprime, wprime);
                }
            }
        }

        // TODO - add convergence criteria to stop early when sufficiently close to answer
        if (!finalPass) {
            // TODO - add epsilon to these values as well
            thetaMin = std::max(0.0, bestTheta - thetaDelta);
            thetaMax = std::min(constants::piByTwo, bestTheta + thetaDelta);
            thetaDelta = (thetaMax - thetaMin) / steps;

            phiMin = std::max(0.0, bestPhi - phiDelta);
            phiMax = std::min(constants::piByTwo, bestPhi + phiDelta);
            phiDelta = (phiMax - phiMin) / steps;
        }
    }

    resultBb = bestLocalBb;
    resultAxes = bestAxes;
    resultRotations = Vector3(bestTheta, bestPhi, bestPsi);
    return true;
}


void gaden::Work::report() const {
    std::ostringstream oss;
    oss << "m_fileName = " << m_fileName << std::endl;
    oss << "m_faceNumber.size = " << m_faceNumber.size() << std::endl;
    oss << "m_points.size = " << m_points.size() << std::endl;
    oss << "m_normals.size = " << m_normals.size() << std::endl;
    int nPoints = m_points.size();
    for (int i = 0; i < nPoints; ++i) {
        oss << i << ":" << m_faceNumber[i];
        oss << " | " << m_points[i];
        oss << " | " << m_normals[i] << std::endl;
    }
    Log_Debug(oss.str());
}
