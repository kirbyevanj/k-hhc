cmake_minimum_required(VERSION 3.15)
project(k-hhc-benchmarks VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)

# Dependencies
find_package(Threads REQUIRED)

# Static library naming follows the toolchain, not the OS: MSVC produces
# benchmark.lib while MinGW on Windows produces libbenchmark.a. CMake's
# per-toolchain variables match what the inner project (same forwarded
# compiler) emits.
set(LIB_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")

# Google Benchmark external project
# Only forward compilers that are actually set: the top-level project is
# CXX-only, so CMAKE_C_COMPILER may be empty and an explicit empty value
# would break the inner project's compiler detection.
set(_benchmark_cmake_args
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/benchmark-install
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_STANDARD=17
    -DBENCHMARK_ENABLE_TESTING=OFF
    -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    -DBENCHMARK_USE_BUNDLED_GTEST=OFF
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
)
if(CMAKE_C_COMPILER)
    list(APPEND _benchmark_cmake_args -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})
endif()

ExternalProject_Add(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    CMAKE_ARGS ${_benchmark_cmake_args}
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark${LIB_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark_main${LIB_SUFFIX}
)

# Benchmark imported targets
add_library(benchmark_lib STATIC IMPORTED)
set_target_properties(benchmark_lib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark${LIB_SUFFIX}
)

add_library(benchmark_main STATIC IMPORTED)
set_target_properties(benchmark_main PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark_main${LIB_SUFFIX}
)

# Benchmark executable
set(HHC_BENCH_SOURCES
    encode32_bench.cpp
    encode64_bench.cpp
    decode32_bench.cpp
    decode64_bench.cpp
    validate_bench.cpp
    extras_bench.cpp
    main.cpp
)

add_executable(hhc_benchmarks ${HHC_BENCH_SOURCES})
target_include_directories(hhc_benchmarks PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/include
)
add_dependencies(hhc_benchmarks benchmark)

if(WIN32)
    target_link_libraries(hhc_benchmarks PRIVATE k-hhc benchmark_lib benchmark_main Threads::Threads Shlwapi)
else()
    target_link_libraries(hhc_benchmarks PRIVATE k-hhc benchmark_lib benchmark_main Threads::Threads)
endif()
