#include <gtest/gtest.h>

#include "hhc_constants.hpp"

#include <cstddef>
#include <array>

using hhc::make_hhc_inverse_alphabet;
using hhc::ALPHABET;
using hhc::INVERSE_ALPHABET;
using hhc::HHC_INVALID_CHAR;



/**
 * @file constants_tests.cpp
 * @brief Unit tests covering the compile-time alphabet helpers.
 */

TEST(HhcConstantsTest, MakeInverseAlphabetMatchesAlphabet) {
    const auto inverse = make_hhc_inverse_alphabet();

    for (std::size_t index = 0; index < ALPHABET.size(); ++index) {
        const auto ch = static_cast<unsigned char>(ALPHABET[index]);
        EXPECT_EQ(inverse[ch], index) << "Mismatch at alphabet index " << index;
    }
}

TEST(HhcConstantsTest, GlobalInverseAlphabetEqualsFactory) {
    EXPECT_EQ(INVERSE_ALPHABET, make_hhc_inverse_alphabet());
}

TEST(HhcConstantsTest, NonAlphabetCharactersMapToInvalidSentinel) {
    const auto inverse = make_hhc_inverse_alphabet();
    // Below the alphabet's ASCII range
    EXPECT_EQ(inverse[static_cast<unsigned char>('!')], HHC_INVALID_CHAR);
    // Inside the ASCII range '-'..'~' but NOT in the (non-contiguous) alphabet
    for (const char gap : {'/', ':', ';', '<', '=', '>', '?', '@',
                           '[', '\\', ']', '^', '`', '{', '|', '}'}) {
        EXPECT_EQ(inverse[static_cast<unsigned char>(gap)], HHC_INVALID_CHAR)
            << "Gap character '" << gap << "' must map to the invalid sentinel";
    }
    // Above the alphabet's ASCII range, including high-bit bytes
    EXPECT_EQ(inverse[0x7F], HHC_INVALID_CHAR);
    EXPECT_EQ(inverse[0x80], HHC_INVALID_CHAR);
    EXPECT_EQ(inverse[0xFF], HHC_INVALID_CHAR);
}

TEST(HhcConstantsTest, InverseAlphabetCoversAllByteValues) {
    // The table must be indexable by any byte (memory safety of decode_unsafe)
    static_assert(INVERSE_ALPHABET.size() == 256);

    // Exactly the 66 alphabet characters are valid; every other byte is invalid
    std::size_t valid_count = 0;
    for (std::size_t byte = 0; byte < INVERSE_ALPHABET.size(); ++byte) {
        if (INVERSE_ALPHABET[byte] != HHC_INVALID_CHAR) {
            ASSERT_LT(INVERSE_ALPHABET[byte], ALPHABET.size());
            EXPECT_EQ(static_cast<unsigned char>(ALPHABET[INVERSE_ALPHABET[byte]]), byte);
            ++valid_count;
        }
    }
    EXPECT_EQ(valid_count, ALPHABET.size());
}

