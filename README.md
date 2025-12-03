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

(Ryzen 9 5950X) at v1.0.8, clang 20.1.8:
```
Running ./benchmarks/hhc_benchmarks
Run on (32 X 5086.18 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x16)
  L1 Instruction 32 KiB (x16)
  L2 Unified 512 KiB (x16)
  L3 Unified 32768 KiB (x2)
Load Average: 2.13, 1.53, 1.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
------------------------------------------------------------------------
Benchmark                              Time             CPU   Iterations
------------------------------------------------------------------------
BM_hhc32BitEncodePadded             3.64 ns         3.63 ns    191195224
BM_hhc32BitEncodeUnpadded           10.7 ns         10.7 ns     65552301
BM_hhc64BitEncodePadded             7.55 ns         7.52 ns     93689754
BM_hhc64BitEncodeUnpadded           13.8 ns         13.8 ns     51108440
BM_hhc32BitDecodeUnsafe             2.26 ns         2.25 ns    298530751
BM_hhc32BitDecodeSafe               5.03 ns         5.01 ns    100000000
BM_hhc64BitDecodeUnsafe             4.86 ns         4.85 ns    179958209
BM_hhc64BitDecodeSafePadded         8.32 ns         8.29 ns     83133464
BM_hhc64BitDecodeSafeUnpadded       9.25 ns         9.22 ns     75757135
BM_hhcValidateString32              2.82 ns         2.81 ns    260687294
BM_hhcValidateString64              6.96 ns         6.94 ns    101104402
HM_rand32Bit                        4.77 ns         4.76 ns    146694025
BM_Permuted32Next                  0.701 ns        0.699 ns    997647405
```

As you can see, the performance is not quite there yet, additionally segmentation between the different performance cases needs to be done, such as unalligned access and aligned access, or differing length of strings.

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

# Run the benchmarks
./benchmarks/hhc_benchmarks

# Run the examples
./examples/hhc_encode_example
./examples/hhc_decode_example
```

## Building the Project

### Prerequisites

- CMake 3.20 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 19.14+ / VS 2017 15.7+)
- Python 3.6+ and pybind11 (optional, for Python bindings)

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

#### Build with Python Bindings

```bash
# Clone the repository
...
# Build with Python bindings
cmake -DHHC_BUILD_PYTHON=ON ..
cmake --build .
...

# Goto python directory
cd ../python

# Build the Python package
pip install .

# Test the module
PYTHONPATH=./python python3 -c "import k_hhc; print(k_hhc.encode_padded_32bit(629717763))"
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

### CMake Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `HHC_ENABLE_COVERAGE` | `OFF` | Enable LLVM code coverage instrumentation. Requires Clang compiler. Adds a `coverage` target that generates HTML reports. |
| `HHC_BUILD_PYTHON` | `OFF` | Build Python bindings using pybind11. Requires Python 3.6+ and pybind11. |
| `HHC_ENABLE_FUZZING` | `OFF` | Build libFuzzer targets for fuzzing. Requires Clang compiler with fuzzing support. |
| `CMAKE_BUILD_TYPE` | `Release` | Build type: `Debug`, `Release`, `RelWithDebInfo`, or `MinSizeRel`. |
| `CMAKE_C_COMPILER` | (system default) | C compiler to use (e.g., `clang`, `gcc`). |
| `CMAKE_CXX_COMPILER` | (system default) | C++ compiler to use (e.g., `clang++`, `g++`). |

## Running Benchmarks

```bash
# From the build directory
./benchmarks/hhc_benchmarks
```

## API Reference
- [C++ API Reference (Doxygen)](https://kirbyevanj.github.io/k-hhc/)

## External Dependencies

- [GoogleTest](https://github.com/google/googletest) - v1.14.0 (automatically downloaded)
- [Google Benchmark](https://github.com/google/benchmark) - v1.8.3 (automatically downloaded)

Dependencies are managed via CMake's ExternalProject_Add and will be automatically downloaded and built.

## Python Bindings

### Building from Source with CMake

```bash
mkdir build && cd build
cmake -DHHC_BUILD_PYTHON=ON ..
cmake --build .

# Test the module
PYTHONPATH=./python python3 -c "import k_hhc; print(k_hhc.encode_padded_32bit(42))"
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

