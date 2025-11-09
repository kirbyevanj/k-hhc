cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED BUILD_DIR)
    message(FATAL_ERROR "BUILD_DIR not provided")
endif()
if(NOT DEFINED LLVM_COV)
    message(FATAL_ERROR "LLVM_COV not provided")
endif()
if(NOT DEFINED TARGET_EXE)
    message(FATAL_ERROR "TARGET_EXE not provided")
endif()
if(NOT DEFINED PROFILE_DATA)
    message(FATAL_ERROR "PROFILE_DATA not provided")
endif()
if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR not provided")
endif()
if(NOT DEFINED IGNORE_REGEX)
    message(FATAL_ERROR "IGNORE_REGEX not provided")
endif()

file(MAKE_DIRECTORY "${OUTPUT_DIR}")

execute_process(
    COMMAND ${LLVM_COV} report ${TARGET_EXE}
            --instr-profile=${PROFILE_DATA}
            --ignore-filename-regex=${IGNORE_REGEX}
            --summary-only
    OUTPUT_FILE "${OUTPUT_DIR}/coverage.txt"
    WORKING_DIRECTORY "${BUILD_DIR}"
    RESULT_VARIABLE _report_result
)
if(_report_result)
    message(FATAL_ERROR "llvm-cov report failed with code ${_report_result}")
endif()

execute_process(
    COMMAND ${LLVM_COV} show ${TARGET_EXE}
            --instr-profile=${PROFILE_DATA}
            --format=html
            --output-dir=${OUTPUT_DIR}/html
            --ignore-filename-regex=${IGNORE_REGEX}
    WORKING_DIRECTORY "${BUILD_DIR}"
    RESULT_VARIABLE _show_result
)
if(_show_result)
    message(FATAL_ERROR "llvm-cov show failed with code ${_show_result}")
endif()

