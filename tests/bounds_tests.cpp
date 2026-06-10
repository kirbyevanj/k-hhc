#include <gtest/gtest.h>

#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <string>


using hhc::hhc_bounds_check;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;

using std::string;

/**
 * @file bounds_tests.cpp
 * @brief Unit tests for the generic bounds checking helper.
 */

static_assert(hhc_bounds_check("1QLCp1", "1QLCp1"));

TEST(HhcBoundsTest, ThirtyTwoBitEqualToMaxReturnsTrue) {
    EXPECT_TRUE(hhc_bounds_check(HHC_32BIT_ENCODED_MAX_STRING,HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitLowerThanMaxReturnsTrue) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate.back() = '0';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitHigherThanMaxReturnsFalse) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate.back() = '2';
    EXPECT_FALSE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitShorterInputReturnsTrue) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate.pop_back();
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitLongerInputPrefixEqualReturnsTrue) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate += '0';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitDivergenceInMiddleReturnsFalse) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate[1] = 'z';
    EXPECT_FALSE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitDivergenceInMiddleLowerReturnsTrue) {
    string candidate = HHC_32BIT_ENCODED_MAX_STRING;
    candidate[2] = '0';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitEqualToMaxReturnsTrue) {
    EXPECT_TRUE(hhc_bounds_check(HHC_64BIT_ENCODED_MAX_STRING, HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitLowerThanMaxReturnsTrue) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate.back() = 'C';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitHigherThanMaxReturnsFalse) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate.back() = 'E';
    EXPECT_FALSE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitShorterInputReturnsTrue) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate.pop_back();
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitLongerInputPrefixEqualReturnsTrue) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate += 'C';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitDivergenceInMiddleReturnsFalse) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate[3] = 'z';
    EXPECT_FALSE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitDivergenceInMiddleLowerReturnsTrue) {
    string candidate = HHC_64BIT_ENCODED_MAX_STRING;
    candidate[5] = '-';
    EXPECT_TRUE(hhc_bounds_check(candidate.c_str(), HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, EmptyStringReturnsTrue) {
    EXPECT_TRUE(hhc_bounds_check("", ""));
}

