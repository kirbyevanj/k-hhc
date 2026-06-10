#include <gtest/gtest.h>

#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <array>
#include <cstddef>
#include <limits>

using hhc::ALPHABET;
using hhc::BASE;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::HHC_INVALID_CHAR;
using hhc::INVERSE_ALPHABET;
using hhc::make_hhc_inverse_alphabet;

static_assert(BASE == 66);
static_assert(HHC_32BIT_ENCODED_LENGTH == 6);
static_assert(HHC_64BIT_ENCODED_LENGTH == 11);
static_assert(HHC_32BIT_STRING_LENGTH == 8);
static_assert(HHC_64BIT_STRING_LENGTH == 16);
static_assert(hhc::hhc_32bit_decode(HHC_32BIT_ENCODED_MAX_STRING) ==
              std::numeric_limits<uint32_t>::max());
static_assert(hhc::hhc_64bit_decode(HHC_64BIT_ENCODED_MAX_STRING) ==
              std::numeric_limits<uint64_t>::max());

/**
 * @file constants_tests.cpp
 * @brief Unit tests covering the compile-time alphabet helpers.
 */

TEST(HhcConstantsTest, DocumentedLengthConstants) {
    EXPECT_EQ(BASE, 66u);
    EXPECT_EQ(HHC_32BIT_ENCODED_LENGTH, 6u);
    EXPECT_EQ(HHC_64BIT_ENCODED_LENGTH, 11u);
    EXPECT_EQ(HHC_32BIT_STRING_LENGTH, 8u);
    EXPECT_EQ(HHC_64BIT_STRING_LENGTH, 16u);
    EXPECT_EQ(hhc::hhc_32bit_decode(HHC_32BIT_ENCODED_MAX_STRING),
              std::numeric_limits<uint32_t>::max());
    EXPECT_EQ(hhc::hhc_64bit_decode(HHC_64BIT_ENCODED_MAX_STRING),
              std::numeric_limits<uint64_t>::max());
}

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

