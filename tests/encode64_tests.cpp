#include <gtest/gtest.h>
#include "hhc.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

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

TEST(HhcEncode64Test, Encode64BitUnpaddedZeroProducesSpaces) {
    std::string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_unpadded(U64_MIN_VALUE, output.data());
    // After unpadding, all padding is removed and string is null-terminated
    // For zero (all padding), the result should be an empty string
    EXPECT_STREQ(output.data(), "");
}

TEST(HhcEncode64Test, Encode64BitUnpaddedPreservesSignificantDigits) {
    std::string output(HHC_64BIT_STRING_LENGTH, '\0');
    constexpr uint64_t kValue = 1;
    hhc_64bit_encode_unpadded(kValue, output.data());
    // After unpadding, only the significant digit remains at the beginning
    EXPECT_STREQ(output.data(), ".");
}

TEST(HhcEncode64Test, Encode64BitUnpaddedMaxRemainsUnchanged) {
    std::string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_unpadded(U64_MAX_VALUE, output.data());
    std::string_view unpadded(output.data(), hhc::HHC_64BIT_ENCODED_LENGTH);
    EXPECT_EQ(unpadded, "9lH9ebONzYD");
}

