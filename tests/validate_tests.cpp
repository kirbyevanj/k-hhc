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