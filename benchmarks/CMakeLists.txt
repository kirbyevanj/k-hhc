cmake_minimum_required(VERSION 3.20)
project(hhc-cpp-benchmarks VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)

# Download and configure Google Benchmark
ExternalProject_Add(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/benchmark-install
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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
target_link_libraries(hhc_benchmarks PRIVATE hhc-cpp benchmark_lib benchmark_main pthread)
target_include_directories(hhc_benchmarks PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/include
)
add_dependencies(hhc_benchmarks benchmark)
