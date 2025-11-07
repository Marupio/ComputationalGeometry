# Sandbox — 3D Minimum Bounding Box Solver (C++ / CMake / VS Code)

This project is a show-piece example of computational geometry. It computes the **minimum-volume oriented bounding box (OBB)** of a set of 3D points using convex hull reduction and a rotating-calipers algorithm, with optional point merging.  It was written **from scratch**, and as such, has no third-party dependencies.

## Components

The library includes:

* Templated Field class and Primitives
* Library of primitives
* 3d Surface class
* Point Cloud tools
* Convex Hull tools
* BoundBox class and tools
* Logging support classes
* Data Registry and dependency tracking

### Templated Field class

A **Field** is a `vector` of values, with math operations and I/O functionality.  Values can be any primitives:

* **int** types, **`IntField`**
* **scalar** types, **`ScalarField`**
* mathematical vector types, **`Vector2Field`**, **`Vector3Field`**
* indexed variations, **`IndexedVector2Field`**, **`IndexedVector3Field`**

### Library of Primitives

The library includes all primitives mentioned above, **int**, **scalar**, **vector**, indexed vector variants.  It also includes geometric primitives, such as **Edge** and **Face** classes.

### 3d Surface class

Three dimensional surface includes points, normals, and indexing.  It also has I/O functionality, writing and reading CSV files.

### Point Cloud tools

These tools include **point merging** functions that use magnitude-squared histogram spatial reduction, and **projection to plane** functions that also perform sorted magnitude-squared point merging.

### Convex Hull tools

The library includes 2D and 3D convex hull construction from point clouds.  It also includes a **rotating calipers** algorithm for finding the minimum 2D rectangle enclosing the points.

### BoundBox class and tools

The bounding box class is feature rich, and includes axis-aligned as well as an algorithm to determine the minimum oriented bound box.

### Logging support classes

In-code logging and tracing, output to console and / or log file with call-depth indentation.  This all includes levels from **Debug4** through **Debug**, and the usual **Info**, **Warning**, **Error**.  The **`LoggerConfigurator`** abstracts away configuration of the logger, allowing for easy CLI and JSON setup.

### Data Registry and dependency tracking

Also included, behind the scenes, is a registry, tracking data objects, and noting when these get modified.  Complex derived objects can check with the registry if they are up-to-date or need to recalculate based on the state of their dependent data objects.

## Purpose

### Challenge

The original challenge that inspired this project is as follows:

Read in a CSV file with the following format:

`     faceNumber, x, y, z, nx, ny, nz`

With that data, determine the minimum oriented bounding box.

### Algorithm overview (idea)
1. **Input**
   - Reads a CSV file of the form:
     ```
     faceNumber, x, y, z, nx, ny, nz
     ```
2. **Point Merge (optional)**
   - Merges points within a given tolerance (`--epsilon`) using a magnitude-square histogram spatial reduction.
   - Reduces CPU and memory load.
3. **Convex Hull (3D)**
   - Builds a 3D convex hull; only vertices on the hull are kept for further computation.
4. **Grid Search**
   - Defines a grid over `(θ, φ)` space — yaw and declination — with `steps × steps` samples.
   - For each orientation:
     - Projects the 3D convex hull points onto a 2D plane.
     - Merges any 2D points within a tolerance
     - Computes the 2D convex hull.
     - Applies a rotating-calipers algorithm to find the optimal in-plane roll angle `ψ`.
     - Constructs the corresponding 3D oriented bounding box.
5. **Multi-Pass Refinement**
   - If multiple passes are requested (`--passes`), the search window narrows around the best solution and repeats with finer resolution.
6. **Output**
   - Reports the final bounding box, axis vectors, and intermediate counts (merged points, hull size, etc.) through the logger.


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

## Command Line Usage

```bash
sandbox.exe [logger options] [--epsilon <double>] [--steps <int>] [--passes <int>] [--merge-points] <filePath>
