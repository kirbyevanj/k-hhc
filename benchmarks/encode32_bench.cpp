#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"

#include <array>
#include <string>

/**
 * @file encode32_bench.cpp
 * @brief Benchmarks for 32-bit encoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;

/**
 * @brief Benchmark the padded 32-bit encoder which writes the full 6-character representation.
 */
void BM_hhc32BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint32_t, 2U << 16> inputs{};
    hhc::bench::fill_with_permuted_values(inputs, permuted32);

    std::array<char, hhc::HHC_32BIT_STRING_LENGTH> output{};
    std::size_t index = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc::hhc_32bit_encode_padded(inputs[++index & mask], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodePadded);

/**
 * @brief Benchmark the unpadded 32-bit encoder which post-processes leading padding.
 */
void BM_hhc32BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint32_t, 2U << 16> inputs{};
    hhc::bench::fill_with_permuted_values(inputs, permuted32);

    std::array<char, hhc::HHC_32BIT_STRING_LENGTH> output{};
    std::size_t index = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc::hhc_32bit_encode_unpadded(inputs[++index & mask], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodeUnpadded);

}  // namespace

