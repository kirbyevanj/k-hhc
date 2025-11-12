#ifndef HHC_ASSERT_HPP
#define HHC_ASSERT_HPP

#include <cstdlib>
#include <iostream>

#if defined(__clang__)
#  define HHC_NO_PROFILE __attribute__((no_profile_instrument_function))
#else
#  define HHC_NO_PROFILE
#endif

// Platform-specific includes for stack traces
// Check Windows first (clang-cl defines __clang__ but not __unix__)
#if defined(_WIN32) || defined(_WIN64)
    #ifndef NOMINMAX
        #define NOMINMAX  // Prevent Windows from defining min/max macros
    #endif
    #include <windows.h>
    #include <dbghelp.h>
    #define HHC_HAVE_STACKWALK 1
    #pragma comment(lib, "dbghelp.lib")
#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__) || defined(__MACH__)
    #include <execinfo.h>
    #include <unistd.h>
    #define HHC_HAVE_BACKTRACE 1
#endif

namespace hhc::detail {

#ifdef LLVM_BUILD_INSTRUMENTED
HHC_NO_PROFILE
extern "C" int __llvm_profile_write_file(void);
inline void flush_coverage_profile() {
    (void)__llvm_profile_write_file();
}
#else
inline void flush_coverage_profile() {}
#endif


    /**
     * @brief Print stack trace (debug builds only)
     */
    HHC_NO_PROFILE inline void print_stack_trace() {

#if defined(HHC_HAVE_BACKTRACE)
        // Unix-like systems (Linux, macOS, BSD)
        constexpr int max_frames = 64;
        void* buffer[max_frames];
        
        int frame_count = backtrace(buffer, max_frames);
        
        std::cerr << "\n=== Stack Trace ===\n";
        backtrace_symbols_fd(buffer, frame_count, STDERR_FILENO);
        std::cerr << "===================\n\n";
        
#elif defined(HHC_HAVE_STACKWALK)
        // Windows
        constexpr int max_frames = 64;
        void* stack[max_frames];
        
        HANDLE process = GetCurrentProcess();
        SymInitialize(process, NULL, TRUE);
        
        WORD frame_count = CaptureStackBackTrace(0, max_frames, stack, NULL);
        
        std::cerr << "\n=== Stack Trace ===\n";
        
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
        if (symbol) {
            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            
            for (int i = 0; i < frame_count; i++) {
                if (SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol)) {
                    std::cerr << "  " << symbol->Name << " [0x" << std::hex << symbol->Address << std::dec << "]\n";
                } else {
                    std::cerr << "  [0x" << std::hex << (DWORD64)stack[i] << std::dec << "]\n";
                }
            }
            
            free(symbol);
        }
        
        std::cerr << "===================\n\n";
        SymCleanup(process);
        
#else
        // Fallback: no stack trace available
        std::cerr << "\n[Stack trace not available on this platform]\n\n";
#endif
    }

    /**
     * @brief Handle assertion failure
     * 
     * In debug builds: prints detailed error message with stack trace and aborts
     * In release builds: invokes undefined behavior trap (optimizer hint)
     * 
     * @param expression The expression that failed (as string)
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     */
    [[noreturn]] HHC_NO_PROFILE inline void assertion_failed(const char* expression,
                                                             const char* file,
                                                             const int line,
                                                             const char* function) {
#ifndef NDEBUG
        // Debug build: provide stack trace and error message
        std::cerr << "\n╔═════════════════════════════════════════════════════════════╗\n"
                  << "║                  HHC ASSERTION FAILED                       ║\n"
                  << "╚═════════════════════════════════════════════════════════════╝\n\n"
                  << "Expression: " << expression << '\n'
                  << "Location:   " << file << ':' << line << '\n'
                  << "Function:   " << function << '\n';
        
        print_stack_trace();
        
        std::cerr << "This is a critical error indicating a bug in the HHC library.\n"
                  << "Please report this issue with the above information.\n\n";
        
        flush_coverage_profile();
        std::abort();
#else
        // Release build: generate trap instruction
        // This tells the compiler this code path is unreachable,
        // enabling aggressive optimizations while still catching bugs
        
        // Suppress unused parameter warnings
        (void)expression;
        (void)file;
        (void)line;
        (void)function;
        
        flush_coverage_profile();
        
        // Use compiler built-in to generate trap instruction
        #if defined(__GNUC__) || defined(__clang__)
            __builtin_trap();
        #elif defined(_MSC_VER)
            __debugbreak();
        #else
            #error "Unsupported compiler (no trap instruction)"
        #endif
        
        // Tell compiler this is unreachable
        #if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
        #elif defined(_MSC_VER)
            __assume(0);
        #else
            #error "Unsupported compiler (no unreachable instruction)"
        #endif
#endif
    }

} // namespace hhc::detail

/**
 * @brief HHC assertion macro
 * 
 * Unlike standard assert(), this macro:
 * - Is ALWAYS enabled (both debug and release builds)
 * - In debug: provides detailed error message with stack trace
 * - In release: generates trap instruction (optimization hint)
 * 
 * Use this for conditions that should NEVER be false. If they are,
 * it indicates a critical bug in the library code.
 * 
 * Example:
 *   HHC_ASSERT(pointer != nullptr);
 * 
 * @param expr The expression to check (must evaluate to true)
 */
#define HHC_ASSERT(expr) \
    do { \
        if (!(expr)) [[unlikely]] { \
            ::hhc::detail::assertion_failed( \
                #expr, \
                __FILE__, \
                __LINE__, \
                __func__ \
            ); \
        } \
    } while (0)

/**
 * @brief HHC assertion macro with custom message
 * 
 * Same as HHC_ASSERT but allows a custom error message.
 * 
 * Example:
 *   HHC_ASSERT_MSG(size > 0, "Size must be positive");
 * 
 * @param expr The expression to check
 * @param msg Custom error message (string literal)
 */
#define HHC_ASSERT_MSG(expr, msg) \
    do { \
        if (!(expr)) [[unlikely]] { \
            ::hhc::detail::assertion_failed( \
                #expr " (" msg ")", \
                __FILE__, \
                __LINE__, \
                __func__ \
            ); \
        } \
    } while (0)

/**
 * @brief Mark a code path as unreachable
 * 
 * Use this for code paths that should never be reached.
 * In debug: aborts with error message
 * In release: generates trap instruction (optimization hint)
 * 
 * Example:
 *   switch (value) {
 *       case 1: return foo();
 *       case 2: return bar();
 *       default: HHC_UNREACHABLE("Invalid value");
 *   }
 * 
 * @param msg Explanation of why this should be unreachable
 */
#define HHC_UNREACHABLE(msg) \
    ::hhc::detail::assertion_failed( \
        "Unreachable code: " msg, \
        __FILE__, \
        __LINE__, \
        __func__ \
    )

#endif // HHC_ASSERT_HPP

