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

constexpr auto U64_MAX_VALUE = std::numeric_limits<uint64_t>::max();
constexpr auto U64_MIN_VALUE = std::numeric_limits<uint64_t>::min();

using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::HHC_64BIT_ENCODED_LENGTH;

using std::string;
using std::string_view;

TEST(HhcEncode64Test, Encode64BitTestUINT64_MIN) {
    string output(HHC_64BIT_ENCODED_LENGTH + 2, '\0');
    hhc_64bit_encode_padded(U64_MIN_VALUE, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), string(HHC_64BIT_ENCODED_LENGTH, '-'));
}

TEST(HhcEncode64Test, Encode64BitTestUINT64_MAX) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_padded(U64_MAX_VALUE, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), "9lH9ebONzYD");
}

TEST(HhcEncode64Test, Encode64BitUnpaddedZeroProducesSinglePaddingChar) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_unpadded(U64_MIN_VALUE, output.data());
    // Zero's canonical unpadded encoding is a single padding character,
    // which round-trips through the decoder (unlike an empty string)
    EXPECT_STREQ(output.data(), "-");
}

TEST(HhcEncode64Test, Encode64BitUnpaddedZeroRoundTrips) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_unpadded(U64_MIN_VALUE, output.data());
    EXPECT_EQ(hhc::hhc_64bit_decode(output.c_str()), U64_MIN_VALUE);
}

// Compile-time proof that the encode -> decode pipeline is genuinely constexpr
// and that every digit count round-trips, including zero
namespace {
constexpr uint64_t constexpr_roundtrip64(uint64_t value) {
    char buffer[HHC_64BIT_STRING_LENGTH] = {};
    hhc_64bit_encode_unpadded(value, buffer);
    return hhc::hhc_64bit_decode(buffer);
}
static_assert(constexpr_roundtrip64(0ULL) == 0ULL);
static_assert(constexpr_roundtrip64(1ULL) == 1ULL);
static_assert(constexpr_roundtrip64(9876543210ULL) == 9876543210ULL);
static_assert(constexpr_roundtrip64(U64_MAX_VALUE) == U64_MAX_VALUE);
}  // namespace

TEST(HhcEncode64Test, Encode64BitUnpaddedPreservesSignificantDigits) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    constexpr uint64_t kValue = 1;
    hhc_64bit_encode_unpadded(kValue, output.data());
    // After unpadding, only the significant digit remains at the beginning
    EXPECT_STREQ(output.data(), ".");
}

TEST(HhcEncode64Test, Encode64BitUnpaddedMaxRemainsUnchanged) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_unpadded(U64_MAX_VALUE, output.data());
    string_view unpadded(output.data(), HHC_64BIT_ENCODED_LENGTH);
    EXPECT_EQ(unpadded, "9lH9ebONzYD");
}

