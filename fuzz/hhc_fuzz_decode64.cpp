#include "hhc.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, [[maybe_unused]] size_t size) {
    try {
        (void)hhc::hhc_64bit_decode(reinterpret_cast<const char*>(data));
    } catch (const std::exception&) {
        /* Expected */
    }
    
    return 0;
}

