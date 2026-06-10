#include <gtest/gtest.h>
#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <cstdint>
#include <limits>
#include <string>
#include <stdexcept>

/**
 * @file decode32_tests.cpp
 * @brief Unit tests covering 32-bit decoding helpers.
 */

constexpr auto U32_MAX_VALUE = std::numeric_limits<uint32_t>::max();
constexpr auto U32_MIN_VALUE = std::numeric_limits<uint32_t>::min();

using hhc::hhc_32bit_decode;
using hhc::hhc_32bit_decode_unsafe;
using hhc::hhc_32bit_encode_padded;
using hhc::hhc_validate_string;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;

using std::string;


TEST(HhcDecode32Test, Decode32BitTestUINT32_MIN) {
    const string input = "------\0\0";
    auto output = hhc_32bit_decode_unsafe(input.data());
    EXPECT_EQ(output, U32_MIN_VALUE);
}

TEST(HhcDecode32Test, Decode32BitTestUINT32_MAX) {
    const string input = "1QLCp1\0\0";
    auto output = hhc_32bit_decode_unsafe(input.data());
    EXPECT_EQ(output, U32_MAX_VALUE);
}

TEST(HhcDecode32Test, Decode32BitSafeReturnsValue) {
    string encoded(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(424242U, encoded.data());
    encoded.resize(HHC_32BIT_ENCODED_LENGTH);
    auto decoded = hhc_32bit_decode(encoded.c_str());
    EXPECT_EQ(decoded, 424242U);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsOnInvalidCharacters) {
    const string input = "1QLCP!";
    EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsOnAlphabetGapCharacters) {
    // Regression: these bytes are inside ASCII '-'..'~' but NOT in the
    // alphabet; a range-based validator wrongly accepted them and decoded
    // them as 0 (e.g. "//////" == "------" == 0)
    EXPECT_THROW(hhc_32bit_decode("//////"), std::invalid_argument);
    EXPECT_THROW(hhc_32bit_decode("-----:"), std::invalid_argument);
    EXPECT_THROW(hhc_32bit_decode("@@@@@@"), std::invalid_argument);
    EXPECT_THROW(hhc_32bit_decode("``````"), std::invalid_argument);
    for (const char gap : {'/', ':', ';', '<', '=', '>', '?', '@',
                           '[', '\\', ']', '^', '`', '{', '|', '}'}) {
        string input = "----";
        input += gap;
        EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::invalid_argument)
            << "Gap character '" << gap << "' must not decode";
    }
}

TEST(HhcDecode32Test, Decode32BitUnsafeToleratesArbitraryBytesWithoutUB) {
    // Regression: decode_unsafe used to index the inverse table with a signed
    // char, causing an out-of-bounds read for bytes >= 0x80. The value is
    // garbage by contract, but the read must stay in bounds (ASan-clean).
    const char high_bit[] = {'\x80', '\xFF', '\x7F', '-', '-', '-', '\0'};
    (void)hhc_32bit_decode_unsafe(high_bit);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsOnOutOfRange) {
    string input = HHC_32BIT_ENCODED_MAX_STRING;
    input.back() = '2';
    EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::out_of_range);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsTooLongString) {
    string input = HHC_32BIT_ENCODED_MAX_STRING;
    input.back() = '0';
    input += '.';
    EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode32Test, Decode32BitSafeEmptyString) {
    EXPECT_THROW(hhc_32bit_decode(""), std::invalid_argument);
}

TEST(HhcDecode32Test, Decode32BitSafeNullString) {
    EXPECT_THROW(hhc_32bit_decode(nullptr), std::invalid_argument);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput0) {
    auto output = hhc_32bit_decode("-");
    EXPECT_EQ(output, 0u);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput1) {
    auto output = hhc_32bit_decode(".");
    EXPECT_EQ(output, 1u);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput6Characters0) {
    auto output = hhc_32bit_decode("------");
    EXPECT_EQ(output, 0u);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput6Characters1) {
    auto output = hhc_32bit_decode("-----.");
    EXPECT_EQ(output, 1u);
}

TEST(HhcDecode32Test, RoundTrip32BitTestFirst1Million) {
    for (uint32_t i = 0; i < 1000000; i++) {
        string output(HHC_32BIT_STRING_LENGTH, 0);
        hhc_32bit_encode_padded(i, output.data());
        auto decoded = hhc_32bit_decode_unsafe(output.data());
        ASSERT_EQ(decoded, i);
    }
}

TEST(HhcDecode32Test, RoundTrip32BitSafeDecodeUnpaddedFirst100K) {
    // Exercise the full public path: unpadded encode -> validated decode
    for (uint32_t i = 0; i < 100000; i++) {
        string output(HHC_32BIT_STRING_LENGTH, 0);
        hhc::hhc_32bit_encode_unpadded(i, output.data());
        ASSERT_EQ(hhc_32bit_decode(output.c_str()), i);
    }
}

TEST(HhcDecode32Test, RoundTrip32BitSafeDecodeBoundaryValues) {
    for (const uint32_t value : {0U, 1U, 65U, 66U, 67U, 4355U, 4356U,
                                 U32_MAX_VALUE - 1, U32_MAX_VALUE}) {
        string padded(HHC_32BIT_STRING_LENGTH, 0);
        hhc_32bit_encode_padded(value, padded.data());
        ASSERT_EQ(hhc_32bit_decode(padded.c_str()), value);

        string unpadded(HHC_32BIT_STRING_LENGTH, 0);
        hhc::hhc_32bit_encode_unpadded(value, unpadded.data());
        ASSERT_EQ(hhc_32bit_decode(unpadded.c_str()), value);
    }
}


TEST(HhcDecode32Test, SecurityUnpaddedMaxValueAccepted) {
    // The max value (1QLCp1) should be accepted when unpadded
    EXPECT_NO_THROW({
        auto result = hhc_32bit_decode("1QLCp1");
        EXPECT_EQ(result, U32_MAX_VALUE);
    });
}

TEST(HhcDecode32Test, SecurityUnpaddedOutOfRangeRejected) {
    // Anything above max (1QLCp1) should be rejected
    // 1QLCp2 is one above the max
    EXPECT_THROW(hhc_32bit_decode("1QLCp2"), std::out_of_range);
}

TEST(HhcDecode32Test, SecurityUnpaddedWellAboveMaxRejected) {
    // Well above max should also be rejected
    EXPECT_THROW(hhc_32bit_decode("1QLCpz"), std::out_of_range);
}

TEST(HhcDecode32Test, SecurityUnpaddedShorterOutOfRangeRejected) {
    // Even shorter strings can be out of range
    // Create a shorter string that would be out of range when padded
    EXPECT_THROW(hhc_32bit_decode("2-----"), std::out_of_range);
}

TEST(HhcDecode32Test, SecurityUnpaddedJustBelowMaxAccepted) {
    // Just below max should work
    EXPECT_NO_THROW({
        auto result = hhc_32bit_decode("1QLCp0");
        EXPECT_LT(result, U32_MAX_VALUE);
    });
}



