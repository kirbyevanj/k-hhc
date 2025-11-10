#include <benchmark/benchmark.h>

#include "bench_utils.hpp"
#include "hhc.hpp"
#include "hhc_constants.hpp"

#include <array>
#include <string>

/**
 * @file validate_bench.cpp
 * @brief Benchmarks for the string validation helpers.
 */

namespace {

using hhc::bench::Permuted32;

template <std::size_t N>
std::array<std::string, hhc::bench::kPermutationBlockSize> generate_random_strings() {
    Permuted32 permuted32(rand());
    std::array<std::string, hhc::bench::kPermutationBlockSize> inputs{};
    for (auto& str : inputs) {
        str.resize(N);
        for (auto& ch : str) {
            ch = hhc::ALPHABET[permuted32.next() % hhc::BASE];
        }
    }
    return inputs;
}

/**
 * @brief Benchmark validation of 6-character strings.
 */
void BM_hhcValidateString32(benchmark::State& state) {
    auto inputs = generate_random_strings<hhc::HHC_32BIT_STRING_LENGTH>();
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        bool result = hhc::hhc_validate_string(current.c_str());
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_hhcValidateString32);

/**
 * @brief Benchmark validation of 11-character strings.
 */
void BM_hhcValidateString64(benchmark::State& state) {
    auto inputs = generate_random_strings<hhc::HHC_64BIT_STRING_LENGTH>();
    std::size_t idx = 0;
    const std::size_t mask = inputs.size() - 1;

    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        bool result = hhc::hhc_validate_string(current.c_str());
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_hhcValidateString64);

}  // namespace

