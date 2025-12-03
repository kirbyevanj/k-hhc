if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(WARNING "Fuzzing targets skipped: libFuzzer requires Clang")
    return()
endif()

set(_hhc_fuzz_flags -fsanitize=fuzzer,address -fno-omit-frame-pointer -g)

foreach(_fuzzer IN ITEMS hhc_fuzz_decode32 hhc_fuzz_decode64)
    add_executable(${_fuzzer} ${_fuzzer}.cpp)
    target_link_libraries(${_fuzzer} PRIVATE k-hhc)
    target_compile_options(${_fuzzer} PRIVATE ${_hhc_fuzz_flags})
    target_link_options(${_fuzzer} PRIVATE ${_hhc_fuzz_flags})
    target_compile_features(${_fuzzer} PRIVATE cxx_std_17)
endforeach()

unset(_hhc_fuzz_flags)

