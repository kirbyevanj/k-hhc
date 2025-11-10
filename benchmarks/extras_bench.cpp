#include <benchmark/benchmark.h>

#include "bench_utils.hpp"

#include <cstdint>

/**
 * @file extras_bench.cpp
 * @brief Additional benchmarks that provide context for the HHC routines.
 */

namespace {

/**
 * @brief Baseline benchmark for the C rand() implementation.
 */
void HM_rand32Bit(benchmark::State& state) {
    for (auto _ : state) {
        uint32_t value = rand();
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(HM_rand32Bit);

/**
 * @brief Benchmark the Permuted32 helper used to generate deterministic pseudo-random values.
 */
void BM_Permuted32Next(benchmark::State& state) {
    hhc::bench::Permuted32 permuted32(rand());
    for (auto _ : state) {
        uint32_t value = permuted32.next();
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(BM_Permuted32Next);

}  // namespace

