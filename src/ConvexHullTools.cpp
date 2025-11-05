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

    // Cache actual hull points (for downstream sttoleranceIn)
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
    const Vector2Field& ptsIn,

    // Outputs
    Vector2Field& ptsOut,
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

    // Alias
    const Vector2Field& ptsIn(ptsIn);
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
    std::vector<Vector2> H;
    int nA = static_cast<int>(ptsIn.size());
    H.reserve(nA*2);
    for (int i = 0; i < nA; ++i) {
        while (H.size() >= 2) {
            const Vector2 a = H[H.size()-2];
            const Vector2 b = H[H.size()-1];
            const Vector2 c = ptsIn[i];
            const Vector2 ab = b - a;
            const Vector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                // keep CCW turns; collinear -> pop to keep outermost
                H.pop_back();
            } else {
                break;
            }
        }
        H.push_back(ptsIn[i]);
    }

    // Upper hull
    const int lowerSize = static_cast<int>(H.size());
    for (int i = ptsIn.size()-2; i >= 0; --i) {
        while (H.size() > lowerSize) {
            const Vector2 a = H[H.size()-2];
            const Vector2 b = H[H.size()-1];
            const Vector2 c = ptsIn[i];
            const Vector2 ab = b - a;
            const Vector2 ac = c - a;
            if (ab.crossProduct(ac) <= 0.0) {
                H.pop_back();
            } else {
                break;
            }
        }
        H.push_back(ptsIn[i]);
    }

    // Last point equals first; remove it
    if (!H.empty()) {
        H.pop_back();
    }

    // Convert back to original indices (CCW polygon)
    verticesOut.reserve(H.size());
    int nH = static_cast<int>(H.size());
    for (int i = 0; i < nH; ++i) {
        verticesOut.push_back(H[i].idx());
    }
    return 2;
}
