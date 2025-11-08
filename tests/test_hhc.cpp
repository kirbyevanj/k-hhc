#include <gtest/gtest.h>
#include "hhc.hpp"
#include <string>
#include <limits>
#include <cstdint>

constexpr auto U32_MAX  = std::numeric_limits<uint32_t>::max();
constexpr auto U32_MIN  = std::numeric_limits<uint32_t>::min();
constexpr auto U64_MAX = std::numeric_limits<uint64_t>::max();
constexpr auto U64_MIN = std::numeric_limits<uint64_t>::min();
using hhc::hhc_32bit_encode_padded;
using hhc::hhc_32bit_decode;
using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;
using hhc::hhc_64bit_decode;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;
using std::string;

// Test for hhc_32bit_encode
TEST(HhcTest, Encode32BitTestUINT32_MIN) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(U32_MIN, output.data());
    EXPECT_STREQ(output.c_str(), "------");
}

TEST(HhcTest, Encode32BitTestUINT32_MAX) {
    string output(HHC_32BIT_STRING_LENGTH, '\0');
    hhc_32bit_encode_padded(U32_MAX, output.data());
    EXPECT_STREQ(output.c_str(), "1QLCp1");
}

// Test for hhc_32bit_decode
TEST(HhcTest, Decode32BitTestUINT32_MIN) {
    const string input = "------\0\0";
    auto output = hhc_32bit_decode(input.data());
    EXPECT_EQ(output, U32_MIN);
}

TEST(HhcTest, Decode32BitTestUINT32_MAX) {
    const string input = "1QLCp1\0\0";
    auto output = hhc_32bit_decode(input.data());
    EXPECT_EQ(output, U32_MAX);
}

TEST(HhcTest, RoundTrip32BitTestFirst1Million) {
    for (uint32_t i = 0; i < 1000000; i++) {
        string output(HHC_32BIT_STRING_LENGTH, 0);
        hhc_32bit_encode_padded(i, output.data());
        auto decoded = hhc_32bit_decode(output.data());
        ASSERT_EQ(decoded, i);
    }
}

TEST(HhcTest, Encode64BitTestUINT64_MIN) {
    string output(HHC_64BIT_ENCODED_LENGTH+2, '\0');
    hhc_64bit_encode_padded(U64_MIN, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), string(HHC_64BIT_ENCODED_LENGTH, '-'));
}

TEST(HhcTest, Encode64BitTestUINT64_MAX) {
    string output(HHC_64BIT_STRING_LENGTH, '\0');
    hhc_64bit_encode_padded(U64_MAX, output.data());
    EXPECT_EQ(output.substr(0, HHC_64BIT_ENCODED_LENGTH), "9lH9ebONzYD");
}

TEST(HhcTest, Decode64BitTestUINT64_MIN) {
    string input(HHC_64BIT_STRING_LENGTH, '-');
    auto output = hhc_64bit_decode(input.c_str());
    EXPECT_EQ(output, U64_MIN);
}

TEST(HhcTest, Decode64BitTestUINT64_MAX) {
    const string input = "9lH9ebONzYD";
    auto output = hhc_64bit_decode(input.c_str());
    EXPECT_EQ(output, U64_MAX);
}

TEST(HhcTest, RoundTrip64BitTestFirst1Million) {
    for (uint64_t i = 0; i < 1000000; i++) {
        string output(HHC_64BIT_ENCODED_LENGTH+1, 0);
        hhc_64bit_encode_padded(i, output.data());
        auto decoded = hhc_64bit_decode(output.data());
        ASSERT_EQ(decoded, i);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
