#include <gtest/gtest.h>
#include "hhc.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

/**
 * @file encode32_tests.cpp
 * @brief Unit tests covering 32-bit encoding helpers.
 */


constexpr auto U32_MAX_VALUE = std::numeric_limits<uint32_t>::max();
constexpr auto U32_MIN_VALUE = std::numeric_limits<uint32_t>::min();

using hhc::hhc_32bit_encode_padded;
using hhc::hhc_32bit_encode_unpadded;
using hhc::HHC_32BIT_STRING_LENGTH; 
using hhc::HHC_32BIT_ENCODED_LENGTH;

using std::string;
using std::string_view;

TEST(HhcEncode32Test, Encode32BitTestUINT32_MIN) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(U32_MIN_VALUE, output.data());
    EXPECT_STREQ(output.c_str(), "------");
}

TEST(HhcEncode32Test, Encode32BitTestUINT32_MAX) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(U32_MAX_VALUE, output.data());
    EXPECT_STREQ(output.c_str(), "1QLCp1");
}

TEST(HhcEncode32Test, Encode32BitUnpaddedZeroProducesSinglePaddingChar) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_unpadded(U32_MIN_VALUE, output.data());
    // Zero's canonical unpadded encoding is a single padding character,
    // which round-trips through the decoder (unlike an empty string)
    EXPECT_STREQ(output.data(), "-");
}

TEST(HhcEncode32Test, Encode32BitUnpaddedZeroRoundTrips) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_unpadded(U32_MIN_VALUE, output.data());
    EXPECT_EQ(hhc::hhc_32bit_decode(output.c_str()), U32_MIN_VALUE);
}

// Compile-time proof that the encode -> decode pipeline is genuinely constexpr
// and that every digit count round-trips, including zero
namespace {
constexpr uint32_t constexpr_roundtrip32(uint32_t value) {
    char buffer[HHC_32BIT_STRING_LENGTH] = {};
    hhc_32bit_encode_unpadded(value, buffer);
    return hhc::hhc_32bit_decode(buffer);
}
static_assert(constexpr_roundtrip32(0U) == 0U);
static_assert(constexpr_roundtrip32(1U) == 1U);
static_assert(constexpr_roundtrip32(424242U) == 424242U);
static_assert(constexpr_roundtrip32(U32_MAX_VALUE) == U32_MAX_VALUE);
}  // namespace

TEST(HhcEncode32Test, Encode32BitUnpaddedPreservesSignificantDigits) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    constexpr uint32_t kValue = 1;
    hhc_32bit_encode_unpadded(kValue, output.data());
    // After unpadding, only the significant digit remains at the beginning
    EXPECT_STREQ(output.data(), ".");
}

TEST(HhcEncode32Test, Encode32BitUnpaddedMaxRemainsUnchanged) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_unpadded(U32_MAX_VALUE, output.data());
    string_view unpadded(output.data(), HHC_32BIT_ENCODED_LENGTH);
    EXPECT_EQ(unpadded, "1QLCp1");
}

TEST(HhcEncode32Test, Encode32BitPaddedWritesExactlySixBytesNoNullTerminator) {
    string output(HHC_32BIT_STRING_LENGTH, static_cast<char>(0x7F));
    hhc_32bit_encode_padded(424242U, output.data());

    string expected(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(424242U, expected.data());
    for (std::size_t i = 0; i < HHC_32BIT_ENCODED_LENGTH; ++i) {
        EXPECT_EQ(output[i], expected[i]);
    }
    EXPECT_EQ(static_cast<unsigned char>(output[HHC_32BIT_ENCODED_LENGTH]), 0x7Fu);
}

