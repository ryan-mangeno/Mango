#pragma once

#include "core/mgmemory.h"
#include "defines.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vsnprintf
#include <string.h> // strlen, memcmp, strncmp

// mg_string
//
// A UTF-8 byte string with small-string optimization (SSO).
//
// Layout:
//   SSO  (len <= SSO_CAP): characters stored directly in buf[], no heap alloc.
//   Heap (len >  SSO_CAP): buf holds a heap pointer + length + capacity.
//
// SSO_CAP = 15 bytes of character data + 1 implicit null terminator.
// The last byte of the SSO buffer doubles as the "is heap" flag:
//   0x00  => SSO  (remaining capacity is also encoded here as SSO_CAP - len)
//   0x01  => heap

class MGO_API mg_string {
public:
  // Constants
  static constexpr u64 SSO_CAP = 15; // max chars stored inline
  static constexpr u64 NPOS = ~(u64)0;

  // constructors / destructor
  mg_string() { _sso_init_empty(); }

  mg_string(const char *cstr) {
    if (!cstr) {
      _sso_init_empty();
      return;
    }
    _assign_cstr(cstr, (u64)strlen(cstr));
  }

  mg_string(const char *data, u64 len) { _assign_cstr(data, len); }

  mg_string(const mg_string &other) { _copy_from(other); }

  mg_string(mg_string &&other) noexcept { _move_from(other); }

  ~mg_string() { _free_heap(); }

  // Assignment
  mg_string &operator=(const mg_string &other) {
    if (this == &other)
      return *this;
    _free_heap();
    _copy_from(other);
    return *this;
  }

  mg_string &operator=(mg_string &&other) noexcept {
    if (this == &other)
      return *this;
    _free_heap();
    _move_from(other);
    return *this;
  }

  mg_string &operator=(const char *cstr) {
    _free_heap();
    if (!cstr) {
      _sso_init_empty();
      return *this;
    }
    _assign_cstr(cstr, (u64)strlen(cstr));
    return *this;
  }

  // Observers
  const char *c_str() const { return _is_heap() ? _heap.ptr : _sso.buf; }

  char *data() { return _is_heap() ? _heap.ptr : _sso.buf; }

  u64 length() const { return _is_heap() ? _heap.len : (u64)_sso.len; }

  u64 capacity() const { return _is_heap() ? _heap.cap : SSO_CAP; }

  b8 empty() const { return length() == 0; }

  // Element access
  char &operator[](u64 i) { return data()[i]; }
  char operator[](u64 i) const { return c_str()[i]; }

  char &front() { return data()[0]; }
  char front() const { return c_str()[0]; }

  char &back() { return data()[length() - 1]; }
  char back() const { return c_str()[length() - 1]; }

  // Modification
  void clear() {
    _free_heap();
    _sso_init_empty();
  }

  void reserve(u64 new_cap) {
    if (new_cap <= capacity())
      return;
    _grow_to(new_cap);
  }

  void push_back(char c) {
    u64 len = length();
    _ensure_capacity(len + 1);
    data()[len] = c;
    data()[len + 1] = '\0';
    _set_length(len + 1);
  }

  void pop_back() {
    u64 len = length();
    if (len == 0)
      return;
    data()[len - 1] = '\0';
    _set_length(len - 1);
  }

  mg_string &append(const char *cstr, u64 len) {
    if (!cstr || len == 0)
      return *this;
    u64 cur = length();
    _ensure_capacity(cur + len);
    mg_copy_memory(data() + cur, cstr, len);
    data()[cur + len] = '\0';
    _set_length(cur + len);
    return *this;
  }

  mg_string &append(const char *cstr) {
    if (!cstr)
      return *this;
    return append(cstr, (u64)strlen(cstr));
  }

  mg_string &append(const mg_string &other) {
    return append(other.c_str(), other.length());
  }

  mg_string &operator+=(const mg_string &other) { return append(other); }
  mg_string &operator+=(const char *cstr) { return append(cstr); }
  mg_string &operator+=(char c) {
    push_back(c);
    return *this;
  }

  // Search
  u64 find(char c, u64 from = 0) const {
    const char *s = c_str();
    u64 len = length();
    for (u64 i = from; i < len; ++i)
      if (s[i] == c)
        return i;
    return NPOS;
  }

  u64 find(const char *needle, u64 from = 0) const {
    if (!needle || !needle[0])
      return NPOS;
    u64 nlen = (u64)strlen(needle);
    u64 len = length();
    if (nlen > len)
      return NPOS;
    const char *s = c_str();
    for (u64 i = from; i <= len - nlen; ++i)
      if (memcmp(s + i, needle, nlen) == 0)
        return i;
    return NPOS;
  }

  b8 contains(const char *needle) const { return find(needle) != NPOS; }
  b8 contains(char c) const { return find(c) != NPOS; }

  b8 starts_with(const char *prefix) const {
    u64 plen = (u64)strlen(prefix);
    if (plen > length())
      return FALSE;
    return memcmp(c_str(), prefix, plen) == 0;
  }

  b8 ends_with(const char *suffix) const {
    u64 slen = (u64)strlen(suffix);
    u64 len = length();
    if (slen > len)
      return FALSE;
    return memcmp(c_str() + len - slen, suffix, slen) == 0;
  }

  // Substrings / slicing
  mg_string substr(u64 pos, u64 count = NPOS) const {
    u64 len = length();
    if (pos >= len)
      return mg_string();
    u64 actual = (count == NPOS || pos + count > len) ? len - pos : count;
    return mg_string(c_str() + pos, actual);
  }

  // Comparison
  i32 compare(const mg_string &other) const {
    return compare(other.c_str(), other.length());
  }

  i32 compare(const char *cstr) const {
    if (!cstr)
      return length() > 0 ? 1 : 0;
    return compare(cstr, (u64)strlen(cstr));
  }

  i32 compare(const char *cstr, u64 len2) const {
    u64 len1 = length();
    u64 minlen = len1 < len2 ? len1 : len2;
    i32 result = memcmp(c_str(), cstr, minlen);
    if (result != 0)
      return result;
    if (len1 < len2)
      return -1;
    if (len1 > len2)
      return 1;
    return 0;
  }

  b8 operator==(const mg_string &o) const { return compare(o) == 0; }
  b8 operator!=(const mg_string &o) const { return compare(o) != 0; }
  b8 operator<(const mg_string &o) const { return compare(o) < 0; }
  b8 operator>(const mg_string &o) const { return compare(o) > 0; }
  b8 operator==(const char *s) const { return compare(s) == 0; }
  b8 operator!=(const char *s) const { return compare(s) != 0; }

  // Static helpers

  // printf-style formatting into a new mg_string
  static mg_string format(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    i32 needed = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);

    if (needed <= 0)
      return mg_string();

    mg_string result;
    result._ensure_capacity((u64)needed);

    va_start(args, fmt);
    vsnprintf(result.data(), (u64)needed + 1, fmt, args);
    va_end(args);

    result._set_length((u64)needed);
    return result;
  }

  // Iterators
  char *begin() { return data(); }
  char *end() { return data() + length(); }
  const char *begin() const { return c_str(); }
  const char *end() const { return c_str() + length(); }

private:
  // Internal layout
  //
  // We keep two representations in a union:
  //
  //   SSO:  [ buf[16] | u8 len | padding | u8 flag  ]
  //   Heap: [ char* ptr | u64 len | u64 cap | padding | u8 flag  ]
  //
  // The u8 at _sso.flag / _heap.flag is the last byte in both paths.
  // flag == 0 => SSO, flag == 1 => heap.

  struct heap_rep {
    char *ptr;
    u64 len;
    u64 cap;
    u8 _pad[7];
    u8 flag; // always 1
  };

  struct sso_rep {
    char buf[SSO_CAP +
             1]; // buf[15] is the null terminator slot; buf[0..len-1] is data
    u8 len;      // 0..15
    u8 _pad[14];
    u8 flag; // always 0
  };

  // Both reps are the same total size.
  static_assert(sizeof(heap_rep) == sizeof(sso_rep), "rep size mismatch");

  union {
    heap_rep _heap;
    sso_rep _sso;
  };

  // Internal helpers
  b8 _is_heap() const { return _sso.flag == 1; }

  void _sso_init_empty() {
    mg_zero_memory(this, sizeof(mg_string));
    // _sso.flag = 0 already; _sso.len = 0
  }

  void _set_length(u64 len) {
    if (_is_heap())
      _heap.len = len;
    else
      _sso.len = (u8)len;
  }

  // Promote SSO buffer to heap, or grow existing heap buffer.
  void _grow_to(u64 new_cap) {
    u64 cur_len = length();

    // Grow by 1.5x to amortize allocs
    if (new_cap < new_cap + (new_cap >> 1))
      new_cap = new_cap + (new_cap >> 1);

    char *new_ptr = (char *)mg_allocate(new_cap + 1, MEMORY_TAG_STRING);
    mg_copy_memory(new_ptr, c_str(), cur_len);
    new_ptr[cur_len] = '\0';

    if (_is_heap())
      mg_free(_heap.ptr, _heap.cap + 1, MEMORY_TAG_STRING);

    _heap.ptr = new_ptr;
    _heap.len = cur_len;
    _heap.cap = new_cap;
    _heap.flag = 1;
  }

  void _ensure_capacity(u64 needed) {
    if (needed > capacity())
      _grow_to(needed);
  }

  void _free_heap() {
    if (_is_heap() && _heap.ptr)
      mg_free(_heap.ptr, _heap.cap + 1, MEMORY_TAG_STRING);
  }

  void _assign_cstr(const char *src, u64 len) {
    if (len <= SSO_CAP) {
      mg_zero_memory(this, sizeof(mg_string));
      mg_copy_memory(_sso.buf, src, len);
      _sso.buf[len] = '\0';
      _sso.len = (u8)len;
      _sso.flag = 0;
    } else {
      _heap.ptr = (char *)mg_allocate(len + 1, MEMORY_TAG_STRING);
      mg_copy_memory(_heap.ptr, src, len);
      _heap.ptr[len] = '\0';
      _heap.len = len;
      _heap.cap = len;
      _heap.flag = 1;
    }
  }

  void _copy_from(const mg_string &other) {
    if (!other._is_heap()) {
      mg_copy_memory(this, &other, sizeof(mg_string));
    } else {
      _heap.ptr = (char *)mg_allocate(other._heap.cap + 1, MEMORY_TAG_STRING);
      mg_copy_memory(_heap.ptr, other._heap.ptr, other._heap.len + 1);
      _heap.len = other._heap.len;
      _heap.cap = other._heap.cap;
      _heap.flag = 1;
    }
  }

  void _move_from(mg_string &other) noexcept {
    mg_copy_memory(this, &other, sizeof(mg_string));
    other._sso_init_empty(); // leave other in valid empty state
  }
};

// Free-function concatenation
inline mg_string operator+(mg_string lhs, const mg_string &rhs) {
  lhs.append(rhs);
  return lhs;
}

inline mg_string operator+(mg_string lhs, const char *rhs) {
  lhs.append(rhs);
  return lhs;
}

inline mg_string operator+(const char *lhs, const mg_string &rhs) {
  mg_string result(lhs);
  result.append(rhs);
  return result;
}
