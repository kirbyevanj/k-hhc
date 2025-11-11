#include <gtest/gtest.h>
#include "hhc_assert.hpp"

#ifndef _WIN32
#include <csignal>
#include <csetjmp>
#endif

/**
 * @file assert_tests.cpp
 * @brief Unit tests for the HHC custom assertion module.
 * 
 * Tests verify that HHC_ASSERT provides proper error handling in both
 * debug and release builds, with stack traces in debug and trap instructions
 * in release.
 */

#ifndef _WIN32
// Unix-like signal handling
namespace {

static std::jmp_buf jump_buffer;
static volatile bool signal_caught = false;

void signal_handler([[maybe_unused]] int sig) {
    signal_caught = true;
    std::longjmp(jump_buffer, 1);
}

} // namespace
#endif

// Test that passing condition does not trigger assertion
TEST(HhcAssertTest, PassingConditionDoesNotFail) {
    EXPECT_NO_THROW({
        HHC_ASSERT(1 + 1 == 2);
        HHC_ASSERT(true);
        HHC_ASSERT(42 > 0);
    });
}

// Test that HHC_ASSERT_MSG works with passing conditions
TEST(HhcAssertTest, PassingConditionWithMessageDoesNotFail) {
    EXPECT_NO_THROW({
        HHC_ASSERT_MSG(1 + 1 == 2, "Math should work");
        HHC_ASSERT_MSG(true, "True is true");
    });
}

// Test with complex expressions
TEST(HhcAssertTest, ComplexExpressionPasses) {
    int x = 10;
    int y = 20;
    EXPECT_NO_THROW({
        HHC_ASSERT(x < y && y > 0 && x >= 0);
    });
}

// Test with pointer checks
TEST(HhcAssertTest, PointerCheckPasses) {
    int value = 42;
    int* ptr = &value;
    EXPECT_NO_THROW({
        HHC_ASSERT(ptr != nullptr);
        HHC_ASSERT(*ptr == 42);
    });
}

#ifndef NDEBUG
// Debug-specific tests

TEST(HhcAssertTest, FailingConditionAbortsInDebug) {
    // In debug mode, HHC_ASSERT should call abort()
    // We test this by checking if EXPECT_DEATH detects it
    
    EXPECT_DEATH({
        HHC_ASSERT(false);
    }, "ASSERTION FAILED");
}

TEST(HhcAssertTest, FailingConditionWithMessageAbortsInDebug) {
    EXPECT_DEATH({
        HHC_ASSERT_MSG(false, "Custom error message");
    }, "Custom error message");
}

TEST(HhcAssertTest, UnreachableCodeAbortsInDebug) {
    EXPECT_DEATH({
        HHC_UNREACHABLE("This should never execute");
    }, "Unreachable code");
}

TEST(HhcAssertTest, DebugPrintsFileName) {
    // Check that file name appears in output
    EXPECT_DEATH({
        HHC_ASSERT(1 == 2);
    }, "assert_tests.cpp");
}

TEST(HhcAssertTest, DebugPrintsLineNumber) {
    // Check that line numbers appear in output
    EXPECT_DEATH({
        HHC_ASSERT(false);
    }, "[0-9]+");
}

TEST(HhcAssertTest, DebugPrintsExpression) {
    // Check that the failing expression appears in output
    EXPECT_DEATH({
        HHC_ASSERT(1 + 1 == 3);
    }, "1 \\+ 1 == 3");
}

#else
// Release-specific tests

// Note: In release mode, HHC_ASSERT calls __builtin_trap() which generates
// SIGILL or SIGTRAP. These tests verify the trap behavior.

TEST(HhcAssertTest, FailingConditionTrapsInRelease) {
#ifdef _WIN32
    // Windows: We can't easily test __debugbreak() without crashing the test runner
    // Just verify that the code compiles and the macro expands correctly
    // The trap behavior is tested implicitly by the fact that debug builds work
    GTEST_SKIP() << "Trap testing not supported on Windows (would crash test runner)";
#else
    // Unix-like: Use signal handlers to catch traps
    // __builtin_trap() can generate SIGILL or SIGTRAP depending on platform
    // (SIGILL on x86/x64, SIGTRAP on ARM64)
    
    signal_caught = false;
    
    // Set up signal handlers for both possible signals
    auto old_sigill_handler = std::signal(SIGILL, signal_handler);
    
#ifdef SIGTRAP
    auto old_sigtrap_handler = std::signal(SIGTRAP, signal_handler);
#endif
    
    if (setjmp(jump_buffer) == 0) {
        // First time through - trigger the assertion
        HHC_ASSERT(false);
        // Should never reach here
        FAIL() << "HHC_ASSERT did not trap in release mode";
    } else {
        // Caught the signal - test passes
        EXPECT_TRUE(signal_caught);
    }
    
    // Restore old handlers
    std::signal(SIGILL, old_sigill_handler);
    
#ifdef SIGTRAP
    std::signal(SIGTRAP, old_sigtrap_handler);
#endif
#endif
}

#endif

// Tests that work in both debug and release

TEST(HhcAssertTest, MultipleAssertionsInSequence) {
    EXPECT_NO_THROW({
        HHC_ASSERT(true);
        HHC_ASSERT(1 == 1);
        HHC_ASSERT(42 > 0);
        HHC_ASSERT_MSG(true, "Should pass");
    });
}

TEST(HhcAssertTest, AssertInFunction) {
    auto check_positive = [](int x) {
        HHC_ASSERT(x > 0);
        return x * 2;
    };
    
    EXPECT_NO_THROW({
        int result = check_positive(5);
        EXPECT_EQ(result, 10);
    });
}

TEST(HhcAssertTest, AssertWithSideEffects) {
    // Verify that the expression is always evaluated
    // (even though assertions are typically pure checks)
    int counter = 0;
    auto increment = [&counter]() -> bool {
        counter++;
        return true;
    };
    
    HHC_ASSERT(increment());
    EXPECT_EQ(counter, 1);
}

