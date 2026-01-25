#pragma once

namespace Mango {

    // Unsigned int types.
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef unsigned long long u64;

    // Signed int types.
    typedef signed char i8;
    typedef signed short i16;
    typedef signed int i32;
    typedef signed long long i64;

    // Floating point types
    typedef float f32;
    typedef double f64;

    // Boolean types
    typedef int b32;
    typedef char b8;

    static_assert(sizeof(u8) == 1, "u8 must be 1 byte");
    static_assert(sizeof(u16) == 2, "u16 must be 2 bytes");
    static_assert(sizeof(u32) == 4, "u32 must be 4 bytes");
    static_assert(sizeof(u64) == 8, "u64 must be 8 bytes");


    static_assert(sizeof(i8) == 1, "i8 must be 1 byte");
    static_assert(sizeof(i16) == 2, "i16 must be 2 bytes");
    static_assert(sizeof(i32) == 4, "i32 must be 4 bytes");
    static_assert(sizeof(i64) == 8, "i64 must be 8 bytes");


    static_assert(sizeof(f32) == 4, "f32 must be 4 bytes");
    static_assert(sizeof(f64) == 8, "f64 must be 8 bytes");

    #define TRUE 1
    #define FALSE 0
    // Platform detection
    #if defined(_WIN32) || defined(WIN32)
        #define MGO_PLATFORM_WINDOWS 1
        #if !defined(_WIN64)
            #error "64-bit Windows is required!"
        #endif

    #elif defined(__linux__) || defined(__gnu_linux__)
        #define MGO_PLATFORM_LINUX 1
        #if defined(__ANDROID__)
            #define MGO_PLATFORM_ANDROID 1
        #endif

    #elif defined(__APPLE__)
        #include <TargetConditionals.h>
        #define MGO_PLATFORM_APPLE 1
        #if TARGET_IPHONE_SIMULATOR
            #define MGO_PLATFORM_IOS 1
            #define MGO_PLATFORM_IOS_SIMULATOR 1
        #elif TARGET_OS_IPHONE
            #define MGO_PLATFORM_IOS 1
        #elif TARGET_OS_MAC
            #define MGO_PLATFORM_MAC 1
        #else
            #error "Unknown Apple platform"
        #endif

    #elif defined(__unix__)
        #define MGO_PLATFORM_UNIX 1

    #elif defined(_POSIX_VERSION)
        #define MGO_PLATFORM_POSIX 1

    #else
        #error "Unknown platform!"
    #endif

    // Export / Import macros
    #ifdef MGO_EXPORT
        #ifdef _MSC_VER
            #define MGO_API __declspec(dllexport)
        #else
            #define MGO_API __attribute__((visibility("default")))
        #endif
    #else
        #ifdef _MSC_VER
            #define MGO_API __declspec(dllimport)
        #else
            #define MGO_API
        #endif
    #endif
        

}