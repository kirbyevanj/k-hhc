#ifndef hhc_HPP
#define hhc_HPP
#include <cstdint>
#include <array>
#include <stdexcept>
#include "hhc_constants.hpp"
#include "hhc_assert.hpp"
#include <string>

namespace hhc {

    /**
     * @brief Encode a 32-bit integer into a 6-character string
     * @note Writes exactly HHC_32BIT_ENCODED_LENGTH (6) bytes and does not null-terminate.
     *       Use a zero-initialized buffer of HHC_32BIT_STRING_LENGTH (8) bytes if a
     *       null-terminated result is needed.
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_32bit_encode_padded(uint32_t input, char* output_string) {
        HHC_ASSERT(output_string != nullptr);

        for (uint32_t pos = HHC_32BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = input % BASE;
            input /= BASE;
            output_string[pos - 1] = ALPHABET[index];

        }
    }

    /**
     * @brief Unpad a string by removing the leading '-' padding characters and moving the significant content to the beginning
     * @note The output string is null-terminated after unpadding
     * @note A string consisting entirely of padding represents zero and unpads to a single padding character ("-"), so the result always round-trips through decode
     * @param output_string The output string to unpad (must be null-terminated)
     */
    constexpr void hhc_unpad_string(char* output_string) {
        HHC_ASSERT(output_string != nullptr);

        const char* first_significant = output_string;
        while (*first_significant == ALPHABET[0]) {
            ++first_significant;
        }

        // All characters were padding: the value is zero, whose canonical
        // unpadded encoding is a single padding character
        if (*first_significant == '\0') {
            if (first_significant != output_string) {
                output_string[0] = ALPHABET[0];
                output_string[1] = '\0';
            }
            return;
        }

        // Move the significant characters (including null terminator) to the
        // start. Destination never overtakes source, so a forward copy is safe.
        char* dst = output_string;
        while ((*dst++ = *first_significant++) != '\0') {
        }
    }

    /**
     * @brief Encode a 32-bit integer into a 6-character string without padding
     * @note The output string is null-terminated after unpadding
     * @note Zero encodes to a single padding character ("-")
     * @note This version is slower because it needs to unpad the string
     * @param input The 32-bit integer to encode
     * @param output_string The output string to write the encoded result to (must be at least HHC_32BIT_STRING_LENGTH bytes and null-terminated)
     */
    constexpr void hhc_32bit_encode_unpadded(uint32_t input, char* output_string) {
        HHC_ASSERT(output_string != nullptr);
        hhc_32bit_encode_padded(input, output_string);
        hhc_unpad_string(output_string);
    }

    /**
     * @brief Decode a 32-bit integer from a 6-character string
     * @note Performs no validation: non-alphabet bytes produce a garbage value
     *       (but never out-of-bounds memory access). Use hhc_32bit_decode for
     *       untrusted input.
     * @param input_string The input string to decode (must be at least HHC_32BIT_ENCODED_LENGTH bytes)
     * @return The decoded 32-bit integer
     */
    constexpr uint32_t hhc_32bit_decode_unsafe(const char* input_string) {
        HHC_ASSERT(input_string != nullptr);
        uint32_t output = 0;
        for (std::size_t pos = 0; pos < HHC_32BIT_ENCODED_LENGTH; ++pos) {
            const auto c = static_cast<unsigned char>(input_string[pos]);
            output = output * BASE + INVERSE_ALPHABET[c];
        }
        return output;
    }


    /**
     * @brief Encode a 64-bit integer into a 11-character string
     * @note Writes exactly HHC_64BIT_ENCODED_LENGTH (11) bytes and does not null-terminate.
     *       Use a zero-initialized buffer of HHC_64BIT_STRING_LENGTH (16) bytes if a
     *       null-terminated result is needed.
     * @param input The 64-bit integer to encode
     * @param output_string The output string to write the encoded result to
     */
    constexpr void hhc_64bit_encode_padded(uint64_t input, char* output_string) {
        HHC_ASSERT(output_string != nullptr);
        for (uint32_t pos = HHC_64BIT_ENCODED_LENGTH; pos > 0; --pos) {
            const uint32_t index = input % BASE;
            input /= BASE;
            output_string[pos - 1] = ALPHABET[index];
        }
    }

    /**
     * @brief Encode a 64-bit integer into a 11-character string without padding
     * @note The output string is null-terminated after unpadding
     * @note Zero encodes to a single padding character ("-")
     * @note This version is slower because it needs to unpad the string
     * @param input The 64-bit integer to encode
     * @param output_string The output string to write the encoded result to (must be at least HHC_64BIT_STRING_LENGTH bytes and null-terminated)
     */
    constexpr void hhc_64bit_encode_unpadded(uint64_t input, char* output_string) {
        HHC_ASSERT(output_string != nullptr);
        hhc_64bit_encode_padded(input, output_string);
        hhc_unpad_string(output_string);
    }

    /**
     * @brief Decode a 64-bit integer from a 11-character string
     * @note Performs no validation: non-alphabet bytes produce a garbage value
     *       (but never out-of-bounds memory access). Use hhc_64bit_decode for
     *       untrusted input.
     * @param input_string The input string to decode (must be at least HHC_64BIT_ENCODED_LENGTH bytes)
     * @return The decoded 64-bit integer
     */
    constexpr uint64_t hhc_64bit_decode_unsafe(const char* input_string) {
        HHC_ASSERT(input_string != nullptr);
        uint64_t output = 0;
        for (std::size_t pos = 0; pos < HHC_64BIT_ENCODED_LENGTH; ++pos) {
            const auto c = static_cast<unsigned char>(input_string[pos]);
            output = output * BASE + INVERSE_ALPHABET[c];
        }
        return output;
    }

    /**
     * @brief Validate a string to ensure it is a valid HHC string
     * @note Validity is alphabet membership via INVERSE_ALPHABET, not an ASCII
     *       range check: the alphabet is non-contiguous, so bytes such as
     *       '/' ':' '@' '`' '{' fall between alphabet characters and are invalid.
     * @param input_string The input string to validate
     * @return The length of the valid string, 0 if the string is invalid
     */
    constexpr std::size_t hhc_validate_string(const char* input_string) {
        HHC_ASSERT(input_string != nullptr);
        if (*input_string == '\0') {
            return 0;
        }
        const char* const start = input_string;
        while (*input_string != '\0') {
            const auto c = static_cast<unsigned char>(*input_string++);
            if (INVERSE_ALPHABET[c] == HHC_INVALID_CHAR) {
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
        HHC_ASSERT(input_string != nullptr);
        HHC_ASSERT(max_string != nullptr);
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
        if (input_string == nullptr) {
            throw std::invalid_argument("Invalid HHC string (nullptr)");
        }

        const std::size_t length = hhc_validate_string(input_string);
        if (length == 0 || length > HHC_32BIT_ENCODED_LENGTH) {
            throw std::invalid_argument("Invalid HHC string (length " + std::to_string(length) + ")");
        }

        // If the string is not padded, pad it (no bounds check needed - shorter strings are always valid)
        if (length < HHC_32BIT_ENCODED_LENGTH) {
            char padded_string[HHC_32BIT_STRING_LENGTH] = {};  // Zero-init also null-terminates
            const std::size_t padding = HHC_32BIT_ENCODED_LENGTH - length;

            for (std::size_t i = 0; i < padding; ++i) {
                padded_string[i] = ALPHABET[0];
            }
            for (std::size_t i = 0; i < length; ++i) {
                padded_string[padding + i] = input_string[i];
            }

            return hhc_32bit_decode_unsafe(padded_string);
        }

        // Check bounds on the already-padded string
        if (!hhc_bounds_check(input_string, HHC_32BIT_ENCODED_MAX_STRING)) {
            throw std::out_of_range("HHC string exceeds 32-bit bounds");
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
            throw std::invalid_argument("Invalid HHC string (nullptr)");
        }

        const std::size_t length = hhc_validate_string(input_string);
        if (length == 0 || length > HHC_64BIT_ENCODED_LENGTH) {
            throw std::invalid_argument("Invalid HHC string (length " + std::to_string(length) + ")");
        }

        // If the string is not padded, pad it (no bounds check needed - shorter strings are always valid)
        if (length < HHC_64BIT_ENCODED_LENGTH) {
            char padded_string[HHC_64BIT_STRING_LENGTH] = {};  // Zero-init also null-terminates
            const std::size_t padding = HHC_64BIT_ENCODED_LENGTH - length;

            for (std::size_t i = 0; i < padding; ++i) {
                padded_string[i] = ALPHABET[0];
            }
            for (std::size_t i = 0; i < length; ++i) {
                padded_string[padding + i] = input_string[i];
            }

            return hhc_64bit_decode_unsafe(padded_string);
        }

        // Check bounds on the already-padded string
        if (!hhc_bounds_check(input_string, HHC_64BIT_ENCODED_MAX_STRING)) {
            throw std::out_of_range("HHC string exceeds 64-bit bounds");
        }

        return hhc_64bit_decode_unsafe(input_string); // Already padded
    }
} // namespace hhc

#endif // hhc_HPP
