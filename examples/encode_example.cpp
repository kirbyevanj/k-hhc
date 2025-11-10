#include "hhc.hpp"

#include <iostream>
#include <cstdint>

/**
 * @file encode_example.cpp
 * @brief Example demonstrating HHC encoding functionality.
 */

int main() {
    std::cout << "HHC Encoding Example\n";
    std::cout << "====================\n\n";

    // Example 1: Encode 32-bit values
    std::cout << "32-bit Encoding:\n";
    std::cout << "----------------\n";
    
    uint32_t values_32[] = {0, 1, 42, 1000, 424242, UINT32_MAX};
    char buffer_32[hhc::HHC_32BIT_STRING_LENGTH];
    
    for (uint32_t value : values_32) {
        // Padded encoding (6 characters, always full length)
        hhc::hhc_32bit_encode_padded(value, buffer_32);
        buffer_32[hhc::HHC_32BIT_ENCODED_LENGTH] = '\0';
        std::cout << "  " << value << " -> " << buffer_32;
        
        // Unpadded encoding (variable length, null-terminated)
        hhc::hhc_32bit_encode_unpadded(value, buffer_32);
        std::cout << " (unpadded: \"" << buffer_32 << "\")\n";
    }

    std::cout << "\n";

    // Example 2: Encode 64-bit values
    std::cout << "64-bit Encoding:\n";
    std::cout << "----------------\n";
    
    uint64_t values_64[] = {0, 1, 100, 1000000, 9876543210ULL, UINT64_MAX};
    char buffer_64[hhc::HHC_64BIT_STRING_LENGTH];
    
    for (uint64_t value : values_64) {
        // Padded encoding (11 characters, always full length)
        hhc::hhc_64bit_encode_padded(value, buffer_64);
        buffer_64[hhc::HHC_64BIT_ENCODED_LENGTH] = '\0';
        std::cout << "  " << value << " -> " << buffer_64;
        
        // Unpadded encoding (variable length, null-terminated)
        hhc::hhc_64bit_encode_unpadded(value, buffer_64);
        std::cout << " (unpadded: \"" << buffer_64 << "\")\n";
    }

    return 0;
}

