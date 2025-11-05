# hcc-cpp

A C++23 header-only library for HCC encoding/decoding operations.

## Project Structure

```
hcc-cpp/
├── hcc-cpp/           # Header-only library
│   └── hcc.hpp
├── tests/             # GoogleTest unit tests
│   ├── CMakeLists.txt
│   └── test_hcc.cpp
├── benchmarks/        # Google Benchmark performance tests
│   ├── CMakeLists.txt
│   └── benchmark_hcc.cpp
├── CMakeLists.txt     # Root CMake configuration
└── .gitignore
```

## Building the Project

### Prerequisites

- CMake 3.20 or higher
- C++23 compatible compiler (GCC 11+, Clang 15+, or MSVC 2022+)
- Git (for fetching GoogleTest and Google Benchmark)

### Build Instructions

```bash
# Create build directory
mkdir build && cd build

# Configure the project
cmake ..

# Build all targets
cmake --build .
```

## Running Tests

```bash
# From the build directory
./tests/hcc_tests

# Or using CTest
ctest --output-on-failure
```

## Running Benchmarks

```bash
# From the build directory
./benchmarks/hcc_benchmarks
```

## Using the Library

The library is header-only, so simply include it in your project:

```cpp
#include "hcc.hpp"

int main() {
    hcc::hcc_encode();
    hcc::hcc_decode();
    hcc::hcc_32bit_encode();
    hcc::hcc_32bit_decode();
    hcc::hcc_64bit_encode();
    hcc::hcc_64bit_decode();
    return 0;
}
```

## API Reference

### Functions

- `void hcc_encode()` - Encode data using HCC algorithm
- `void hcc_decode()` - Decode HCC-encoded data
- `void hcc_32bit_encode()` - Encode data using 32-bit HCC variant
- `void hcc_32bit_decode()` - Decode 32-bit HCC-encoded data
- `void hcc_64bit_encode()` - Encode data using 64-bit HCC variant
- `void hcc_64bit_decode()` - Decode 64-bit HCC-encoded data

All functions are in the `hcc` namespace.

## External Dependencies

- [GoogleTest](https://github.com/google/googletest) - v1.14.0 (automatically downloaded)
- [Google Benchmark](https://github.com/google/benchmark) - v1.8.3 (automatically downloaded)

Dependencies are managed via CMake's ExternalProject_Add and will be automatically downloaded and built during configuration.

## Notes

- Benchmarks use `benchmark::ClobberMemory()` to prevent compiler optimizations from eliminating the function calls
- The library functions are currently stubs (TODO) for demonstration purposes

