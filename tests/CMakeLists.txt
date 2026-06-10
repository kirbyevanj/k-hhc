cmake_minimum_required(VERSION 3.15)
project(k-hhc-tests VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)

# Dependencies
find_package(Threads REQUIRED)

# Static library naming follows the toolchain, not the OS: MSVC produces
# gtest.lib while MinGW on Windows produces libgtest.a. CMake's per-toolchain
# variables match what the inner project (same forwarded compiler) emits.
set(LIB_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}")
set(LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")

# GoogleTest external project
# Only forward compilers that are actually set: the top-level project is
# CXX-only, so CMAKE_C_COMPILER may be empty and an explicit empty value
# would break the inner project's compiler detection.
set(_gtest_cmake_args
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest-install
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_CXX_STANDARD=17
    -DBUILD_GMOCK=OFF
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -Dgtest_force_shared_crt=ON
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
)
if(CMAKE_C_COMPILER)
    list(APPEND _gtest_cmake_args -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER})
endif()

ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS ${_gtest_cmake_args}
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest${LIB_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest_main${LIB_SUFFIX}
)

# GoogleTest imported targets
add_library(gtest STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest${LIB_SUFFIX}
)

add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest_main PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest_main${LIB_SUFFIX}
)

# Test executable
set(HHC_TEST_SOURCES
    encode32_tests.cpp
    decode32_tests.cpp
    encode64_tests.cpp
    decode64_tests.cpp
    validate_tests.cpp
    bounds_tests.cpp
    unpad_tests.cpp
    constants_tests.cpp
    assert_tests.cpp
)

add_executable(hhc_tests ${HHC_TEST_SOURCES})
target_link_libraries(hhc_tests PRIVATE k-hhc gtest gtest_main Threads::Threads)
target_include_directories(hhc_tests PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/include
)
add_dependencies(hhc_tests googletest)

add_test(NAME hhc_tests COMMAND hhc_tests)
