#include <gtest/gtest.h>
#include "hhc.hpp"

#include <string>

/**
 * @file validate_tests.cpp
 * @brief Unit tests covering validation helpers and bounds checks.
 */

namespace {

using hhc::hhc_bounds_check;
using hhc::hhc_validate_string;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;

}  // namespace

TEST(HhcValidateTest, ValidateStringAcceptsAlphabetCharacters) {
    const std::string input = "A0.-~_";
    EXPECT_TRUE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetCharactersFirstCharacter) {
    const std::string input = "!0.-~_";
    EXPECT_FALSE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetCharactersLastCharacter) {
    const std::string input = "A0.-~_!";
    EXPECT_FALSE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetAllCharacters) {
    const std::string input = "!\n{}!";
    EXPECT_FALSE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsEmptyString) {
    EXPECT_FALSE(hhc_validate_string(""));
}

TEST(HhcValidateTest, ValidateStringAccepts32BitMaxString) {
    EXPECT_TRUE(hhc_validate_string(HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcValidateTest, ValidateStringAccepts64BitMaxString) {
    EXPECT_TRUE(hhc_validate_string(HHC_64BIT_ENCODED_MAX_STRING));
}

// ========== BOUNDARY CONDITION TESTS ==========
// ALPHABET[0] = '-' (ASCII 45)
// ALPHABET.back() = '~' (ASCII 126)

TEST(HhcValidateTest, AcceptsCharacterAtLowerBoundary) {
    // Test: c == ALPHABET[0] (exactly at lower bound)
    const std::string input = "-";  // ASCII 45
    EXPECT_EQ(hhc_validate_string(input.c_str()), 1);
}

TEST(HhcValidateTest, RejectsCharacterJustBelowLowerBoundary) {
    // Test: c < ALPHABET[0] (just below lower bound)
    const std::string input = ",";  // ASCII 44 (one before '-')
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, RejectsCharacterWellBelowLowerBoundary) {
    // Test: c < ALPHABET[0] (well below lower bound)
    const std::string input = " ";  // ASCII 32 (space)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, RejectsControlCharacters) {
    // Test: c < ALPHABET[0] (control characters)
    const std::string input = "\t";  // ASCII 9 (tab)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, AcceptsCharacterAtUpperBoundary) {
    // Test: c == ALPHABET.back() (exactly at upper bound)
    const std::string input = "~";  // ASCII 126
    EXPECT_EQ(hhc_validate_string(input.c_str()), 1);
}

TEST(HhcValidateTest, RejectsCharacterJustAboveUpperBoundary) {
    // Test: c > ALPHABET.back() (just above upper bound)
    const std::string input = "\x7F";  // ASCII 127 (DEL character)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, RejectsExtendedASCIICharacters) {
    // Test: c > ALPHABET.back() (extended ASCII)
    const std::string input = "\x80";  // ASCII 128
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, RejectsHighExtendedASCIICharacters) {
    // Test: c > ALPHABET.back() (high extended ASCII)
    const std::string input = "\xFF";  // ASCII 255
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, AcceptsBothBoundaryCharacters) {
    // Test: both boundaries in one string
    const std::string input = "-~";  // ASCII 45 and 126
    EXPECT_EQ(hhc_validate_string(input.c_str()), 2);
}

TEST(HhcValidateTest, RejectsStringWithInvalidCharacterInMiddle) {
    // Test: valid chars, then invalid (below bound), then valid
    const std::string input = "ABC,DEF";  // ',' is ASCII 44
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, RejectsStringWithInvalidCharacterAtEnd) {
    // Test: valid chars, then one just above upper bound
    const std::string input = "ABC\x7F";  // DEL character (ASCII 127)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0);
}

TEST(HhcValidateTest, ReturnsCorrectLengthForValidString) {
    // Test: verify function returns correct length, not just truthy value
    const std::string input = "0123456789";
    EXPECT_EQ(hhc_validate_string(input.c_str()), 10);
}