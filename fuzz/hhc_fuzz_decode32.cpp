#include "hhc.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
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
        const uint32_t value = hhc::hhc_32bit_decode(null_terminated_input.data());

        // Round-trip oracle: any accepted input must decode to a value whose
        // canonical re-encodings decode back to the same value
        char unpadded[hhc::HHC_32BIT_STRING_LENGTH] = {};
        hhc::hhc_32bit_encode_unpadded(value, unpadded);
        if (hhc::hhc_32bit_decode(unpadded) != value) {
            __builtin_trap();
        }

        char padded[hhc::HHC_32BIT_STRING_LENGTH] = {};
        hhc::hhc_32bit_encode_padded(value, padded);
        if (hhc::hhc_32bit_decode(padded) != value ||
            hhc::hhc_32bit_decode_unsafe(padded) != value) {
            __builtin_trap();
        }
    } catch (const std::exception&) {
        /* Expected - invalid inputs should throw */
    }

    // The unsafe decoder must be memory-safe for arbitrary bytes (the value is
    // garbage by contract, but reads must stay in bounds)
    if (size >= hhc::HHC_32BIT_ENCODED_LENGTH) {
        (void)hhc::hhc_32bit_decode_unsafe(null_terminated_input.data());
    }

    return 0;
}
