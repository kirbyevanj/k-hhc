#ifndef hhc_HPP
#define hhc_HPP
#include <cstdint>
#include <cassert>
#include <array>
#include <cstdio>
#include "hcc_constants.hpp"
#define BASE 66

namespace hhc {
    constexpr uint32_t BITS_PER_BYTE = 8;
    constexpr size_t HHC_32BIT_ENCODED_LENGTH = 8;

    /**
     * @brief Encode a 32-bit integer into an 8-character string with padding, but no padding in first two characters
     * @note The first two characters are not padded, so the output string is 8 characters long
     * @note The output string is not null-terminated
     * @note This version is fastest because it does not need to assign the first two characters
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_32bit_encode_padded_raw(uint32_t input, char* output_string) {
        assert(output_string != nullptr);
        auto* output = reinterpret_cast<uint64_t*>(output_string);

        for (int iter = 7; iter >= 2; iter--) {
            const uint32_t index = input % BASE;
            input /= BASE;
            *output ^= static_cast<uint64_t>(ALPHABET[index]) << (BITS_PER_BYTE * iter);
        }
    }

    /**
     * @brief Encode a 32-bit integer into an 8-character string with padding, assigning the first two characters to '-'
     * @note The first two characters are padded with '-', so the output string is 8 characters long
     * @note The output string is not null-terminated
     * @note This version is slightly slower because it needs to assign the first two characters
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_32bit_encode_padded(uint32_t input, char* output_string) {
        assert(output_string != nullptr);
        output_string[0] = ALPHABET[0];
        output_string[1] = ALPHABET[0];
        hhc_32bit_encode_padded_raw(input, output_string);
    }

    /**
     * @brief Unpad a string by replacing the leading '-' characters with ' '
     * @note The output string is not null-terminated
     * @param output_string The output string to unpad
     */
    constexpr void hhc_unpad_string(char* output_string) {
        assert(output_string != nullptr);
        while (*output_string == ALPHABET[0]) {
            *output_string = ' ';
            output_string++;
        }
    }

    /**
     * @brief Encode a 32-bit integer into an 8-character string without padding
     * @note The output string is not null-terminated
     * @note This version is slower because it needs to unpad the string
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_32bit_encode_unpadded(uint32_t input, char* output_string) {
        assert(output_string != nullptr);
        hhc_32bit_encode_padded(input, output_string);
        hhc_unpad_string(output_string);
    }

    /**
     * @brief Decode a 32-bit integer from an 8-character string
     * @param input_string The input string to decode
     * @return The decoded 32-bit integer
     */
    constexpr uint32_t hhc_32bit_decode(const char* input_string) {
        assert(input_string != nullptr);
        uint32_t output = 0;
        uint32_t exponent = 1;

        for (int iter = 7; iter >= 2; --iter) {
            const uint32_t index = INVERSE_ALPHABET[input_string[iter]];
            output += index * exponent;
            exponent *= BASE;
        }
        return output;
    }

} // namespace hhc

#endif // hhc_HPP
