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

namespace {

constexpr auto U32_MAX_VALUE = std::numeric_limits<uint32_t>::max();
constexpr auto U32_MIN_VALUE = std::numeric_limits<uint32_t>::min();

using hhc::hhc_32bit_decode;
using hhc::hhc_32bit_decode_unsafe;
using hhc::hhc_32bit_encode_padded;
using hhc::hhc_validate_string;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_32BIT_ENCODED_MAX_STRING;

}  // namespace

TEST(HhcDecode32Test, Decode32BitTestUINT32_MIN) {
    const std::string input = "------\0\0";
    auto output = hhc_32bit_decode_unsafe(input.data());
    EXPECT_EQ(output, U32_MIN_VALUE);
}

TEST(HhcDecode32Test, Decode32BitTestUINT32_MAX) {
    const std::string input = "1QLCp1\0\0";
    auto output = hhc_32bit_decode_unsafe(input.data());
    EXPECT_EQ(output, U32_MAX_VALUE);
}

TEST(HhcDecode32Test, Decode32BitSafeReturnsValue) {
    std::string encoded(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(424242U, encoded.data());
    encoded.resize(HHC_32BIT_ENCODED_LENGTH);
    auto decoded = hhc_32bit_decode(encoded.c_str());
    EXPECT_EQ(decoded, 424242U);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsOnInvalidCharacters) {
    const std::string input = "1QLCP!";
    EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::invalid_argument);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsOnOutOfRange) {
    std::string input = HHC_32BIT_ENCODED_MAX_STRING;
    input.back() = '2';
    EXPECT_THROW(hhc_32bit_decode(input.c_str()), std::out_of_range);
}

TEST(HhcDecode32Test, Decode32BitSafeThrowsTooLongString) {
    std::string input = HHC_32BIT_ENCODED_MAX_STRING;
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
    EXPECT_EQ(output, 0);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput1) {
    auto output = hhc_32bit_decode(".");
    EXPECT_EQ(output, 1);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput6Characters0) {
    auto output = hhc_32bit_decode("------");
    EXPECT_EQ(output, 0);
}

TEST(HhcDecode32Test, Decode32BitSafeUnpaddedInput6Characters1) {
    auto output = hhc_32bit_decode("-----.");
    EXPECT_EQ(output, 1);
}

TEST(HhcDecode32Test, RoundTrip32BitTestFirst1Million) {
    for (uint32_t i = 0; i < 1000000; i++) {
        std::string output(HHC_32BIT_STRING_LENGTH, 0);
        hhc_32bit_encode_padded(i, output.data());
        auto decoded = hhc_32bit_decode_unsafe(output.data());
        ASSERT_EQ(decoded, i);
    }
}



