#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>

/**
 * @file decode32_bench.cpp
 * @brief Benchmarks for 32-bit decoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;
using hhc::bench::random_alphabet_char;
using hhc::bench::fill_with_permuted_values;
using hhc::bench::PERMUTATION_BLOCKSIZE;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::hhc_32bit_decode_unsafe;
using hhc::hhc_32bit_decode;
using hhc::hhc_32bit_encode_padded;

using std::array;
using std::string;
using benchmark::DoNotOptimize;

/**
 * @brief Benchmark the unchecked 32-bit decoder with random alphabet characters.
 */
void BM_hhc32BitDecodeUnsafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    constexpr std::size_t pool_size = 1U << 16;
    constexpr std::size_t mask = pool_size - 1;

    array<string, pool_size> inputs{};
    for (auto& entry : inputs) {
        entry.resize(HHC_32BIT_STRING_LENGTH);
        for (std::size_t i = 0; i < HHC_32BIT_STRING_LENGTH; ++i) {
            entry[i] = random_alphabet_char(permuted32);
        }
        entry[HHC_32BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_32bit_decode_unsafe(current.data()));
    }
}
BENCHMARK(BM_hhc32BitDecodeUnsafe);

/**
 * @brief Benchmark the safe 32-bit decoder using pre-encoded values.
 */
void BM_hhc32BitDecodeSafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, PERMUTATION_BLOCKSIZE> values{};
    fill_with_permuted_values(values, permuted32);

    array<string, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        inputs[i].resize(HHC_32BIT_STRING_LENGTH);
        hhc_32bit_encode_padded(values[i], inputs[i].data());
        inputs[i][HHC_32BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_32bit_decode(current.data()));
    }
}
BENCHMARK(BM_hhc32BitDecodeSafe);

}  // namespace

