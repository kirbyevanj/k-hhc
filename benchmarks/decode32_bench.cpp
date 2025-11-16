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
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::hhc_32bit_decode_unsafe;
using hhc::hhc_32bit_decode;
using hhc::hhc_32bit_encode_padded;
using hhc::hhc_32bit_encode_unpadded;

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
        const int len = state.range(0);
        entry.resize(len);
        for (int i = 0; i < len; ++i) {
            entry[i] = random_alphabet_char(permuted32);
        }
        entry[len-1] = '\0';
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_32bit_decode_unsafe(current.data()));
    }
}
BENCHMARK(BM_hhc32BitDecodeUnsafe)->Range(HHC_32BIT_ENCODED_LENGTH, HHC_32BIT_ENCODED_LENGTH);

/**
 * @brief Benchmark the safe 32-bit decoder using pre-encoded values.
 */
void BM_hhc32BitDecodeSafePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, PERMUTATION_BLOCKSIZE> values{};
    fill_with_permuted_values(values, permuted32);

    array<string, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        const int len = state.range(0);
        inputs[i].resize(len);
        for (int j = 0; j < len; ++j) {
            inputs[i][j] = random_alphabet_char(permuted32);
        }
        hhc_32bit_encode_padded(values[i], inputs[i].data());
        inputs[i][len-1] = '\0';
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_32bit_decode(current.data()));
    }
}
BENCHMARK(BM_hhc32BitDecodeSafePadded)->DenseRange(2, HHC_32BIT_ENCODED_LENGTH+1);

void BM_hhc32BitDecodeSafeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, PERMUTATION_BLOCKSIZE> values{};
    fill_with_permuted_values(values, permuted32);

    array<string, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        const int len = state.range(0);
        inputs[i] = string(len, 0);
        hhc_32bit_encode_unpadded(values[i], inputs[i].data());
        inputs[i][len-1] = '\0';
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_32bit_decode(current.data()));
    }
}
BENCHMARK(BM_hhc32BitDecodeSafeUnpadded)->DenseRange(2, HHC_32BIT_ENCODED_LENGTH+1);

}  // namespace
