cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(k-hhc VERSION 1.0.4 LANGUAGES CXX)

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
    add_compile_definitions(LLVM_BUILD_INSTRUMENTED)
    set(HHC_COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
endif()

include(GNUInstallDirs)

option(HHC_ENABLE_FUZZING "Enable libFuzzer targets" OFF)
option(HHC_BUILD_PYTHON "Build Python bindings" OFF)

# Header-only library
add_library(k-hhc INTERFACE)
target_include_directories(k-hhc INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/k-hhc>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS k-hhc EXPORT k-hhcTargets)
install(DIRECTORY k-hhc/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp")

set(PC_FILE "${CMAKE_CURRENT_BINARY_DIR}/k-hhc.pc")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/k-hhc.pc.in ${PC_FILE} @ONLY)
install(FILES ${PC_FILE} DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(tests)
add_subdirectory(benchmarks)
add_subdirectory(examples)
if(HHC_ENABLE_FUZZING)
    add_subdirectory(fuzz)
endif()
if(HHC_BUILD_PYTHON)
    add_subdirectory(python)
endif()

if(HHC_ENABLE_COVERAGE)
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
                "-DIGNORE_REGEX=\\(tests/\\|benchmarks/\\|fuzz/\\)"
                -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_coverage.cmake
        DEPENDS hhc_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
        COMMENT "Generating LLVM coverage report"
    )
endif()
