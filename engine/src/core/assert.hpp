#pragma once

#include <defines.hpp>

namespace Mango {

    // comment out this line to disable assertions
    #define MGO_ASSERTS_ENABLED

    #ifdef MGO_ASSERTS_ENABLED
        #if _MSC_VER
            #define debugbreak() __debugbreak()
        #else 
            #define debugbreak() __builtin_trap()
        #endif
    #endif

    #define MGO_ASSERT(expr) \
            if (!(expr)) { \
                report_assertion_failure(#expr, "Assertion failed", __FILE__, __LINE__); \
                debugbreak(); \
            } \

    #define MGO_ASSERT_MSG(expr, msg) \
            if (!(expr)) { \
                report_assertion_failure(#expr, msg, __FILE__, __LINE__); \
                debugbreak(); \
            } \

    MGO_API void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line);

}