#pragma once

/**
 * @file bench_utils.hpp
 * @brief Common utilities shared across benchmarks.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "hhc_constants.hpp"

namespace hhc::bench {

/// Size of the pre-generated permutation blocks used by the benchmarks.
inline constexpr std::size_t PERMUTATION_BLOCKSIZE = 1U << 9;

/**
 * @brief Lightweight 32-bit permuted counter used to generate reproducible pseudo-random inputs.
 *
 * The implementation mirrors MurmurHash3's fmix32 finaliser which provides a cheap bijective
 * mapping across the 32-bit space. Incrementing the seed and mixing ensures that every 32-bit
 * value is visited exactly once before wrapping, making it ideal for micro-benchmarks that need
 * deterministic yet well-distributed data.
 */
struct Permuted32 {
    uint32_t s;

    explicit constexpr Permuted32(uint32_t seed = 0) : s(seed) {}

    static constexpr uint32_t mix(uint32_t x) {
        x ^= x >> 16;
        x *= 0x85ebca6bU;  // odd -> invertible mod 2^32
        x ^= x >> 13;
        x *= 0xc2b2ae35U;  // odd -> invertible mod 2^32
        x ^= x >> 16;
        return x;
    }

    constexpr uint32_t next() {
        return mix(s++);
    }
};

/**
 * @brief Fill an std::array with permuted values generated from the provided sequence.
 */
template <typename T, std::size_t N, typename Generator>
void fill_with_permuted_values(std::array<T, N>& out, Generator& generator) {
    for (auto& value : out) {
        value = static_cast<T>(generator.next());
    }
}

inline char random_alphabet_char(Permuted32& generator) {
    constexpr std::size_t mask = (1U << 7) - 1; // 127 covers BASE=66
    while (true) {
        const std::size_t candidate = generator.next() & mask;
        if (candidate < hhc::BASE) {
            return hhc::ALPHABET[candidate];
        }
    }
}

/**
 * @brief Generate the next 64-bit value from the permuted generator.
 */
inline uint64_t next_u64(Permuted32& generator) {
    return (static_cast<uint64_t>(generator.next()) << 32) | generator.next();
}

}  // namespace hhc::bench