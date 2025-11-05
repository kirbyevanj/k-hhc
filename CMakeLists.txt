cmake_minimum_required(VERSION 3.20)
project(hcc-cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library
add_library(hcc-cpp INTERFACE)
target_include_directories(hcc-cpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hcc-cpp>
    $<INSTALL_INTERFACE:include>
)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(tests)
add_subdirectory(benchmarks)
