cmake_minimum_required(VERSION 3.20)
project(sandbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions($<$<CONFIG:Debug>:GADEN_DEBUG>)
add_compile_definitions(GADEN_RELEASE_TYPE="$<CONFIG>")

# # default to Release for single-config generators
# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#   set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
# endif()

# GLOBAL (applies to all targets)
add_compile_options(
  # GCC/Clang Debug => -g
  $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<CONFIG:Debug>>:-g>
  # nice to have on MSVC always
  $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
  # MSVC Debug => PDB info
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/Zi>
)
add_link_options(
  # MSVC link: emit PDB
  $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/DEBUG>
)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    message(STATUS "No build type set, setting Debug.")
endif()

# Add NDEBUG in Release mode (disables assert)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
endif()

# Add GADEN_DEBUG define in Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(GADEN_DEBUG)
    # set(CMAKE_CXX_FLAGS_DEBUG "-g")
    message(STATUS "Debug enabled, GADEN_DEBUG is defined.")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Release build enabled, no debug defined and stuff")
endif()

# Logging option
option(Logs "Logging (OFF by default)" OFF)

if(Logs)
    add_compile_definitions(GADEN_LOGS)
    message(STATUS "Logging is ON (enabled) in this configuration")
else(Logs)
    message(STATUS "Logging is OFF (disabled) in this configuration")
endif(Logs)

# Sources
add_executable(sandbox
    src/AutoMergingPointCloud.cpp
    src/base.cpp
    src/BoundBox.cpp
    src/ConvexHullTools.cpp
    src/Logger.cpp
    src/LoggerConfigurator.cpp
    src/main.cpp
    src/PointCloudTools.cpp
    src/Surface3.cpp
    src/Tools.cpp
    src/VectorNField.cpp
)

# Headers (public include path)
target_include_directories(sandbox PRIVATE ${CMAKE_SOURCE_DIR}/include)

# No exports — silence DLL-interface warnings by making GADEN_API empty.
target_compile_definitions(sandbox PRIVATE GADEN_API=)

# Warnings
if (MSVC)
  target_compile_options(sandbox PRIVATE /W4 /permissive- /EHsc /wd4244 /wd4267)
else()
  target_compile_options(sandbox PRIVATE -Wall -Wextra -Wpedantic)
endif()

# (Optional) trivial smoke test preset hook
include(CTest)
option(SANDBOX_BUILD_TESTS "Build tests" OFF)
if (SANDBOX_BUILD_TESTS)
  add_test(NAME runs COMMAND sandbox --smoke-test)
endif()
