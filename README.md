# k-hhc

[![CI](https://github.com/kirbyevanj/k-hhc/actions/workflows/ci.yml/badge.svg)](https://github.com/kirbyevanj/k-hhc/actions/workflows/ci.yml)
[![Fuzzing](https://github.com/kirbyevanj/k-hhc/actions/workflows/fuzzing.yml/badge.svg)](https://github.com/kirbyevanj/k-hhc/actions/workflows/fuzzing.yml)
[![codecov](https://codecov.io/github/kirbyevanj/k-hhc/graph/badge.svg?token=YT9H76C6MC)](https://codecov.io/github/kirbyevanj/k-hhc)
[![PyPI](https://img.shields.io/pypi/v/k-hhc.svg)](https://pypi.org/project/k-hhc/)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

A library for encoding/decoding HHC(hexahexacontadecimal) for 32bit and 64bit integers, with C++ and Python bindings.

Inspired by [hexahexacontadecimal](https://pypi.org/project/hexahexacontadecimal/2.2.1/) by [Alexander Ljungberg](https://github.com/aljungberg)


This project is intended to be an exploration in creating performant, secure, and portable algorithms, while also being a learning experience for me. Take it with a grain of salt as this project moves to full production.

## Why?

The base66 alphabet is comprised of all the URL unreserved characters ([see: RFC 3986](https://datatracker.ietf.org/doc/html/rfc3986)), 
This is useful for creating short, human-readable URLs for things like short links, tracking codes, or other identifiers.

If encoding information in a URL is important, base66 outperforms base64 in information density. However, base66 is much slower to encode and decode, this project aims to provide optimized implementations that attempt to compete with base64 in terms of speed for integer sized operations.

In theory, base66 will always underperform when compared against base64 as the naive algorithm in O(n^2) time complexity, and inherently requires many modulo operations.

Procedural approaches are currently being explored, but are not yet implemented.

### Dirty Benchmark

(Ryzen 9 5950X) at v1.0.9, clang 20.1.8. `SafePadded` decodes full-length strings;
`SafeUnpadded/N` decodes valid strings of length N (exercising the internal re-padding path):
```
Running ./benchmarks/hhc_benchmarks
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
BM_hhc32BitEncodePadded                3.66 ns         3.64 ns    192808111
BM_hhc32BitEncodeUnpadded              7.29 ns         7.25 ns     96727892
BM_hhc64BitEncodePadded                7.38 ns         7.34 ns     95334676
BM_hhc64BitEncodeUnpadded              11.2 ns         11.1 ns     62800279
BM_hhc32BitDecodeUnsafe                1.97 ns         1.95 ns    357813255
BM_hhc32BitDecodeSafePadded            5.18 ns         5.15 ns    137772050
BM_hhc32BitDecodeSafeUnpadded/2        7.48 ns         7.44 ns     94386643
BM_hhc32BitDecodeSafeUnpadded/6        5.21 ns         5.18 ns    135880173
BM_hhc64BitDecodeUnsafe                3.61 ns         3.59 ns    196041287
BM_hhc64BitDecodeSafePadded            7.94 ns         7.90 ns     89124368
BM_hhc64BitDecodeSafeUnpadded/2        7.74 ns         7.69 ns     91107737
BM_hhc64BitDecodeSafeUnpadded/10       11.0 ns         11.0 ns     63744347
BM_hhc64BitDecodeSafeUnpadded/11       7.88 ns         7.85 ns     89452653
BM_hhcValidateString32                 2.96 ns         2.95 ns    236655903
BM_hhcValidateString64                 6.03 ns         6.01 ns    116794736
HM_rand32Bit                           4.34 ns         4.31 ns    165584729
BM_Permuted32Next                     0.693 ns        0.690 ns   1014214212
```

## Quick Start

### Python

```bash
# Install from PyPI
pip install k-hhc

# Verify
python -c "import k_hhc; print(k_hhc.encode_padded_32bit(629717763))"
```

### C++

```bash
# Clone the repository
git clone https://github.com/kirbyevanj/k-hhc.git
cd k-hhc

# Basic build: produces library, tests, and examples
mkdir build && cd build
cmake ..
cmake --build .

# Run the tests
ctest --output-on-failure

# Run the examples
./examples/hhc_encode_example
./examples/hhc_decode_example
./examples/hhc_assert_example
```

## Building the Project

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 19.14+ / VS 2017 15.7+)
- Python 3.7+ and pybind11 (optional, build-time only, for Python bindings)

### Build Instructions

#### Basic Build

```bash
# Clone the repository
git clone https://github.com/kirbyevanj/k-hhc.git
cd k-hhc

# Create build directory
mkdir build && cd build

# Configure the project (defaults to Release build)
cmake ..

# Or specify build type explicitly
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..

# Build all targets
cmake --build .
```

#### Build the Python Bindings

The Python extension is built with setuptools, not CMake:

```bash
cd python

# Build and install the Python package
pip install .

# Test the module
python3 -c "import k_hhc; print(k_hhc.encode_padded_32bit(629717763))"
```

#### Build with Code Coverage

```bash
# Configure with coverage enabled (requires Clang)
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DHHC_ENABLE_COVERAGE=ON \
      ..

# Build the project
cmake --build .

# Run tests to generate coverage data
ctest --output-on-failure

# Generate coverage report
cmake --build . --target coverage

# View the HTML report
xdg-open coverage/html/index.html  # Linux
# or
open coverage/html/index.html      # macOS
```

#### Build with Fuzzing

```bash
# Configure with fuzzing enabled (requires Clang)
mkdir build && cd build
cmake -DHHC_ENABLE_FUZZING=ON \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      ..

# Build fuzzing targets
cmake --build .

# Run a fuzzer
./fuzz/hhc_fuzz_decode32 -max_total_time=60
```

### Release build behavior (`HHC_ASSERT`)

In **Debug** builds, failed internal assertions (`HHC_ASSERT` in `hhc_assert.hpp`) abort
with a diagnostic message. In **Release** builds (`NDEBUG` defined), the same check
compiles to `__builtin_trap()` and terminates the process immediately without a message.
Release Python wheels follow this behavior by design (fail-fast on internal invariant
violations rather than continuing in a broken state).

### CMake Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `HHC_BUILD_TESTS` | `ON` | Build unit tests (downloads GoogleTest). |
| `HHC_BUILD_EXAMPLES` | `ON` | Build the example programs. |
| `HHC_BUILD_BENCHMARKS` | `OFF` | Build benchmarks (downloads Google Benchmark). |
| `HHC_ENABLE_COVERAGE` | `OFF` | Enable LLVM code coverage instrumentation. Requires Clang compiler. Adds a `coverage` target that generates HTML reports. |
| `HHC_ENABLE_FUZZING` | `OFF` | Build libFuzzer targets for fuzzing. Requires Clang compiler with fuzzing support. |
| `CMAKE_BUILD_TYPE` | `Release` | Build type: `Debug`, `Release`, `RelWithDebInfo`, or `MinSizeRel`. |
| `CMAKE_C_COMPILER` | (system default) | C compiler to use (e.g., `clang`, `gcc`). |
| `CMAKE_CXX_COMPILER` | (system default) | C++ compiler to use (e.g., `clang++`, `g++`). |

### Installing and consuming with CMake

```bash
cmake --install build --prefix /your/prefix
```

```cmake
find_package(k-hhc 1.0 REQUIRED)
target_link_libraries(your_target PRIVATE k-hhc::k-hhc)
```

A pkg-config file (`k-hhc.pc`) is also installed.

## Running Benchmarks

```bash
# Configure with benchmarks enabled, then run from the build directory
cmake -DHHC_BUILD_BENCHMARKS=ON ..
cmake --build .
./benchmarks/hhc_benchmarks
```

## API Reference
- [C++ API Reference (Doxygen)](https://kirbyevanj.github.io/k-hhc/)

## External Dependencies

- [GoogleTest](https://github.com/google/googletest) - v1.14.0 (automatically downloaded)
- [Google Benchmark](https://github.com/google/benchmark) - v1.9.1 (automatically downloaded)

Dependencies are managed via CMake's ExternalProject_Add and will be automatically downloaded and built.

## Python Bindings

### Building from Source

```bash
cd python
pip install .

# Test the module
python3 -c "import k_hhc; print(k_hhc.encode_padded_32bit(42))"
```

### Python API

```python
import k_hhc

# 32-bit operations
encoded = k_hhc.encode_padded_32bit(424242)   # Returns: "--.TNv"
encoded = k_hhc.encode_unpadded_32bit(424242) # Returns: ".TNv"
decoded = k_hhc.decode_32bit(".TNv")          # Returns: 424242

# 64-bit operations
encoded = k_hhc.encode_padded_64bit(9876543210)   # Returns: "-----5tVfK4"
encoded = k_hhc.encode_unpadded_64bit(9876543210) # Returns: "5tVfK4"
decoded = k_hhc.decode_64bit("5tVfK4")            # Returns: 9876543210
```

See `python/examples/` for more detailed examples.

