#include <gtest/gtest.h>
#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <stdexcept>

/**
 * @file decode64_tests.cpp
 * @brief Unit tests covering 64-bit decoding helpers.
 */


constexpr auto U64_MAX_VALUE = std::numeric_limits<uint64_t>::max();
constexpr auto U64_MIN_VALUE = std::numeric_limits<uint64_t>::min();

using hhc::hhc_bounds_check;
using hhc::hhc_64bit_decode;
using hhc::hhc_64bit_decode_unsafe;
using hhc::hhc_64bit_encode_padded;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;

using std::string;


TEST(HhcDecode64Test, Decode64BitTestUINT64_MIN) {
    string input(HHC_64BIT_STRING_LENGTH, '-');
    auto output = hhc_64bit_decode_unsafe(input.c_str());
    EXPECT_EQ(output, U64_MIN_VALUE);
}

TEST(HhcDecode64Test, Decode64BitTestUINT64_MAX) {
    const string input = "9lH9ebONzYD";
    auto output = hhc_64bit_decode_unsafe(input.c_str());
    EXPECT_EQ(output, U64_MAX_VALUE);
}

TEST(HhcDecode64Test, Decode64BitSafeReturnsValue) {
    string encoded(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_padded(9876543210ULL, encoded.data());
    encoded.resize(HHC_64BIT_ENCODED_LENGTH);
    auto decoded = hhc_64bit_decode(encoded.c_str());
    EXPECT_EQ(decoded, 9876543210ULL);
}



TEST(HhcDecode64Test, Decode64BitSafeThrowsOnInvalidCharacters) {
    const string input = "9lH9ebONz!D";
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnAlphabetGapCharacters) {
    // Regression: these bytes are inside ASCII '-'..'~' but NOT in the
    // alphabet; a range-based validator wrongly accepted them
    EXPECT_THROW(hhc_64bit_decode("///////////"), std::invalid_argument);
    EXPECT_THROW(hhc_64bit_decode("----------:"), std::invalid_argument);
    for (const char gap : {'/', ':', ';', '<', '=', '>', '?', '@',
                           '[', '\\', ']', '^', '`', '{', '|', '}'}) {
        string input = "----";
        input += gap;
        EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::invalid_argument)
            << "Gap character '" << gap << "' must not decode";
    }
}

TEST(HhcDecode64Test, Decode64BitUnsafeToleratesArbitraryBytesWithoutUB) {
    // Regression: decode_unsafe used to index the inverse table with a signed
    // char, causing an out-of-bounds read for bytes >= 0x80. The value is
    // garbage by contract, but the read must stay in bounds (ASan-clean).
    const char high_bit[] = {'\x80', '\xFF', '\x7F', '-', '-', '-',
                             '-', '-', '-', '-', '-', '\0'};
    (void)hhc_64bit_decode_unsafe(high_bit);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnOutOfRange) {
    string input = HHC_64BIT_ENCODED_MAX_STRING;
    input.back() = '~';
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::out_of_range);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsTooLongString) {
    string input = HHC_64BIT_ENCODED_MAX_STRING;
    input += '1';
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnNullString) {
    EXPECT_THROW(hhc_64bit_decode(nullptr), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnEmptyString) {
    EXPECT_THROW(hhc_64bit_decode(""), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput0) {
    auto output = hhc_64bit_decode("-");
    EXPECT_EQ(output, 0u);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput1) {
    auto output = hhc_64bit_decode(".");
    EXPECT_EQ(output, 1u);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput6Characters0) {
    auto output = hhc_64bit_decode("------");
    EXPECT_EQ(output, 0u);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput6Characters1) {
    auto output = hhc_64bit_decode("-----.");
    EXPECT_EQ(output, 1u);
}

TEST(HhcDecode64Test, RoundTrip64BitTestFirst1Million) {
    for (uint64_t i = 0; i < 1000000; i++) {
        string output(HHC_64BIT_ENCODED_LENGTH + 1, 0);
        hhc_64bit_encode_padded(i, output.data());
        auto decoded = hhc_64bit_decode_unsafe(output.data());
        ASSERT_EQ(decoded, i);
    }
}

TEST(HhcDecode64Test, RoundTrip64BitSafeDecodeUnpaddedFirst100K) {
    // Exercise the full public path: unpadded encode -> validated decode
    for (uint64_t i = 0; i < 100000; i++) {
        string output(HHC_64BIT_STRING_LENGTH, 0);
        hhc::hhc_64bit_encode_unpadded(i, output.data());
        ASSERT_EQ(hhc_64bit_decode(output.c_str()), i);
    }
}

TEST(HhcDecode64Test, RoundTrip64BitSafeDecodeBoundaryValues) {
    for (const uint64_t value : {uint64_t{0}, uint64_t{1}, uint64_t{65},
                                 uint64_t{66}, uint64_t{67},
                                 uint64_t{4294967295}, uint64_t{4294967296},
                                 U64_MAX_VALUE - 1, U64_MAX_VALUE}) {
        string padded(HHC_64BIT_STRING_LENGTH, 0);
        hhc_64bit_encode_padded(value, padded.data());
        ASSERT_EQ(hhc_64bit_decode(padded.c_str()), value);

        string unpadded(HHC_64BIT_STRING_LENGTH, 0);
        hhc::hhc_64bit_encode_unpadded(value, unpadded.data());
        ASSERT_EQ(hhc_64bit_decode(unpadded.c_str()), value);
    }
}


TEST(HhcDecode64Test, SecurityUnpaddedMaxValueAccepted) {
    // The max value (9lH9ebONzYD) should be accepted when unpadded
    EXPECT_NO_THROW({
        auto result = hhc_64bit_decode("9lH9ebONzYD");
        EXPECT_EQ(result, U64_MAX_VALUE);
    });
}

TEST(HhcDecode64Test, SecurityUnpaddedOutOfRangeRejected) {
    // Anything above max should be rejected
    // 9lH9ebONzYE is one above the max
    EXPECT_THROW(hhc_64bit_decode("9lH9ebONzYE"), std::out_of_range);
}

TEST(HhcDecode64Test, SecurityUnpaddedWellAboveMaxRejected) {
    // Well above max should also be rejected
    EXPECT_THROW(hhc_64bit_decode("9lH9ebONzYz"), std::out_of_range);
}

TEST(HhcDecode64Test, SecurityUnpaddedShorterOutOfRangeRejected) {
    // Even shorter strings can be out of range
    // Create a shorter string that would be out of range when padded
    EXPECT_THROW(hhc_64bit_decode("z----------"), std::out_of_range);
}

TEST(HhcDecode64Test, SecurityUnpaddedJustBelowMaxAccepted) {
    // Just below max should work
    EXPECT_NO_THROW({
        auto result = hhc_64bit_decode("9lH9ebONzYC");
        EXPECT_LT(result, U64_MAX_VALUE);
    });
}

