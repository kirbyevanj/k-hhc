#include <gtest/gtest.h>
#include "hhc.hpp"

#include <algorithm>
#include <ios>
#include <string>

/**
 * @file validate_tests.cpp
 * @brief Unit tests covering validation helpers and bounds checks.
 */


using hhc::hhc_validate_string;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;

using std::string;

TEST(HhcValidateTest, ValidateStringAcceptsAlphabetCharacters) {
    const string input = "A0.-~_";
    EXPECT_TRUE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetCharactersFirstCharacter) {
    const string input = "!0.-~_";
    EXPECT_FALSE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetCharactersLastCharacter) {
    const string input = "A0.-~_!";
    EXPECT_FALSE(hhc_validate_string(input.c_str()));
}

TEST(HhcValidateTest, ValidateStringRejectsNonAlphabetAllCharacters) {
    const string input = "!\n{}!";
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
    const string input = "-";  // ASCII 45
    EXPECT_EQ(hhc_validate_string(input.c_str()), 1u);
}

TEST(HhcValidateTest, RejectsCharacterJustBelowLowerBoundary) {
    // Test: c < ALPHABET[0] (just below lower bound)
    const string input = ",";  // ASCII 44 (one before '-')
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, RejectsCharacterWellBelowLowerBoundary) {
    // Test: c < ALPHABET[0] (well below lower bound)
    const string input = " ";  // ASCII 32 (space)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, RejectsControlCharacters) {
    // Test: c < ALPHABET[0] (control characters)
    const string input = "\t";  // ASCII 9 (tab)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, AcceptsCharacterAtUpperBoundary) {
    // Test: c == ALPHABET.back() (exactly at upper bound)
    const string input = "~";  // ASCII 126
    EXPECT_EQ(hhc_validate_string(input.c_str()), 1u);
}

TEST(HhcValidateTest, RejectsCharacterJustAboveUpperBoundary) {
    // Test: c > ALPHABET.back() (just above upper bound)
    const string input = "\x7F";  // ASCII 127 (DEL character)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, RejectsExtendedASCIICharacters) {
    // Test: c > ALPHABET.back() (extended ASCII)
    const string input = "\x80";  // ASCII 128
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, RejectsHighExtendedASCIICharacters) {
    // Test: c > ALPHABET.back() (high extended ASCII)
    const string input = "\xFF";  // ASCII 255
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, AcceptsBothBoundaryCharacters) {
    // Test: both boundaries in one string
    const string input = "-~";  // ASCII 45 and 126
    EXPECT_EQ(hhc_validate_string(input.c_str()), 2u);
}

TEST(HhcValidateTest, RejectsStringWithInvalidCharacterInMiddle) {
    // Test: valid chars, then invalid (below bound), then valid
    const string input = "ABC,DEF";  // ',' is ASCII 44
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, RejectsStringWithInvalidCharacterAtEnd) {
    // Test: valid chars, then one just above upper bound
    const string input = "ABC\x7F";  // DEL character (ASCII 127)
    EXPECT_EQ(hhc_validate_string(input.c_str()), 0u);
}

TEST(HhcValidateTest, ReturnsCorrectLengthForValidString) {
    // Test: verify function returns correct length, not just truthy value
    const string input = "0123456789";
    EXPECT_EQ(hhc_validate_string(input.c_str()), 10u);
}

// ========== ALPHABET GAP TESTS ==========
// The alphabet is non-contiguous in ASCII: these characters fall inside the
// range '-'..'~' but are NOT part of the alphabet. A pure range check would
// wrongly accept them (regression test for the validation bypass).

TEST(HhcValidateTest, RejectsEveryGapCharacterInsideAsciiRange) {
    for (const char gap : {'/', ':', ';', '<', '=', '>', '?', '@',
                           '[', '\\', ']', '^', '`', '{', '|', '}'}) {
        const string input(1, gap);
        EXPECT_EQ(hhc_validate_string(input.c_str()), 0u)
            << "Gap character '" << gap << "' must be rejected";
    }
}

TEST(HhcValidateTest, RejectsGapCharacterSurroundedByValidCharacters) {
    EXPECT_EQ(hhc_validate_string("ABC/DEF"), 0u);
    EXPECT_EQ(hhc_validate_string("-----:"), 0u);
    EXPECT_EQ(hhc_validate_string("//////"), 0u);
    EXPECT_EQ(hhc_validate_string("@@@@@@"), 0u);
    EXPECT_EQ(hhc_validate_string("``````"), 0u);
}

TEST(HhcValidateTest, AcceptsExactlyTheAlphabetOverAllByteValues) {
    // Exhaustive single-byte check: a byte is valid iff it is in ALPHABET
    for (int byte = 1; byte < 256; ++byte) {  // skip 0: empty string is invalid
        const char c = static_cast<char>(byte);
        const char input[2] = {c, '\0'};
        const bool in_alphabet =
            std::find(hhc::ALPHABET.begin(), hhc::ALPHABET.end(), c) != hhc::ALPHABET.end();
        EXPECT_EQ(hhc_validate_string(input) != 0, in_alphabet)
            << "Byte 0x" << std::hex << byte << " validation mismatch";
    }
}