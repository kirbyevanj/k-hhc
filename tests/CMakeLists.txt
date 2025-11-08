cmake_minimum_required(VERSION 3.20)

include(ExternalProject)

# Download and configure GoogleTest
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest-install
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD=23
        -DBUILD_GMOCK=OFF
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/libgtest.a
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/libgtest_main.a
)

# Create imported target for gtest
add_library(gtest STATIC IMPORTED)
set_target_properties(gtest PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/libgtest.a
)

add_library(gtest_main STATIC IMPORTED)
set_target_properties(gtest_main PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/libgtest_main.a
)

# Test executable
set(HHC_TEST_SOURCES
    encode32_tests.cpp
    decode32_tests.cpp
    encode64_tests.cpp
    decode64_tests.cpp
    validate_tests.cpp
)
add_executable(hhc_tests ${HHC_TEST_SOURCES})
target_link_libraries(hhc_tests PRIVATE hhc-cpp gtest gtest_main pthread)
target_include_directories(hhc_tests PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/include
)
add_dependencies(hhc_tests googletest)

# Add test to CTest
add_test(NAME hhc_tests COMMAND hhc_tests)
