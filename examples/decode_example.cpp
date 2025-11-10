#include "hhc.hpp"

#include <iostream>
#include <cstdint>
#include <stdexcept>

/**
 * @file decode_example.cpp
 * @brief Example demonstrating HHC decoding functionality.
 */

int main() {
    std::cout << "HHC Decoding Example\n";
    std::cout << "====================\n\n";

    // Example 1: Decode 32-bit strings
    std::cout << "32-bit Decoding:\n";
    std::cout << "----------------\n";
    
    const char* encoded_32[] = {
        "------",      // 0 (padded)
        "-",           // 0 (unpadded - single padding char)
        ".",           // 1 (unpadded)
        "-----A",      // 66 (unpadded)
        "1QLCp1",      // UINT32_MAX (padded)
        "--.TNv",      // 424242 (padded)
        ".TNv"         // 424242 (unpadded)
    };
    
    for (const char* encoded : encoded_32) {
        try {
            uint32_t decoded = hhc::hhc_32bit_decode(encoded);
            std::cout << "  \"" << encoded << "\" -> " << decoded << "\n";
        } catch (const std::exception& e) {
            std::cout << "  \"" << encoded << "\" -> Error: " << e.what() << "\n";
        }
    }

    std::cout << "\n";

    // Example 2: Decode 64-bit strings
    std::cout << "64-bit Decoding:\n";
    std::cout << "----------------\n";
    
    const char* encoded_64[] = {
        "-----------",        // 0 (padded)
        "-",                  // 0 (unpadded - single padding char)
        ".",                  // 1 (unpadded)
        "9lH9ebONzYD",        // UINT64_MAX (padded)
        "-----5tVfK4",        // 9876543210 (padded)
        "5tVfK4"              // 9876543210 (unpadded)
    };
    
    for (const char* encoded : encoded_64) {
        try {
            uint64_t decoded = hhc::hhc_64bit_decode(encoded);
            std::cout << "  \"" << encoded << "\" -> " << decoded << "\n";
        } catch (const std::exception& e) {
            std::cout << "  \"" << encoded << "\" -> Error: " << e.what() << "\n";
        }
    }

    std::cout << "\n";

    // Example 3: Round-trip encoding and decoding
    std::cout << "Round-trip Example (32-bit):\n";
    std::cout << "----------------------------\n";
    
    uint32_t test_values[] = {0, 1, 42, 100, 1000, 424242};
    char buffer[hhc::HHC_32BIT_STRING_LENGTH];
    
    for (uint32_t original : test_values) {
        hhc::hhc_32bit_encode_unpadded(original, buffer);
        std::string encoded_str(buffer);
        
        // Handle empty string case (value 0 unpadded) - use single padding char for display
        std::string display_str = encoded_str.empty() ? "-" : encoded_str;
        
        try {
            // For empty string (value 0), use single padding character to decode
            const char* decode_str = encoded_str.empty() ? "-" : encoded_str.c_str();
            uint32_t decoded = hhc::hhc_32bit_decode(decode_str);
            std::cout << "  " << original << " -> \"" << display_str << "\" -> " << decoded;
            if (original == decoded) {
                std::cout << " ✓\n";
            } else {
                std::cout << " ✗ (mismatch!)\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  " << original << " -> \"" << display_str << "\" -> Error: " << e.what() << "\n";
        }
    }

    return 0;
}

