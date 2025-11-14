cmake_minimum_required(VERSION 3.20)

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

# Download and configure GoogleTest
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/googletest-install
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD=20
        -DBUILD_GMOCK=OFF
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -Dgtest_force_shared_crt=ON
        -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
    BUILD_BYPRODUCTS
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest${LIB_SUFFIX}
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-install/lib/${LIB_PREFIX}gtest_main${LIB_SUFFIX}
)

# Create imported target for gtest
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

# Add test to CTest
add_test(NAME hhc_tests COMMAND hhc_tests)
