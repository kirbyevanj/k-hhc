#include <benchmark/benchmark.h>
#include "hhc.hpp"
#include "hcc_constants.hpp"
#include <cstdlib>
#include <cstdint>
#include <string>

using std::string;

// Benchmark for hhc_32bit_encode
static void BM_hhc32BitEncodePadded(benchmark::State& state) {
    uint32_t input = rand();
    for (auto _ : state) {
        string output(8, 0);
        hhc::hhc_32bit_encode_padded(input, output.data());

        benchmark::DoNotOptimize(output);
        input *= 2;
    }
}
BENCHMARK(BM_hhc32BitEncodePadded);


static void BM_hhc32BitEncodeUnpadded(benchmark::State& state) {
    uint32_t input = rand();
    for (auto _ : state) {
        string output(8, 0);
        hhc::hhc_32bit_encode_unpadded(input, output.data());
        benchmark::DoNotOptimize(output);
        input *= 2;
    }
}
BENCHMARK(BM_hhc32BitEncodeUnpadded);

// Benchmark for hhc_32bit_decode
static void BM_hhc32BitDecode(benchmark::State& state) {
    // Create block of 1024 characters from hhc::HHC_ALPHABET
    string input;
    input.resize(8);
    for (int i = 0; i < 8; i++) {
        input[i] = hhc::ALPHABET[rand() % BASE];
    }
    const uint32_t index = 0;
    for (auto _ : state) {

        uint32_t output = hhc::hhc_32bit_decode(input.c_str() + index);
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitDecode);


static void HM_rand32Bit(benchmark::State& state) {
    for (auto _ : state) {
        uint32_t input = rand();
        benchmark::DoNotOptimize(input);
    }
}
BENCHMARK(HM_rand32Bit);

BENCHMARK_MAIN();
