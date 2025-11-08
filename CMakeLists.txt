cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(hhc-cpp VERSION 1.0.0 LANGUAGES CXX)

option(HHC_ENABLE_COVERAGE "Enable LLVM coverage instrumentation" OFF)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(HHC_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Coverage support requires the Clang toolchain")
    endif()
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
    set(HHC_COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
endif()

include(GNUInstallDirs)

option(HHC_ENABLE_FUZZING "Enable libFuzzer targets" OFF)

# Header-only library
add_library(hhc-cpp INTERFACE)
target_include_directories(hhc-cpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/hhc-cpp>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS hhc-cpp EXPORT hhc-cppTargets)
install(DIRECTORY hhc-cpp/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp")

set(PC_FILE "${CMAKE_CURRENT_BINARY_DIR}/hhc-cpp.pc")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/hhc-cpp.pc.in ${PC_FILE} @ONLY)
install(FILES ${PC_FILE} DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(tests)
add_subdirectory(benchmarks)
if(HHC_ENABLE_FUZZING)
    add_subdirectory(fuzz)
endif()

if(HHC_ENABLE_COVERAGE)
    find_program(LLVM_PROFDATA llvm-profdata REQUIRED)
    find_program(LLVM_COV llvm-cov REQUIRED)

    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${HHC_COVERAGE_DIR}
        COMMAND ${CMAKE_COMMAND} -E rm -f ${HHC_COVERAGE_DIR}/hhc_tests.profraw ${HHC_COVERAGE_DIR}/hhc_tests.profdata
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=${HHC_COVERAGE_DIR}/hhc_tests.profraw $<TARGET_FILE:hhc_tests>
        COMMAND ${LLVM_PROFDATA} merge -sparse ${HHC_COVERAGE_DIR}/hhc_tests.profraw -o ${HHC_COVERAGE_DIR}/hhc_tests.profdata
        COMMAND ${LLVM_COV} report $<TARGET_FILE:hhc_tests> --instr-profile=${HHC_COVERAGE_DIR}/hhc_tests.profdata --ignore-filename-regex="(tests/|benchmarks/|fuzz/)" --summary-only --output-file=${HHC_COVERAGE_DIR}/coverage.txt
        COMMAND ${LLVM_COV} show $<TARGET_FILE:hhc_tests> --instr-profile=${HHC_COVERAGE_DIR}/hhc_tests.profdata --format=html --output-dir=${HHC_COVERAGE_DIR}/html --ignore-filename-regex="(tests/|benchmarks/|fuzz/)"
        DEPENDS hhc_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
        COMMENT "Generating LLVM coverage report"
    )
endif()
