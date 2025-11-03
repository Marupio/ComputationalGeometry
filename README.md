# Sandbox — 3D Minimum Bounding Box Solver (C++ / CMake / VS Code)

This project computes the **minimum-volume oriented bounding box (OBB)** of a set of 3D points using convex hull reduction and a rotating-calipers algorithm, with optional point merging.
Originally written as a rapid prototype to demonstrate a complete geometric solution — from CSV import to full 3D bounding box optimisation.

---

## 🚀 Quick Start (VS Code)

1. Open this folder in **VS Code**.
2. Install:
   - *CMake Tools* extension
   - A C++20 compiler (MSVC, Clang, or GCC)
3. **Build:**
   - `Ctrl+Shift+P → "CMake: Build"`
   - or use `cmake --preset default && cmake --build --preset default`
4. **Run:**
   - `Ctrl+F5`
   - or run the built binary directly:
     ```bash
     ./build/default/sandbox.exe path/to/points.csv --merge-points --log-level Info
     ```

---

## Algorithm Overview

Given a CSV file of 3D points and normals:

1. **Input**
   - Reads a CSV file of the form:
     ```
     faceNumber, x, y, z, nx, ny, nz
     ```
2. **Point Merge (optional)**
   - Merges points within a given tolerance (`--epsilon`) using a spatial hash.
   - Reduces CPU and memory load.
3. **Convex Hull (3D)**
   - Builds a 3D convex hull; only vertices on the hull are kept for further computation.
4. **Grid Search**
   - Defines a grid over `(θ, φ)` space — yaw and declination — with `steps × steps` samples.
   - For each orientation:
     - Projects the 3D convex hull points onto a 2D plane.
     - Computes the 2D convex hull.
     - Applies a rotating-calipers algorithm to find the optimal in-plane roll angle `ψ`.
     - Constructs the corresponding 3D oriented bounding box.
5. **Multi-Pass Refinement**
   - If multiple passes are requested (`--passes`), the search window narrows around the best solution and repeats with finer resolution.
6. **Output**
   - Reports the final bounding box, axis vectors, and intermediate counts (merged points, hull size, etc.) through the logger.

---

## Command Line Usage

```bash
sandbox.exe [logger options] [--epsilon <double>] [--steps <int>] [--passes <int>] [--merge-points] <filePath>
