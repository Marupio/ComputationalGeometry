#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "gaden/Surface3.hpp"
#include "gaden/ConvexHullTools.hpp"
#include "gaden/Logger.hpp"
#include "gaden/LoggerConfigurator.hpp"
#include "gaden/version.hpp"

using namespace gaden;

namespace { // anonymous namespace for local-only functionality
    struct AppOptions {
        double epsilon = 1.0e-9;
        bool hasEpsilon = false;

        int steps = 7;
        bool hasSteps = false;

        int passes = 1;
        bool hasPasses = false;

        // present => true
        bool mergePoints = false;

        std::string filePath;

        friend std::ostream& operator<<(std::ostream& os, const AppOptions& ao) {
            if (ao.hasEpsilon) {
                os << "Found 'epsilon': " << ao.epsilon << "\n";
            }
            if (ao.hasSteps) {
                os << "Found 'steps': " << ao.steps << "\n";
            }
            if (ao.hasPasses) {
                os << "Found 'passes': " << ao.passes << "\n";
            }
            if (ao.mergePoints) {
                os << "Found 'mergePoints': true\n";
            }
            os << "File path = " << ao.filePath << "\n";
            return os;
        }

    };

    static bool parse_app_options(const std::vector<std::string>& rest, AppOptions& out)
    {
        // Simple positional/flag parse:
        // Accept: --epsilon/-e <val>, --steps/-s <val>, --passes/-p <val>, --merge-points/-m
        // Last bare token is treated as filePath.
        for (size_t i = 0; i < rest.size(); ++i) {
            const std::string& a = rest[i];
            if (a == "--epsilon" || a == "-e") {
                if (i + 1 >= rest.size()) {
                    std::cerr << "Missing value after " << a << "\n";
                    return false;
                }
                out.hasEpsilon = true;
                out.epsilon = std::stod(rest[++i]);
            } else if (a == "--steps" || a == "-s") {
                if (i + 1 >= rest.size()) {
                    std::cerr << "Missing value after " << a << "\n";
                    return false;
                }
                out.steps = std::stoi(rest[++i]);
                out.hasSteps = true;
                if (out.steps < 1) {
                    out.steps = 1;
                }
            } else if (a == "--passes" || a == "-p") {
                if (i + 1 >= rest.size()) {
                    std::cerr << "Missing value after " << a << "\n";
                    return false;
                }
                out.passes = std::stoi(rest[++i]);
                out.hasPasses = true;
                if (out.passes < 1) {
                    out.passes = 1;
                }
            } else if (a == "--merge-points" || a == "-m") {
                out.mergePoints = true;
            } else {
                // treat as positional; keep last one as filePath
                out.filePath = a;
            }
        }
        if (out.filePath.empty()) {
            std::cerr << "Missing filePath argument.\n";
            return false;
        }
        return true;
    }
}


int main(int argc, char** argv)
{
    // Logger configuration
    //  Supports wide variety of flags, debug level, json-configurable input, etc.
    //  Remainder falls through to 'rest()'
    gaden::cli::LoggerConfigurator cfg("gaden-sandbox", "0.1.0", "Gaden Sandbox App");
    if (!cfg.process(argc, argv)) {
        // --help/--version/--info etc. handled inside; exiting quietly is fine.
        return 0;
    }

    // 2) Parse app-specific options from the remaining args
    AppOptions opt;
    if (!parse_app_options(cfg.rest(), opt)) {
        std::cerr
            << "Usage:\n"
            << "  sandbox.exe [logger options] "
            << "[--epsilon <double>] "
            << "[--steps <int>] "
            << "[--passes <int>] "
            << "[--merge-points] filePath\n";
        return 1;
    }

    Log_Info("Command line options\n--------------------\n" << opt << "\n");

    Log_Info("Reading " << opt.filePath);
    std::ifstream iss(opt.filePath);
    if (!iss.is_open()) {
        Log_Error("Failed to open file '" << opt.filePath << "'");
        return -1;
    }

    double readEpsilon = 0;
    if (opt.mergePoints) {
        if (opt.hasEpsilon) {
            readEpsilon = opt.epsilon;
        } else {
            readEpsilon = 1e-9;
            Log_Warn("'--mergePoints' specified but not '--epsilon' value provided.  Using 1e-9.");
        }
    }

    // Read in from csv, throw away all unnecessary data, keep only pruned points.
    Vector3Field pts;
    {
        Surface3 surface(iss, readEpsilon);
        pts.swap(surface.points());
    }
    double chEpsilon = 0.0;
    if (opt.hasEpsilon) {
        chEpsilon = opt.epsilon;
    }

    // Create 3d convex hull to prune internal points
    Vector3Field chPts;
    IntField chVerts_unused;
    std::vector<Face> chFaces_unused;

    int nDims = ConvexHullTools::calculateConvexHull3d(
        // Inputs
        pts,
        chEpsilon,

        // Outputs
        chPts,
        chVerts_unused,
        chFaces_unused
    );

    // Peak memory here
    // Throw away unnecessary data
    pts.clear();
    chVerts_unused.clear();
    chFaces_unused.clear();



    // 3) Input
    std::cout << "\nDone.\n";
    return 0;
}

#if 0

    gaden::Work work(opt.filePath);
    Log_Info("Reading '" << opt.filePath << "' ...");
    if (!work.readData()) {
        Log_Error("Failed to read input file: " << opt.filePath);
        return 2;
    }
    Log_Info("Done reading.  Points=" << work.points().size());
    work.report();

    // 4) Optional point merge
    const std::vector<Vector3>* activePoints = &work.points();
    int nMerged = 0;
    if (opt.mergePoints) {
        Log_Info("Merging points with epsilon=" << opt.epsilon << " ...");
        nMerged = work.mergePoints(opt.epsilon);
        Log_Info(""
            << "Merged " << nMerged << " duplicates; "
            << "kept " << work.mergedPoints().size() << " points."
        );
        activePoints = &work.mergedPoints();
    }

    // 5) Axis-aligned BB for reference
    Log_Info("Calculating axis-aligned bound box...");
    if (!work.calculateAxisAlignedBoundBox(*activePoints)) {
        Log_Error("Failed to calculate axis-aligned bound box.");
        return 3;
    }
    Log_Info("Axis-aligned BoundBox = " << work.axisAlignedBoundBox());

    // 6) Convex hull 3D (optional speed-up)
    Log_Info("Calculating 3D convex hull vertices...");
    const int hullDims = work.calculateConvexHull3dVertexIndices(*activePoints);
    bool validHull = hullDims == 3;
    if (!validHull) {
        Log_Warn(""
            << "Convex hull does not form a valid volume.  Topological dimensions = " << hullDims
            << ", proceeding with full point set."
        );
    }
    const std::vector<Vector3>& principal =
        validHull ? work.convexHull3dPoints() : *activePoints;

    // 7) Rotated OBB: use steps/passes from CLI
    Log_Info("Solving rotated OBB with steps=" << opt.steps << ", passes=" << opt.passes << " ...");
    BoundBox resultBb;
    Axes resultAxes;
    Vector3 resultRotations;

    bool result(
        work.calculateRotatedBoundBox(
            resultBb, resultAxes, resultRotations,
            principal, opt.steps, opt.passes
        )
    );

    // 8) Report
    Log_Info(""
        << "Done calculations:\n"
        << "  Total points: " << work.points().size() << "\n"
        << "  Merged: " << nMerged << "; Kept: "
            << (opt.mergePoints ? work.mergedPoints().size() : work.points().size()) << "\n"
        << "  Convex hull:\n"
        << "      Dimensions: " << hullDims << "\n"
        << "      Size: "
            << (validHull ? work.convexHull3dPoints().size() : principal.size()) << "\n"
        << "  Final solver result :\n"
        << "      Solver success = " << result << "\n"
        << "      BoundBox       = " << resultBb << "\n"
        << "      Axes           = " << resultAxes << "\n"
        << "      Rotation       = " << resultRotations << "\n"
    );


#endif