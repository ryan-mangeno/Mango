#pragma once

// included from my stdx project, check it out!
// https://github.com/ryan-mangeno/stdx/blob/main/stdx/include/type_traits.h

#include <defines.h>

// --- std::integral_constant ---
template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
};

// --- std::true_type and false_type ---
using true_type  = integral_constant<b8, TRUE>;
using false_type = integral_constant<b8, FALSE>;

// --- std::remove_reference ---
template<typename T> struct remove_reference      { using type = T; };
template<typename T> struct remove_reference<T&>  { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };
template<typename T> using  remove_reference_t    = typename remove_reference<T>::type;

// --- std::move ---
template<typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

// --- std::forward ---
template<typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

// --- std::enable_if ---
template<b8 B, typename T = void> struct enable_if {};
template<typename T>              struct enable_if<TRUE, T> { using type = T; };
template<b8 B, typename T = void> using  enable_if_t = typename enable_if<B, T>::type;

// --- std::decay ---
template<typename T>
struct decay {
    using type = remove_reference_t<T>;
};
template<typename T> using decay_t = typename decay<T>::type;

// --- std::is_same ---
template<typename T, typename U> struct is_same       : false_type { };
template<typename T>             struct is_same<T, T> : true_type  { };


// --- std::is_trivial ---
template<typename T> struct is_trivial  { static constexpr b8 value = __is_trivial(T) == TRUE; };
template<typename T> static constexpr b8 is_trivial_v = is_trivial<T>::value;

// --- std::is_copyable ---
template<typename T>
T&& declval();

template<typename T, typename = void> struct is_copyable : false_type {};
template<typename T>
struct is_copyable<T, decltype(void(
    T(declval<T>()),             // copy construct
    declval<T&>() = declval<T>() // copy assign
))> : true_type {};

template<typename T> static constexpr b8 is_copyable_v = is_trivial<T>::value;

// allows overriding of new and delete ops so I can use my own with placement new
inline void* operator new(unsigned long, void* ptr) noexcept { 
    return ptr; 
}
// Some compilers also require the matching placement delete
inline void operator delete(void*, void*) noexcept {}


