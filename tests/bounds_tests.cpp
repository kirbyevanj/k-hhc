#include <gtest/gtest.h>

#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <string>

/**
 * @file bounds_tests.cpp
 * @brief Unit tests for the generic bounds checking helper.
 */

TEST(HhcBoundsTest, ThirtyTwoBitEqualToMaxReturnsTrue) {
    EXPECT_TRUE(hhc::hhc_bounds_check(hhc::HHC_32BIT_ENCODED_MAX_STRING,
                                      hhc::HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitLowerThanMaxReturnsTrue) {
    std::string candidate = hhc::HHC_32BIT_ENCODED_MAX_STRING;
    candidate.back() = '0';
    EXPECT_TRUE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitHigherThanMaxReturnsFalse) {
    std::string candidate = hhc::HHC_32BIT_ENCODED_MAX_STRING;
    candidate.back() = '2';
    EXPECT_FALSE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_32BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, ThirtyTwoBitShorterInputReturnsTrue) {
    std::string candidate = hhc::HHC_32BIT_ENCODED_MAX_STRING;
    candidate.pop_back();
    EXPECT_TRUE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_32BIT_ENCODED_MAX_STRING));
}




TEST(HhcBoundsTest, SixtyFourBitEqualToMaxReturnsTrue) {
    EXPECT_TRUE(hhc::hhc_bounds_check(hhc::HHC_64BIT_ENCODED_MAX_STRING,
                                      hhc::HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitLowerThanMaxReturnsTrue) {
    std::string candidate = hhc::HHC_64BIT_ENCODED_MAX_STRING;
    candidate.back() = 'C';
    EXPECT_TRUE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitHigherThanMaxReturnsFalse) {
    std::string candidate = hhc::HHC_64BIT_ENCODED_MAX_STRING;
    candidate.back() = 'E';
    EXPECT_FALSE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, SixtyFourBitShorterInputReturnsTrue) {
    std::string candidate = hhc::HHC_64BIT_ENCODED_MAX_STRING;
    candidate.pop_back();
    EXPECT_TRUE(hhc::hhc_bounds_check(candidate.c_str(), hhc::HHC_64BIT_ENCODED_MAX_STRING));
}

TEST(HhcBoundsTest, EmptyStringReturnsTrue) {
    EXPECT_TRUE(hhc::hhc_bounds_check("", ""));
}

