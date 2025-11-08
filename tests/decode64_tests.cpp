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

namespace {

constexpr auto U64_MAX_VALUE = std::numeric_limits<uint64_t>::max();
constexpr auto U64_MIN_VALUE = std::numeric_limits<uint64_t>::min();

using hhc::hhc_bounds_check;
using hhc::hhc_64bit_decode;
using hhc::hhc_64bit_decode_unsafe;
using hhc::hhc_64bit_encode_padded;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::HHC_64BIT_ENCODED_MAX_STRING;

}  // namespace

TEST(HhcDecode64Test, Decode64BitTestUINT64_MIN) {
    std::string input(HHC_64BIT_STRING_LENGTH, '-');
    auto output = hhc_64bit_decode_unsafe(input.c_str());
    EXPECT_EQ(output, U64_MIN_VALUE);
}

TEST(HhcDecode64Test, Decode64BitTestUINT64_MAX) {
    const std::string input = "9lH9ebONzYD";
    auto output = hhc_64bit_decode_unsafe(input.c_str());
    EXPECT_EQ(output, U64_MAX_VALUE);
}

TEST(HhcDecode64Test, Decode64BitSafeReturnsValue) {
    std::string encoded(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_padded(9876543210ULL, encoded.data());
    encoded.resize(HHC_64BIT_ENCODED_LENGTH);
    auto decoded = hhc_64bit_decode(encoded.c_str());
    EXPECT_EQ(decoded, 9876543210ULL);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnInvalidCharacters) {
    const std::string input = "9lH9ebONz!D";
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsOnOutOfRange) {
    std::string input = HHC_64BIT_ENCODED_MAX_STRING;
    input.back() = '~';
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::out_of_range);
}

TEST(HhcDecode64Test, Decode64BitSafeThrowsTooLongString) {
    std::string input = HHC_64BIT_ENCODED_MAX_STRING;
    input += '1';
    EXPECT_THROW(hhc_64bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput0) {
    auto output = hhc_64bit_decode("-");
    EXPECT_EQ(output, 0);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput1) {
    auto output = hhc_64bit_decode(".");
    EXPECT_EQ(output, 1);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput6Characters0) {
    auto output = hhc_64bit_decode("------");
    EXPECT_EQ(output, 0);
}

TEST(HhcDecode64Test, Decode64BitSafeUnpaddedInput6Characters1) {
    auto output = hhc_64bit_decode("-----.");
    EXPECT_EQ(output, 1);
}

TEST(HhcDecode64Test, RoundTrip64BitTestFirst1Million) {
    for (uint64_t i = 0; i < 1000000; i++) {
        std::string output(HHC_64BIT_ENCODED_LENGTH + 1, 0);
        hhc_64bit_encode_padded(i, output.data());
        auto decoded = hhc_64bit_decode_unsafe(output.data());
        ASSERT_EQ(decoded, i);
    }
}

