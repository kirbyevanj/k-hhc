#include <benchmark/benchmark.h>
#include "hhc.hpp"

// Benchmark for hhc_32bit_encode
static void BM_hhc32BitEncode(benchmark::State& state) {
    uint32_t input = rand();
    for (auto _ : state) {

        char output[9] = {0};
        hhc::hhc_32bit_encode(input, output);

        benchmark::DoNotOptimize(output);
        input *= 2;
    }
}
BENCHMARK(BM_hhc32BitEncode);

// Benchmark for hhc_32bit_decode
static void BM_hhc32BitDecode(benchmark::State& state) {
    //state.PauseTiming();
    // Create block of 1024 characters from hhc::HHC_ALPHABET
    std::string input;
    input.resize(2048);
    for (int i = 0; i < 2048; i++) {
        input[i] = hhc::HHC_ALPHABET[rand() % BASE];
    }
    const uint32_t index = rand() % (input.length() - 8);
    //state.ResumeTiming();
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
