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

    // Create an inverse alphabet for the HHC alphabet
    // This is used to decode the HHC encoded data
    // The inverse alphabet is a mapping from the HHC alphabet to the indices of the alphabet
    constexpr std::array<uint32_t, ALPHABET.back()+1> make_hhc_inverse_alphabet() {
        std::array<uint32_t, ALPHABET.back()+1> inverse_alphabet{};
        for (uint32_t i = 0; i < BASE; i++) {
            inverse_alphabet[ALPHABET[i]] = i;
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