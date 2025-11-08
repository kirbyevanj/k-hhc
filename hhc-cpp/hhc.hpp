#ifndef hhc_HPP
#define hhc_HPP
#include <cstdint>
#include <cassert>
#include <array>
#include <stdexcept>
#include "hcc_constants.hpp"

namespace hhc {

    /**
     * @brief Encode a 32-bit integer into a 6-character string
     * @note You must ensure the output string is at least 8 bytes long for performance reasons
     * @note The output string is not null-terminated
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_32bit_encode_padded(uint32_t input, char* output_string) {
        assert(output_string != nullptr);

        for (uint32_t pos = HHC_32BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = input % BASE;
            input /= BASE;
            output_string[pos - 1] = ALPHABET[index];

        }
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
     * @brief Encode a 32-bit integer into a 6-character string without padding
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
     * @brief Decode a 32-bit integer from a 6-character string
     * @param input_string The input string to decode
     * @return The decoded 32-bit integer
     */
    constexpr uint32_t hhc_32bit_decode_unsafe(const char* input_string) {
        assert(input_string != nullptr);
        uint32_t output = 0;
        uint32_t exponent = 1;

        for (uint32_t pos = HHC_32BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = INVERSE_ALPHABET[input_string[pos-1]];
            output += index * exponent;
            exponent *= BASE;
        }
        return output;
    }


    /**
     * @brief Encode a 64-bit integer into a 11-character string
     * @note You must ensure the output string is at least 16 bytes long for performance reasons
     * @note The output string is not null-terminated
     * @param input The 64-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_64bit_encode_padded(uint64_t input, char* output_string) {
        assert(output_string != nullptr);
        for (uint32_t pos = HHC_64BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = input % BASE;
            input /= BASE;
            output_string[pos - 1] = ALPHABET[index];
        }
    }

    /**
     * @brief Encode a 64-bit integer into a 11-character string without padding
     * @note The output string is not null-terminated
     * @note This version is slower because it needs to unpad the string
     * @param input The 64-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_64bit_encode_unpadded(uint64_t input, char* output_string) {
        assert(output_string != nullptr);
        hhc_64bit_encode_padded(input, output_string);
        hhc_unpad_string(output_string);
    }

    /**
     * @brief Decode a 64-bit integer from a 11-character string
     * @param input_string The input string to decode
     * @return The decoded 64-bit integer
     */
    constexpr uint64_t hhc_64bit_decode_unsafe(const char* input_string) {
        assert(input_string != nullptr);
        uint64_t output = 0;
        uint64_t exponent = 1;
        for (uint32_t pos = HHC_64BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = INVERSE_ALPHABET[input_string[pos-1]];
            output += static_cast<uint64_t>(index) * exponent;
            exponent *= BASE;
        }
        return output;
    }

    /**
     * @brief Validate a string to ensure it is a valid HHC string
     * @param input_string The input string to validate
     * @return True if the string is valid, false otherwise
     */
    constexpr bool hhc_validate_string(const char* input_string) {
        assert(input_string != nullptr);
        while (*input_string != '\0') {
            const auto c = static_cast<unsigned char>(*input_string++);
            if (c < ALPHABET[0] || c > ALPHABET.back()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Check if a 32-bit encoded string is within the bounds of a 32-bit integer
     * @param input_string The input string to check
     * @return True if the string is within the bounds, false otherwise
     */
    constexpr bool hhc_32bit_bounds_check(const char* input_string) {
        assert(input_string != nullptr);
        const char* max_string = HHC_32BIT_ENCODED_MAX_STRING;
        while (*max_string != '\0') {
            const auto current = static_cast<unsigned char>(*input_string);
            const auto maximum = static_cast<unsigned char>(*max_string);
            if (current < maximum) {
                return true;
            }
            if (current > maximum) {
                return false;
            }
            ++input_string;
            ++max_string;
        }
        return true;
    }

    /**
     * @brief Check if a 64-bit encoded string is within the bounds of a 64-bit integer
     * @param input_string The input string to check
     * @return True if the string is within the bounds, false otherwise
     */
    constexpr bool hhc_64bit_bounds_check(const char* input_string) {
        assert(input_string != nullptr);
        const char* max_string = HHC_64BIT_ENCODED_MAX_STRING;
        while (*max_string != '\0') {
            const auto current = static_cast<unsigned char>(*input_string);
            const auto maximum = static_cast<unsigned char>(*max_string);
            if (current < maximum) {
                return true;
            }
            if (current > maximum) {
                return false;
            }
            ++input_string;
            ++max_string;
        }
        return true;
    }

    /**
     * @brief Decode a 32-bit integer from a 6-character string
     * @param input_string The input string to decode
     * @return The decoded 32-bit integer
     * @throws std::invalid_argument if the string is invalid
     * @throws std::out_of_range if the string exceeds 32-bit bounds
     */
    constexpr uint32_t hhc_32bit_decode(const char* input_string) {
        if (!hhc_validate_string(input_string)) {
            throw std::invalid_argument("Invalid HHC string");
        }
        if (!hhc_32bit_bounds_check(input_string)) {
            throw std::out_of_range("HHC string exceeds 32-bit bounds");
        }
        return hhc_32bit_decode_unsafe(input_string);
    }

    /**
     * @brief Decode a 64-bit integer from a 11-character string
     * @param input_string The input string to decode
     * @return The decoded 64-bit integer
     * @throws std::invalid_argument if the string is invalid
     * @throws std::out_of_range if the string exceeds 64-bit bounds
     */
    constexpr uint64_t hhc_64bit_decode(const char* input_string) {
        if (!hhc_validate_string(input_string)) {
            throw std::invalid_argument("Invalid HHC string");
        }
        if (!hhc_64bit_bounds_check(input_string)) {
            throw std::out_of_range("HHC string exceeds 64-bit bounds");
        }
        return hhc_64bit_decode_unsafe(input_string);
    }
} // namespace hhc

#endif // hhc_HPP
