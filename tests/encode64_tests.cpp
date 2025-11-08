#include <gtest/gtest.h>
#include "hhc.hpp"

#include <cstdint>
#include <limits>
#include <string>

/**
 * @file encode64_tests.cpp
 * @brief Unit tests covering 64-bit encoding helpers.
 */

namespace {

constexpr auto U64_MAX_VALUE = std::numeric_limits<uint64_t>::max();
constexpr auto U64_MIN_VALUE = std::numeric_limits<uint64_t>::min();

using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;

}  // namespace

TEST(HhcEncode64Test, Encode64BitTestUINT64_MIN) {
    std::string output(HHC_64BIT_ENCODED_LENGTH + 2, '\0');
    hhc_64bit_encode_padded(U64_MIN_VALUE, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), std::string(HHC_64BIT_ENCODED_LENGTH, '-'));
}

TEST(HhcEncode64Test, Encode64BitTestUINT64_MAX) {
    std::string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_padded(U64_MAX_VALUE, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), "9lH9ebONzYD");
}

