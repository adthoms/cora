# Based on original by David M. Rosen
# Modified for CORA by Alan Papalia

cmake_minimum_required(VERSION 3.5)

project(CORA LANGUAGES C CXX VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the default build type to debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
    message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}' as none was specified.")
else()
    message(STATUS "Build type set to '${CMAKE_BUILD_TYPE}'.")
endif()

# Directory for built libraries
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Output directory for libraries")
# Directory for built executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Output directory for executables")

# BUILD CONFIGURATIONS
# Enable faster instruction sets (SIMD/AVX)
set(ENABLE_VECTORIZATION OFF CACHE BOOL "Enable vectorized instruction sets (SIMD/AVX)? [disabled by default]")
# Enable OpenMP (if available)
set(ENABLE_OPENMP ON CACHE BOOL "Enable OpenMP (if available)")
# Enable code profiling using gperftools
set(ENABLE_PROFILING OFF CACHE BOOL "Enable code profiling using gperftools")
# Enable visualization module.
set(ENABLE_VISUALIZATION OFF CACHE BOOL "Enable visualization module.")
# Build C++ examples
set(BUILD_EXAMPLES ON CACHE BOOL "Build C++ examples.")
# Build Python bindings
set(BUILD_PYTHON_BINDINGS OFF CACHE BOOL "Build Python bindings.")

# Add the .cmake files that ship with Eigen3 to the CMake module path (useful for finding other stuff)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" CACHE STRING "The CMake module path used for this project")

# Find (system) Eigen library
find_package(Eigen3 3.3.7 REQUIRED)
if(EIGEN3_FOUND)
message(STATUS "Found Eigen3 library: ${EIGEN3_INCLUDE_DIR}\n")
endif()

if(${ENABLE_VECTORIZATION})
message(STATUS "Enabling SIMD/AVX instruction sets")
add_definitions(-march=native)
endif()

if(${ENABLE_PROFILING})
message(STATUS "Enabling code profiling using Google Performance Tools")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -lprofiler)
endif()

if(${ENABLE_VISUALIZATION})
  find_package(Pangolin REQUIRED)
endif()

if(${BUILD_PYTHON_BINDINGS})
message(STATUS "Building Python bindings")
endif()

message(STATUS "")

# DEPENDENCIES

# FIND ADDITIONAL LIBRARIES
# These next operations make use of the .cmake files shipped with Eigen3
find_package(SPQR REQUIRED)
find_package(BLAS REQUIRED)

# print out the current_source_dir
message(STATUS "Current source directory: ${CMAKE_CURRENT_SOURCE_DIR}")

# set the libs directory
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Find Optimization library
set(OPTIMIZATION_DIR ${LIBS_DIR}/Optimization/ CACHE PATH "Path to top-level directory of Optimization library (the one containing CMakeLists.txt)")

# Run the Optimization package's CMake file
add_subdirectory(${OPTIMIZATION_DIR})

# Find Preconditioners library
set(PRECONDITIONERS_DIR ${LIBS_DIR}/Preconditioners/ CACHE PATH "Path to top-level directory of Preconditioners library (the one containing CMakeLists.txt)")

# Run the Preconditioners package's CMake file
add_subdirectory(${PRECONDITIONERS_DIR})

# PERFORMANCE IMPROVEMENTS
if(${ENABLE_OPENMP})
find_package(OpenMP)
if(OPENMP_FOUND)
message(STATUS "Found OpenMP! Turning on support for parallelization\n")
endif()
endif()

# BUILD CORA LIBRARY

set(CORA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CORA_HDR_DIR ${CORA_INCLUDE_DIR}/CORA)
set(CORA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CORA_EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)

# The CORA headers and and Eigen 3, SPQR, and Cholmod are all referenced by the
# header files of the CORA library, hence must be PUBLICLY included (i.e.
# clients using the CORA headers must also include these headers)
set(CORA_INCLUDES ${CORA_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${SPQR_INCLUDES} CACHE INTERNAL "")

# The CORA header and source files
set(CORA_HDRS
${CORA_HDR_DIR}/CORA.h
)

set(CORA_SRCS
${CORA_SOURCE_DIR}/CORA.cpp
)

# The CORA library
add_library(${PROJECT_NAME} SHARED ${CORA_HDRS} ${CORA_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CORA_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SPQR_LIBRARIES} ${BLAS_LIBRARIES} ${OPTIMIZATION_LIBRARIES} ${PRECONDITIONERS_LIBRARIES})

if(OPENMP_FOUND)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${OpenMP_CXX_FLAGS})
endif()

# If visualization is enabled, set warning that this is not implemented yet
if(${ENABLE_VISUALIZATION})
message(WARNING "Visualization is not implemented yet.")
endif()

# If python bindings are enabled, set warning that this is not implemented yet
if(${BUILD_PYTHON_BINDINGS})
message(WARNING "Python bindings are not implemented yet.")
endif()

# Examples
if(${BUILD_EXAMPLES})
message(STATUS "Building c++ examples")
  add_subdirectory(${CORA_EXAMPLES_DIR})
endif()

# export CORA library

set(CMAKE_EXPORT_PACKAGE_REGISTRY TRUE)
export(PACKAGE ${PROJECT_NAME})

# configure and install CORAConfig.cmake