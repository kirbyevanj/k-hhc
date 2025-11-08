#include <benchmark/benchmark.h>
#include "hhc.hpp"
#include "hcc_constants.hpp"
#include <cstdlib>
#include <cstdint>
#include <string>
#include <array>

using std::array;

struct Permuted32 {
  uint32_t s;
  explicit Permuted32(uint32_t seed = 0) : s(seed) {}

  // MurmurHash3 fmix32-style bijective finalizer
  static constexpr uint32_t mix(uint32_t x) {
    x ^= x >> 16;
    x *= 0x85ebca6bU;     // odd -> invertible mod 2^32
    x ^= x >> 13;
    x *= 0xc2b2ae35U;     // odd -> invertible mod 2^32
    x ^= x >> 16;
    return x;
  }

  constexpr uint32_t next() {
    return mix(s++);      // visits each 32-bit value once before wrap
  }
};

using std::string;
using hhc::HHC_32BIT_ENCODED_LENGTH;
using hhc::HHC_64BIT_ENCODED_LENGTH;
using hhc::HHC_32BIT_STRING_LENGTH;
using hhc::HHC_64BIT_STRING_LENGTH;
using hhc::BASE;

// Benchmark for hhc_32bit_encode
static void BM_hhc32BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, 2 << 16> inputs;
    for (auto& input : inputs) {
        input = permuted32.next();
    }

    uint32_t i = 0;
    for (auto _ : state) {
        string output(8, 0);
        hhc::hhc_32bit_encode_padded(inputs[++i & (inputs.size() - 1)], output.data());

        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodePadded);


static void BM_hhc32BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint32_t, 2 << 16> inputs;
    for (auto& input : inputs) {
        input = permuted32.next();
    }

    uint32_t i = 0;
    for (auto _ : state) {
        string output(8, 0);
        hhc::hhc_32bit_encode_unpadded(inputs[++i & (inputs.size() - 1)], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitEncodeUnpadded);

// Benchmark for hhc_32bit_decode
static void BM_hhc32BitDecode(benchmark::State& state) {
    // Create block of characters from hhc::ALPHABET
    string input;
    input.resize(6);
    for (int i = 0; i < 6; i++) {
        input[i] = hhc::ALPHABET[rand() % BASE];
    }

    const uint32_t index = 0;
    for (auto _ : state) {

        uint32_t output = hhc::hhc_32bit_decode(input.c_str() + index);
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc32BitDecode);


static void BM_hhc64BitEncodePadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, 2 << 16> inputs;
    for (auto& input : inputs) {
        input = permuted32.next();
    }

    uint32_t i = 0;
    for (auto _ : state) {
        string output(HHC_64BIT_ENCODED_LENGTH+2, 0);
        hhc::hhc_64bit_encode_padded(inputs[++i & (inputs.size() - 1)], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodePadded);


static void BM_hhc64BitEncodeUnpadded(benchmark::State& state) {
    Permuted32 permuted32(rand());
    array<uint64_t, 2 << 16> inputs;
    for (auto& input : inputs) {
        input = permuted32.next();
    }

    uint32_t i = 0;
    for (auto _ : state) {
        string output(HHC_64BIT_ENCODED_LENGTH+2, 0);
        hhc::hhc_64bit_encode_unpadded(inputs[++i & (inputs.size() - 1)], output.data());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitEncodeUnpadded);

static void BM_hhc64BitDecode(benchmark::State& state) {
    string input(HHC_64BIT_ENCODED_LENGTH+2, 0);
    for (size_t i = 0; i < input.size(); i++) {
        input[i] = hhc::ALPHABET[rand() % BASE];
    }

    const uint32_t index = 0;
    for (auto _ : state) {
        uint64_t output = hhc::hhc_64bit_decode(input.c_str() + index);
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhc64BitDecode);


static void BM_hhcValidateString32(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<string, 1U << 9> inputs{};
    for (auto& str : inputs) {
        str.resize(HHC_32BIT_STRING_LENGTH);
        for (auto& ch : str) {
            ch = hhc::ALPHABET[permuted32.next() % BASE];
        }
    }
    
    size_t idx = 0;
    const size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        bool output = hhc::hhc_validate_string(current.c_str());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhcValidateString32);

static void BM_hhcValidateString64(benchmark::State& state) {
    Permuted32 permuted32(rand());
    std::array<string, 1U << 9> inputs{};
    for (auto& str : inputs) {
        str.resize(HHC_64BIT_STRING_LENGTH);
        for (auto& ch : str) {
            ch = hhc::ALPHABET[permuted32.next() % BASE];
        }
    }

    size_t idx = 0;
    const size_t mask = inputs.size() - 1;
    for (auto _ : state) {
        const auto& current = inputs[idx++ & mask];
        bool output = hhc::hhc_validate_string(current.c_str());
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_hhcValidateString64);

static void HM_rand32Bit(benchmark::State& state) {
    for (auto _ : state) {
        uint32_t input = rand();
        benchmark::DoNotOptimize(input);
    }
}
BENCHMARK(HM_rand32Bit);

static void BM_Permuted32Next(benchmark::State& state) {
    Permuted32 permuted32(rand());
    for (auto _ : state) {
        uint32_t output = permuted32.next();
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(BM_Permuted32Next);

BENCHMARK_MAIN();
