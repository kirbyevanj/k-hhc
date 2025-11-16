cmake_minimum_required(VERSION 3.20)
project(k-hhc-benchmarks VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)

# Find threading library
find_package(Threads REQUIRED)

# Set platform-specific library extensions
if(WIN32)
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".lib")
else()
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".a")
endif()

# Download and configure Google Benchmark
ExternalProject_Add(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/benchmark-install
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD=17
        -DBENCHMARK_ENABLE_TESTING=OFF
        -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
        -DBENCHMARK_USE_BUNDLED_GTEST=OFF
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark${LIB_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/lib/${LIB_PREFIX}benchmark_main${LIB_SUFFIX}
)

# Create imported target for benchmark
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

# Platform-specific link libraries for benchmark
if(WIN32)
    target_link_libraries(hhc_benchmarks PRIVATE k-hhc benchmark_lib benchmark_main Threads::Threads Shlwapi)
else()
    target_link_libraries(hhc_benchmarks PRIVATE k-hhc benchmark_lib benchmark_main Threads::Threads)
endif()

target_include_directories(hhc_benchmarks PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark-install/include
)
add_dependencies(hhc_benchmarks benchmark)
