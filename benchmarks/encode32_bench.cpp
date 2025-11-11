#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"

#include <array>

/**
 * @file encode32_bench.cpp
 * @brief Benchmarks for 32-bit encoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;
using hhc::bench::fill_with_permuted_values;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::hhc_32bit_encode_padded;
using hhc::hhc_32bit_encode_unpadded;

using std::array;
using benchmark::DoNotOptimize;

/**
 * @brief Benchmark the padded 32-bit encoder which writes the full 6-character representation.
 */
void BM_hhc32BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, 2U << 16> inputs{};
    fill_with_permuted_values(inputs, permuted32);

    array<char, HHC_32BIT_STRING_LENGTH> output{};
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc_32bit_encode_padded(inputs[++idx & mask], output.data());
        DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodePadded);

/**
 * @brief Benchmark the unpadded 32-bit encoder which post-processes leading padding.
 */
void BM_hhc32BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, 2U << 16> inputs{};
    fill_with_permuted_values(inputs, permuted32);

    array<char, HHC_32BIT_STRING_LENGTH> output{};
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc_32bit_encode_unpadded(inputs[++idx & mask], output.data());
        DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodeUnpadded);

}  // namespace

