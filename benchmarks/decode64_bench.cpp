#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"
#include "hcc_constants.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>

/**
 * @file decode64_bench.cpp
 * @brief Benchmarks for 64-bit decoding routines in the HHC library.
 */

namespace {

using hhc::bench::Permuted32;

inline uint64_t next_u64(Permuted32& generator) {
    return (static_cast<uint64_t>(generator.next()) << 32) | generator.next();
}

/**
 * @brief Benchmark the unchecked 64-bit decoder operating on random alphabet characters.
 */
void BM_hhc64BitDecodeUnsafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    constexpr std::size_t kPoolSize = 1U << 16;
    constexpr std::size_t kMask = kPoolSize - 1;

    std::array<std::array<char, hhc::HHC_64BIT_STRING_LENGTH + 1>, kPoolSize> inputs{};
    for (auto& entry : inputs) {
        for (std::size_t i = 0; i < hhc::HHC_64BIT_STRING_LENGTH; ++i) {
            entry[i] = hhc::bench::random_alphabet_char(permuted32);
        }
        entry[hhc::HHC_64BIT_STRING_LENGTH] = '\0';
    }

    std::size_t idx = 0;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & kMask];
        uint64_t output = hhc::hhc_64bit_decode_unsafe(current.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitDecodeUnsafe);

/**
 * @brief Benchmark the safe 64-bit decoder with pre-encoded inputs.
 */
void BM_hhc64BitDecodeSafe(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<uint64_t, hhc::bench::kPermutationBlockSize> values{};
    for (auto& value : values) {
        value = next_u64(permuted32);
    }

    std::array<std::array<char, hhc::HHC_64BIT_STRING_LENGTH>, values.size()> inputs{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        hhc::hhc_64bit_encode_padded(values[i], inputs[i].data());
    }

    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        uint64_t output = hhc::hhc_64bit_decode(current.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitDecodeSafe);

}  // namespace

