#include <benchmark/benchmark.h>
#include "hcc.hpp"

// Benchmark for hcc_encode
static void BM_HccEncode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_encode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_HccEncode);

// Benchmark for hcc_decode
static void BM_HccDecode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_decode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_HccDecode);

// Benchmark for hcc_32bit_encode
static void BM_Hcc32BitEncode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_32bit_encode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Hcc32BitEncode);

// Benchmark for hcc_32bit_decode
static void BM_Hcc32BitDecode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_32bit_decode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Hcc32BitDecode);

// Benchmark for hcc_64bit_encode
static void BM_Hcc64BitEncode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_64bit_encode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Hcc64BitEncode);

// Benchmark for hcc_64bit_decode
static void BM_Hcc64BitDecode(benchmark::State& state) {
    for (auto _ : state) {
        hcc::hcc_64bit_decode();
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Hcc64BitDecode);

BENCHMARK_MAIN();
