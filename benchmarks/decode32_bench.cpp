#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"
#include "hcc_constants.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>

/**
 * @file decode32_bench.cpp
 * @brief Benchmarks for 32-bit decoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;

/**
 * @brief Benchmark the unchecked 32-bit decoder with random alphabet characters.
 */
void BM_hhc32BitDecodeUnsafe(benchmark::State& state) {
    hhc::bench::Permuted32 permuted32(rand());
    constexpr std::size_t kPoolSize = 1U << 16;
    constexpr std::size_t kMask = kPoolSize - 1;

    std::array<std::array<char, hhc::HHC_32BIT_STRING_LENGTH + 1>, kPoolSize> inputs{};
    for (auto& entry : inputs) {
        for (std::size_t i = 0; i < hhc::HHC_32BIT_STRING_LENGTH; ++i) {
            entry[i] = hhc::bench::random_alphabet_char(permuted32);
        }
        entry[hhc::HHC_32BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & kMask];
        uint32_t output = hhc::hhc_32bit_decode_unsafe(current.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitDecodeUnsafe);

/**
 * @brief Benchmark the safe 32-bit decoder using pre-encoded values.
 */
void BM_hhc32BitDecodeSafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint32_t, hhc::bench::kPermutationBlockSize> values{};
    hhc::bench::fill_with_permuted_values(values, permuted32);

    std::array<std::array<char, hhc::HHC_32BIT_STRING_LENGTH>, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        hhc::hhc_32bit_encode_padded(values[i], inputs[i].data());
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        uint32_t output = hhc::hhc_32bit_decode(current.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitDecodeSafe);

}  // namespace

