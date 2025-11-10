#include <gtest/gtest.h>

#include "hhc_constants.hpp"

#include <cstddef>
#include <array>

/**
 * @file constants_tests.cpp
 * @brief Unit tests covering the compile-time alphabet helpers.
 */

TEST(HhcConstantsTest, MakeInverseAlphabetMatchesAlphabet) {
    const auto inverse = hhc::make_hhc_inverse_alphabet();

    for (std::size_t index = 0; index < hhc::ALPHABET.size(); ++index) {
        const auto ch = static_cast<unsigned char>(hhc::ALPHABET[index]);
        EXPECT_EQ(inverse[ch], index) << "Mismatch at alphabet index " << index;
    }
}

TEST(HhcConstantsTest, GlobalInverseAlphabetEqualsFactory) {
    EXPECT_EQ(hhc::INVERSE_ALPHABET, hhc::make_hhc_inverse_alphabet());
}

TEST(HhcConstantsTest, NonAlphabetCharactersDefaultToZero) {
    const auto inverse = hhc::make_hhc_inverse_alphabet();
    constexpr unsigned char non_alphabet_char = '!';
    ASSERT_EQ(hhc::ALPHABET.front(), '-');
    EXPECT_EQ(inverse[non_alphabet_char], 0u);
}

