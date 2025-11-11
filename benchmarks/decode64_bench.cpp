#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>

/**
 * @file decode64_bench.cpp
 * @brief Benchmarks for 64-bit decoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;
using hhc::bench::next_u64;
using hhc::bench::random_alphabet_char;
using hhc::bench::PERMUTATION_BLOCKSIZE;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::hhc_64bit_decode_unsafe;
using hhc::hhc_64bit_decode;
using hhc::hhc_64bit_encode_padded;
using hhc::hhc_64bit_encode_unpadded;

using std::array;
using std::string;
using benchmark::DoNotOptimize;

/**
 * @brief Benchmark the unchecked 64-bit decoder operating on random alphabet characters.
 */
void BM_hhc64BitDecodeUnsafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    constexpr std::size_t pool_size = 1U << 16;
    constexpr std::size_t mask = pool_size - 1;

    array<string, pool_size> inputs{};
    for (auto& entry : inputs) {
        entry.resize(HHC_64BIT_STRING_LENGTH);
        for (std::size_t i = 0; i < HHC_64BIT_STRING_LENGTH; ++i) {
            entry[i] = random_alphabet_char(permuted32);
        }
        entry[HHC_64BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_64bit_decode_unsafe(current.data()));
    }
}
BENCHMARK(BM_hhc64BitDecodeUnsafe);

/**
 * @brief Benchmark the safe 64-bit decoder with padded inputs.
 */
void BM_hhc64BitDecodeSafePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, PERMUTATION_BLOCKSIZE> values{};
    for (auto& value : values) {
        value = next_u64(permuted32);
    }

    array<string, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        inputs[i].resize(HHC_64BIT_STRING_LENGTH);
        hhc_64bit_encode_padded(values[i], inputs[i].data());
        inputs[i][HHC_64BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc::hhc_64bit_decode(current.data()));
    }
}
BENCHMARK(BM_hhc64BitDecodeSafePadded);

/**
 * @brief Benchmark the safe 64-bit decoder with unpadded inputs.
 */
void BM_hhc64BitDecodeSafeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, PERMUTATION_BLOCKSIZE> values{};
    for (auto& value : values) {
        value = next_u64(permuted32);
    }
    array<string, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        inputs[i] = string(HHC_64BIT_STRING_LENGTH, 0);
        hhc_64bit_encode_unpadded(values[i], inputs[i].data());
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        DoNotOptimize(hhc_64bit_decode(current.data()));
    }
}
BENCHMARK(BM_hhc64BitDecodeSafeUnpadded);

}  // namespace

