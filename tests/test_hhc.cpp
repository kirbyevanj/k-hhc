#include <gtest/gtest.h>
#include "hhc.hpp"
#include <string>
#include <limits>
#include <cstdint>

constexpr auto U32_MAX = std::numeric_limits<uint32_t>::max();
constexpr auto U32_MIN = std::numeric_limits<uint32_t>::min();
constexpr size_t HHC_31BIT_ENCODED_LENGTH = 8;
using hhc::hhc_32bit_encode;
using hhc::hhc_32bit_decode;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using std::string;

// Test for hhc_32bit_encode
TEST(HhcTest, Encode32BitTestUINT32_MIN) {
    string output(HHC_32BIT_ENCODED_LENGTH+1, 0);
    hhc_32bit_encode(U32_MIN, output.data());
    EXPECT_STREQ(output.c_str(), "--------");
}

TEST(HhcTest, Encode32BitTestUINT32_MAX) {
    string output(HHC_32BIT_ENCODED_LENGTH+1, 0);
    hhc_32bit_encode(U32_MAX, output.data());
    EXPECT_STREQ(output.c_str(), "--1QLCp1");
}

// Test for hhc_32bit_decode
TEST(HhcTest, Decode32BitTestUINT32_MIN) {
    const string input = "--------";
    auto output = hhc_32bit_decode(input.data());
    EXPECT_EQ(output, U32_MIN);
}

TEST(HhcTest, Decode32BitTestUINT32_MAX) {
    const string input = "--1QLCp1";
    auto output = hhc_32bit_decode(input.data());
    EXPECT_EQ(output, U32_MAX);
}

TEST(HhcTest, RoundTrip32BitTestFirst1Million) {
    for (uint32_t i = 0; i < 1000000; i++) {
        string output(HHC_32BIT_ENCODED_LENGTH+1, 0);
        hhc_32bit_encode(i, output.data());
        auto decoded = hhc_32bit_decode(output.data());
        EXPECT_EQ(decoded, i);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
