#include "gaden/ConvexHullTools.hpp"

#include <numeric>

int gaden::ConvexHullTools::calculateConvexHull3d(
    // Inputs
    const Vector3Field& ptsIn,
    double toleranceIn,

    // Outputs
    Vector3Field& ptsOut,
    IntField& chVerticesOut,
    std::vector<Face> chFacesOut
) {
    // Renaming for readability
    const Vector3Field& P = ptsIn;
    IntField& out = chVerticesOut;

    const int n = P.size();
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
    if (p2 == p0 || best <= toleranceIn) {
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
    Face seed(static_cast<int>(p0), static_cast<int>(p1), static_cast<int>(p2), P, toleranceIn);

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
    if (p3 == p0 || best <= toleranceIn) {
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
            if (d.magSqr() <= toleranceIn*toleranceIn) {
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
        seed = Face(static_cast<int>(p0), static_cast<int>(p2), static_cast<int>(p1), P, toleranceIn);
    }

    // Prepare faces output, rename for brevity
    std::vector<Face>& faces(chFacesOut);
    faces.clear();
    faces.reserve(32);
    // Initial tet faces, CCW as seen from outside
    faces.emplace_back(static_cast<int>(p0), static_cast<int>(p1), static_cast<int>(p2), P, toleranceIn);
    faces.emplace_back(static_cast<int>(p0), static_cast<int>(p2), static_cast<int>(p3), P, toleranceIn);
    faces.emplace_back(static_cast<int>(p2), static_cast<int>(p1), static_cast<int>(p3), P, toleranceIn);
    faces.emplace_back(static_cast<int>(p1), static_cast<int>(p0), static_cast<int>(p3), P, toleranceIn);

    // Assign outside sets
    for (int i = 0; i < n; ++i) {
        if (i == p0 || i == p1 || i == p2 || i == p3) {
            // skip tet vertices
            continue;
        }
        // tolerance to ignore near-boundary points (i.e. <= toleranceIn)
        double bestDist = toleranceIn;
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
                if (faces[i].visibleFrom(P[perspectivePoint], toleranceIn)) {
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
            Face nf(e.u(), e.v(), perspectivePoint, P, toleranceIn);
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
            double bestD = toleranceIn;
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
    ptsOut.clear();
    ptsOut.reserve(nPts);
    for (int i : out) {
        ptsOut.push_back(P[i]);
    }
    return 3;
}


int gaden::ConvexHullTools::calculateConvexHull2d(
    // Inputs
    const IndexedVector2Field& ptsIn,

    // Outputs
    IndexedVector2Field& ptsOut,
    IntField& verticesOut
) {
    int nPts = static_cast<int>(ptsIn.size());

    // Degenerate cases
    verticesOut.clear();
    if (nPts <= 1) {
        if (nPts == 1) {
            verticesOut.push_back(0);
            return 0;
        }
        return -1;
    }

    if (ptsIn.size() == 1) {
        verticesOut.push_back(ptsIn[0].idx());
        return 0;
    }
    if (ptsIn.size() == 2) {
        verticesOut.push_back(ptsIn[0].idx());
        verticesOut.push_back(ptsIn[1].idx());
        return 1;
    }

    // Lower hull
    ptsOut.clear();
    int nA = static_cast<int>(ptsIn.size());
    ptsOut.reserve(nA*2);
    for (int i = 0; i < nA; ++i) {
        while (ptsOut.size() >= 2) {
            const IndexedVector2 a = ptsOut[ptsOut.size()-2];
            const IndexedVector2 b = ptsOut[ptsOut.size()-1];
            const IndexedVector2 c = ptsIn[i];
            const IndexedVector2 ab = b - a;
            const IndexedVector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                // keep CCW turns; collinear -> pop to keep outermost
                ptsOut.pop_back();
            } else {
                break;
            }
        }
        ptsOut.push_back(ptsIn[i]);
    }

    // Upper hull
    const int lowerSize = static_cast<int>(ptsOut.size());
    for (int i = ptsIn.size()-2; i >= 0; --i) {
        while (ptsOut.size() > lowerSize) {
            const IndexedVector2 a = ptsOut[ptsOut.size()-2];
            const IndexedVector2 b = ptsOut[ptsOut.size()-1];
            const IndexedVector2 c = ptsIn[i];
            const IndexedVector2 ab = b - a;
            const IndexedVector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                ptsOut.pop_back();
            } else {
                break;
            }
        }
        ptsOut.push_back(ptsIn[i]);
    }

    // Last point equals first; remove it
    if (!ptsOut.empty()) {
        ptsOut.pop_back();
    }

    // Convert back to original indices (CCW polygon)
    verticesOut.reserve(ptsOut.size());
    int nptsOut = static_cast<int>(ptsOut.size());
    for (int i = 0; i < nptsOut; ++i) {
        verticesOut.push_back(ptsOut[i].idx());
    }
    return 2;
}


gaden::MinRect gaden::ConvexHullTools::rotatingCalipers(
    const Vector3& u,
    const Vector3& v,
    const std::vector<Vector3>& pts
) {
    MinRect mr;
    mr.clear();
    const int m = static_cast<int>(m_convexHull2dIndices.size());
    if (m <= 0) {
        mr.valid() = false;
        return mr;
    }
    if (m == 1) {
        mr.area() = 0.0;
        mr.width() = 0.0;
        mr.height() = 0.0;
        mr.psi() = 0.0;
        mr.parentEdge() = 0;
        mr.valid() = false;
        return mr;
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
        mr.valid() = false;
        return mr;
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
    mr.valid() = true;
    return mr;
}
