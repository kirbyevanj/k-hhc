# k-hhc

[![CI](https://github.com/kirbyevanj/k-hhc/actions/workflows/ci.yml/badge.svg)](https://github.com/kirbyevanj/k-hhc/actions/workflows/ci.yml)
[![Python Package](https://github.com/kirbyevanj/k-hhc/actions/workflows/python.yml/badge.svg)](https://github.com/kirbyevanj/k-hhc/actions/workflows/python.yml)
[![Fuzzing](https://github.com/kirbyevanj/k-hhc/actions/workflows/fuzzing.yml/badge.svg)](https://github.com/kirbyevanj/k-hhc/actions/workflows/fuzzing.yml)
[![codecov](https://codecov.io/github/kirbyevanj/k-hhc/graph/badge.svg?token=YT9H76C6MC)](https://codecov.io/github/kirbyevanj/k-hhc)
[![PyPI](https://img.shields.io/pypi/v/k-hhc.svg)](https://pypi.org/project/k-hhc/)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

A library for encoding/decoding HHC(hexahexacontadecimal) for 32bit and 64bit integers, with C++ and Python bindings.

Inspired by [hexahexacontadecimal](https://pypi.org/project/hexahexacontadecimal/2.2.1/) by [Alexander Ljungberg](https://github.com/aljungberg)


This project is intended to be an exploration in creating performant, secure, and portable algorithms, while also being a learning experience for me. Take it with a grain of salt.

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
- C++23 compatible compiler (GCC 11+, Clang 15+, or MSVC 2022+)
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
| `HHC_ENABLE_FUZZING` | `OFF` | Build libFuzzer targets for fuzz testing. Requires Clang compiler with fuzzing support. |
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

