cmake_minimum_required(VERSION 3.15)

project(k-hhc VERSION 1.0.9 LANGUAGES CXX)

# Build options
option(HHC_BUILD_TESTS "Build unit tests" ON)
option(HHC_BUILD_EXAMPLES "Build example programs" ON)
option(HHC_BUILD_BENCHMARKS "Build benchmarks (downloads Google Benchmark)" OFF)
option(HHC_ENABLE_COVERAGE "Enable LLVM coverage instrumentation" OFF)
option(HHC_ENABLE_FUZZING "Enable libFuzzer targets" OFF)
option(HHC_BUILD_PYTHON "Build Python bindings" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Coverage configuration
if(HHC_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Coverage support requires the Clang toolchain")
    endif()
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_compile_definitions(LLVM_BUILD_INSTRUMENTED)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    set(HHC_COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
endif()

include(GNUInstallDirs)

# Header-only library
add_library(k-hhc INTERFACE)
target_include_directories(k-hhc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/k-hhc>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(k-hhc INTERFACE cxx_std_17)

# Warnings for project targets that link k-hhc (not third-party deps).
# BUILD_INTERFACE keeps the flags out of the exported/installed target so
# downstream consumers are not forced to build with our warning set.
if(MSVC)
    target_compile_options(k-hhc INTERFACE $<BUILD_INTERFACE:/W4>)
else()
    target_compile_options(k-hhc INTERFACE $<BUILD_INTERFACE:-Wall> $<BUILD_INTERFACE:-Wextra>)
endif()

# Installation
include(CMakePackageConfigHelpers)

install(TARGETS k-hhc EXPORT k-hhcTargets)
install(DIRECTORY k-hhc/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp")

install(EXPORT k-hhcTargets
    FILE k-hhcTargets.cmake
    NAMESPACE k-hhc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/k-hhc
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/k-hhcConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/k-hhcConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/k-hhc
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/k-hhcConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/k-hhcConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/k-hhcConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/k-hhc
)

set(PC_FILE "${CMAKE_CURRENT_BINARY_DIR}/k-hhc.pc")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/k-hhc.pc.in ${PC_FILE} @ONLY)
install(FILES ${PC_FILE} DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Subdirectories
if(HHC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(HHC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
if(HHC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(HHC_ENABLE_FUZZING)
    add_subdirectory(fuzz)
endif()

if(HHC_BUILD_PYTHON)
    message(WARNING "HHC_BUILD_PYTHON is deprecated. Use 'cd python && make' or 'python -m build' instead.")
endif()

# Coverage target
if(HHC_ENABLE_COVERAGE)
    if(NOT HHC_BUILD_TESTS)
        message(FATAL_ERROR "HHC_ENABLE_COVERAGE requires HHC_BUILD_TESTS=ON")
    endif()
    find_program(LLVM_PROFDATA llvm-profdata REQUIRED)
    find_program(LLVM_COV llvm-cov REQUIRED)

    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HHC_COVERAGE_DIR}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${HHC_COVERAGE_DIR}/hhc_tests.profraw ${HHC_COVERAGE_DIR}/hhc_tests.profdata
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${HHC_COVERAGE_DIR}/hhc_tests.profraw $<TARGET_FILE:hhc_tests>
        COMMAND ${LLVM_PROFDATA} merge -sparse ${HHC_COVERAGE_DIR}/hhc_tests.profraw -o ${HHC_COVERAGE_DIR}/hhc_tests.profdata
        COMMAND ${CMAKE_COMMAND}
                -DBUILD_DIR=${CMAKE_BINARY_DIR}
                -DLLVM_COV=${LLVM_COV}
                -DTARGET_EXE=$<TARGET_FILE:hhc_tests>
                -DPROFILE_DATA=${HHC_COVERAGE_DIR}/hhc_tests.profdata
                -DOUTPUT_DIR=${HHC_COVERAGE_DIR}
                "-DIGNORE_REGEX=\\(tests/\\|benchmarks/\\|fuzz/\\|k-hhc/hhc_assert\\.hpp\\|k-hhc/hhc_constants\\.hpp\\)"
                -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_coverage.cmake
        DEPENDS hhc_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
        COMMENT "Generating LLVM coverage report"
    )
endif()
