#include "hhc.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (data == nullptr || size == 0) {
        return 0;
    }

    using namespace hhc;

    const size_t len = data[0] % (HHC_32BIT_STRING_LENGTH + 1);
    std::string buffer(len, '\0');

    if (len > 0 && size > 1) {
        const size_t copy_len = std::min(len, size - 1);
        std::memcpy(buffer.data(), data + 1, copy_len);
    }

    if (!hhc_validate_string(buffer.c_str())) {
        return 0;
    }

    if (len != HHC_32BIT_ENCODED_LENGTH) {
        return 0;
    }

    if (!hhc_32bit_bounds_check(buffer.c_str())) {
        return 0;
    }

    (void)hhc_32bit_decode(buffer.c_str());
    return 0;
}

