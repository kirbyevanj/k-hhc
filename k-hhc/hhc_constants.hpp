#ifndef HHC_CONSTANTS_HPP
#define HHC_CONSTANTS_HPP

#include <array>
#include <cstdint>
#include <cstddef>

namespace hhc {
    constexpr uint32_t BASE = 66;
    constexpr std::array<char, BASE> ALPHABET = {
        '-', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
        'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
        'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
        'v', 'w', 'x', 'y', 'z', '~'
    };

    // Sentinel marking bytes that are not part of the HHC alphabet
    constexpr uint8_t HHC_INVALID_CHAR = 0xFF;

    // Create an inverse alphabet for the HHC alphabet
    // This is used to decode the HHC encoded data and to validate input.
    // The table covers every possible byte value (256 entries) so that any
    // byte - including ones outside the ASCII range - indexes it safely.
    // Alphabet bytes map to their index; everything else maps to HHC_INVALID_CHAR.
    // The alphabet is non-contiguous in ASCII, so membership must come from this
    // table, not from a range check.
    constexpr std::array<uint8_t, 256> make_hhc_inverse_alphabet() {
        std::array<uint8_t, 256> inverse_alphabet{};
        for (auto& entry : inverse_alphabet) {
            entry = HHC_INVALID_CHAR;
        }
        for (uint32_t i = 0; i < BASE; i++) {
            inverse_alphabet[static_cast<unsigned char>(ALPHABET[i])] = static_cast<uint8_t>(i);
        }
        return inverse_alphabet;
    }
    constexpr auto INVERSE_ALPHABET = make_hhc_inverse_alphabet();

    constexpr uint32_t BITS_PER_BYTE = 8;
    constexpr size_t HHC_32BIT_STRING_LENGTH = 8;
    constexpr size_t HHC_64BIT_STRING_LENGTH = 16;
    constexpr size_t HHC_32BIT_ENCODED_LENGTH = 6;
    constexpr size_t HHC_64BIT_ENCODED_LENGTH = 11;
    constexpr auto HHC_32BIT_ENCODED_MAX_STRING = "1QLCp1";
    constexpr auto HHC_64BIT_ENCODED_MAX_STRING = "9lH9ebONzYD";
} // namespace hhc

#endif