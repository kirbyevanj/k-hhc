#include <gtest/gtest.h>

#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <array>
#include <cstddef>

/**
 * @file unpad_tests.cpp
 * @brief Unit tests covering the HHC string unpadding helper.
 */

using hhc::ALPHABET;

using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::hhc_unpad_string;

using std::array;

constexpr char PAD = ALPHABET[0];



TEST(HhcUnpadStringTest, RemovesLeadingPadding) {
    array<char, HHC_32BIT_STRING_LENGTH> buffer{
        PAD, PAD, PAD, 'A', 'B', 'C', 'D', '\0'};

    hhc_unpad_string(buffer.data());

    // After unpadding, the non-padded content is moved to the beginning and null-terminated
    EXPECT_STREQ(buffer.data(), "ABCD");
    EXPECT_EQ(buffer[4], '\0');
}

TEST(HhcUnpadStringTest, StopsAtFirstNonPaddingCharacter) {
    array<char, HHC_32BIT_STRING_LENGTH> buffer{
        '.', 'A', 'B', 'C', 'D', 'E', 'F', '\0'};

    hhc_unpad_string(buffer.data());

    EXPECT_EQ(buffer[0], '.');
    EXPECT_EQ(buffer[1], 'A');
}

TEST(HhcUnpadStringTest, RemovesAllLeadingPaddingBeforeSignificantCharacter) {
    array<char, HHC_32BIT_STRING_LENGTH> buffer{
        PAD, PAD, PAD, PAD, PAD, PAD, 'X', '\0'};

    hhc_unpad_string(buffer.data());

    // After unpadding, only the non-padded character remains at the beginning
    EXPECT_STREQ(buffer.data(), "X");
    EXPECT_EQ(buffer[1], '\0');
}

TEST(HhcUnpadStringTest, AllPaddingUnpadsToSinglePaddingCharacter) {
    // An all-padding string encodes zero; its canonical unpadded form is a
    // single padding character so the result still decodes back to zero
    array<char, HHC_32BIT_STRING_LENGTH> buffer{
        PAD, PAD, PAD, PAD, PAD, PAD, '\0', '\0'};

    hhc_unpad_string(buffer.data());

    EXPECT_STREQ(buffer.data(), "-");
    EXPECT_EQ(buffer[0], PAD);
    EXPECT_EQ(buffer[1], '\0');
}

TEST(HhcUnpadStringTest, HandlesEmptyString) {
    // Edge case: empty string should not crash
    array<char, HHC_32BIT_STRING_LENGTH> buffer{'\0'};

    hhc_unpad_string(buffer.data());

    EXPECT_STREQ(buffer.data(), "");
    EXPECT_EQ(buffer[0], '\0');
}

TEST(HhcUnpadStringTest, HandlesSingleNonPaddingCharacter) {
    // Edge case: single character that's not padding
    array<char, HHC_32BIT_STRING_LENGTH> buffer{'X', '\0'};

    hhc_unpad_string(buffer.data());

    EXPECT_STREQ(buffer.data(), "X");
    EXPECT_EQ(buffer[0], 'X');
    EXPECT_EQ(buffer[1], '\0');
}

TEST(HhcUnpadStringTest, HandlesSinglePaddingCharacter) {
    // Edge case: a single padding character is already the canonical zero
    // encoding and must remain unchanged
    array<char, HHC_32BIT_STRING_LENGTH> buffer{PAD, '\0'};

    hhc_unpad_string(buffer.data());

    EXPECT_STREQ(buffer.data(), "-");
    EXPECT_EQ(buffer[0], PAD);
    EXPECT_EQ(buffer[1], '\0');
}

