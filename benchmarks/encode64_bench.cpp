#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"

#include <array>
#include <string>

/**
 * @file encode64_bench.cpp
 * @brief Benchmarks for 64-bit encoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;

inline uint64_t next_u64(Permuted32& generator) {
    return (static_cast<uint64_t>(generator.next()) << 32) | generator.next();
}

/**
 * @brief Benchmark the padded 64-bit encoder that preserves leading padding.
 */
void BM_hhc64BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint64_t, 2U << 16> inputs{};
    for (auto& input : inputs) {
        input = next_u64(permuted32);
    }

    std::array<char, hhc::HHC_64BIT_STRING_LENGTH> output{};
    std::size_t index = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc::hhc_64bit_encode_padded(inputs[++index & mask], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodePadded);

/**
 * @brief Benchmark the unpadded 64-bit encoder that strips leading padding characters.
 */
void BM_hhc64BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint64_t, 2U << 16> inputs{};
    for (auto& input : inputs) {
        input = next_u64(permuted32);
    }

    std::array<char, hhc::HHC_64BIT_STRING_LENGTH> output{};
    std::size_t index = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        hhc::hhc_64bit_encode_unpadded(inputs[++index & mask], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodeUnpadded);

}  // namespace

