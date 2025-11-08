#ifndef hhc_HPP
#define hhc_HPP
#include <cstdint>
#include <cassert>
#include <array>
#include <stdexcept>
#include "hhc_constants.hpp"
#include <cstring>

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
     * @return The length of the valid string, 0 if the string is invalid
     */
    constexpr std::size_t hhc_validate_string(const char* input_string) {
        assert(input_string != nullptr);
        if (*input_string == '\0') {
            return 0;
        }
        const char* const start = input_string;
        while (*input_string != '\0') {
            const auto c = static_cast<unsigned char>(*input_string++);
            if (c < ALPHABET[0] || c > ALPHABET.back()) {
                return 0;
            }
        }
        return input_string - start;
    }

    /**
     * @brief Check if a string is within the bounds of a maximum string
     * @param input_string The input string to check
     * @param max_string The maximum string to check against
     * @return True if the string is within the bounds, false otherwise
     */
    constexpr bool hhc_bounds_check(const char* input_string, const char* max_string) {
        assert(input_string != nullptr);
        assert(max_string != nullptr);
        while (*max_string != '\0') {
            const auto current = static_cast<unsigned char>(*input_string);
            const auto maximum = static_cast<unsigned char>(*max_string++);
            if (current < maximum) {
                return true;
            }
            if (current > maximum) {
                return false;
            }
            ++input_string;
            ++max_string;
        }
        return false;
    }

    /**
     * @brief Decode a 32-bit integer from a 6-character string
     * @param input_string The input string to decode
     * @return The decoded 32-bit integer
     * @throws std::invalid_argument if the string is invalid
     * @throws std::out_of_range if the string exceeds 32-bit bounds
     */
    constexpr uint32_t hhc_32bit_decode(const char* input_string) {
        if (input_string == nullptr) {
            throw std::invalid_argument("Invalid HHC string");
        }

        const std::size_t length = hhc_validate_string(input_string);
        if (length == 0 || length > HHC_32BIT_ENCODED_LENGTH) {
            throw std::invalid_argument("Invalid HHC string");
        }

        if (!hhc_bounds_check(input_string, HHC_32BIT_ENCODED_MAX_STRING)) {
            throw std::out_of_range("HHC string exceeds 32-bit bounds");
        }

        // If the string is not padded, pad it
        if (length != HHC_32BIT_ENCODED_LENGTH) {
            char padded_string[HHC_32BIT_STRING_LENGTH];
            const std::size_t padding = HHC_32BIT_ENCODED_LENGTH - length;

            std::memset(padded_string, ALPHABET[0], padding);
            std::memcpy(padded_string + padding, input_string, length);

            return hhc_32bit_decode_unsafe(padded_string);
        }

        return hhc_32bit_decode_unsafe(input_string); // Already padded
    }

    /**
     * @brief Decode a 64-bit integer from a 11-character string
     * @param input_string The input string to decode
     * @return The decoded 64-bit integer
     * @throws std::invalid_argument if the string is invalid
     * @throws std::out_of_range if the string exceeds 64-bit bounds
     */
    constexpr uint64_t hhc_64bit_decode(const char* input_string) {
        if (input_string == nullptr) {
            throw std::invalid_argument("Invalid HHC string");
        }

        const std::size_t length = hhc_validate_string(input_string);
        if (length == 0 || length > HHC_64BIT_ENCODED_LENGTH) {
            throw std::invalid_argument("Invalid HHC string");
        }

        if (!hhc_bounds_check(input_string, HHC_64BIT_ENCODED_MAX_STRING)) {
            throw std::out_of_range("HHC string exceeds 64-bit bounds");
        }

        // If the string is not padded, pad it
        if (length != HHC_64BIT_ENCODED_LENGTH) {
            char padded_string[HHC_64BIT_STRING_LENGTH];
            const std::size_t padding = HHC_64BIT_ENCODED_LENGTH - length;
            std::memset(padded_string, ALPHABET[0], padding);
            std::memcpy(padded_string + padding, input_string, length);
            return hhc_64bit_decode_unsafe(padded_string);
        }

        return hhc_64bit_decode_unsafe(input_string); // Already padded
    }
} // namespace hhc

#endif // hhc_HPP
