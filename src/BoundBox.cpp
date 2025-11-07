#include "gaden/BoundBox.hpp"
#include "gaden/PointCloudTools.hpp"
#include "gaden/ConvexHullTools.hpp"

gaden::BoundBox gaden::BoundBox::calculateAxisAlignedBoundBox(
    const Vector3Field& ptsIn
) {
    BoundBox bb;
    for (Vector3 pt : ptsIn) {
        bb.append(pt);
    }
    return bb;
}


gaden::BoundBox gaden::BoundBox::solveMinimumRotatedBoundBox(
    // outputs
    Axes& resultAxes, Vector3& resultRotations,

    // inputs
    const Vector3Field& pts, int steps, int passes, double epsilon
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
                IndexedVector2Field projPts;
                PointCloudTools::projectPointsToPlane(
                    pts,
                    theta,
                    phi,
                    epsilon,
                    projPts
                );

                IndexedVector2Field ch2Pts;
                IntField ch2Verts;
                int nCh2Dims = ConvexHullTools::calculateConvexHull2d(
                    // Inputs
                    projPts,

                    // Outputs
                    ch2Pts,
                    ch2Verts
                );
                if (nCh2Dims < 2) {
                    // Degenerate projection; skip
                    continue;
                }

                // *** Solve optimal in-plane roll psi using rotating calipers over projected hull
                // Gives us psi and the minimum rectangle (2d)
                MinRect optimalRect(ConvexHullTools::rotatingCalipers(u, v, projPts));
                // if (!mr.valid()) {
                //  // optimalRect still carries results for degenerate small hulls; proceed
                // }

                const double psi = optimalRect.psi();

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

                // matches optimalRect.width() numerically
                const double width  = (maxU - minU);

                // matches optimalRect.height() numerically
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

    resultAxes = bestAxes;
    resultRotations = Vector3(bestTheta, bestPhi, bestPsi);
    return bestLocalBb;
}
