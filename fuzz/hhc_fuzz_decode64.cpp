#include "hhc.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Create a null-terminated string from the fuzzer input
    // This prevents buffer overflows when hhc_validate_string looks for null terminator
    std::vector<char> null_terminated_input(size + 1);
    if (size > 0) {
        std::memcpy(null_terminated_input.data(), data, size);
    }
    null_terminated_input[size] = '\0';
    
    try {
        (void)hhc::hhc_64bit_decode(null_terminated_input.data());
    } catch (const std::exception&) {
        /* Expected - invalid inputs should throw */
    }
    
    return 0;
}

