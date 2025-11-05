cmake_minimum_required(VERSION 3.20)

include(ExternalProject)

# Download and configure Google Benchmark
ExternalProject_Add(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/benchmark-install
        -DCMAKE_CXX_STANDARD=23
        -DBENCHMARK_ENABLE_TESTING=OFF
        -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/libbenchmark.a
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/libbenchmark_main.a
)

# Create imported target for benchmark
add_library(benchmark_lib STATIC IMPORTED)
set_target_properties(benchmark_lib PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/libbenchmark.a
)

add_library(benchmark_main STATIC IMPORTED)
set_target_properties(benchmark_main PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/libbenchmark_main.a
)

# Benchmark executable
add_executable(hcc_benchmarks benchmark_hcc.cpp)
target_link_libraries(hcc_benchmarks PRIVATE hcc-cpp benchmark_lib benchmark_main pthread)
target_include_directories(hcc_benchmarks PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/include
)
add_dependencies(hcc_benchmarks benchmark)
