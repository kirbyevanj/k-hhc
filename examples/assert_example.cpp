/**
 * @file assert_example.cpp
 * @brief Demonstration of HHC custom assertion module
 * 
 * This example shows the behavior of HHC_ASSERT in both debug and release builds.
 */

#include "hhc_assert.hpp"
#include <iostream>

int safe_divide(int a, int b) {
    // This assertion will:
    // - In Debug: print detailed error + stack trace if b == 0, then abort
    // - In Release: generate trap instruction if b == 0 (unreachable code hint)
    HHC_ASSERT_MSG(b != 0, "Division by zero is not allowed");
    
    return a / b;
}

void safe_copy(char* dest, const char* src, size_t size) {
    HHC_ASSERT(dest != nullptr);
    HHC_ASSERT(src != nullptr);
    HHC_ASSERT_MSG(size > 0, "Size must be positive");
    
    for (size_t i = 0; i < size; ++i) {
        dest[i] = src[i];
    }
}

int main() {
    std::cout << "HHC Custom Assertion Module Demo\n";
    std::cout << "=================================\n\n";
    
#ifndef NDEBUG
    std::cout << "Build Mode: DEBUG\n";
    std::cout << "Assertions will print detailed error messages with stack traces.\n\n";
#else
    std::cout << "Build Mode: RELEASE\n";
    std::cout << "Assertions will generate trap instructions (optimizer hints).\n\n";
#endif

    // Example 1: Passing assertions (work in both debug and release)
    std::cout << "1. Testing passing assertions...\n";
    HHC_ASSERT(1 + 1 == 2);
    HHC_ASSERT_MSG(42 > 0, "Forty-two is positive");
    std::cout << "   ✓ All assertions passed\n\n";
    
    // Example 2: Function with assertions
    std::cout << "2. Testing function with assertions...\n";
    int result = safe_divide(10, 2);
    std::cout << "   ✓ 10 / 2 = " << result << "\n\n";
    
    // Example 3: Buffer operations with assertions
    std::cout << "3. Testing buffer operations with assertions...\n";
    char dest[10];
    const char* src = "Hello";
    safe_copy(dest, src, 5);
    dest[5] = '\0';
    std::cout << "   ✓ Copied: " << dest << "\n\n";
    
    std::cout << "All examples completed successfully!\n\n";
    
    return 0;
}

