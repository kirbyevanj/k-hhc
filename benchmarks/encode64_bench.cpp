#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"

#include <array>

/**
 * @file encode64_bench.cpp
 * @brief Benchmarks for 64-bit encoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;
using hhc::bench::next_u64;
using hhc::bench::fill_with_permuted_values;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;

using std::array;
using benchmark::DoNotOptimize;

/**
 * @brief Benchmark the padded 64-bit encoder that preserves leading padding.
 */
void BM_hhc64BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, 2U << 16> inputs{};
    for (auto& input : inputs) {
        input = next_u64(permuted32);
    }

    array<char, HHC_64BIT_STRING_LENGTH> output{};
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc_64bit_encode_padded(inputs[++idx & mask], output.data());
        DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodePadded);

/**
 * @brief Benchmark the unpadded 64-bit encoder that strips leading padding characters.
 */
void BM_hhc64BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, 2U << 16> inputs{};
    for (auto& input : inputs) {
        input = next_u64(permuted32);
    }

    array<char, HHC_64BIT_STRING_LENGTH> output{};
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc_64bit_encode_unpadded(inputs[++idx & mask], output.data());
        DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodeUnpadded);

}  // namespace

