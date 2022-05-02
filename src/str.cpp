#include "str.hpp"
#include "scene.hpp"
#include "vector.hpp"

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

namespace str {

bool is_match_ignore_case(const char *a, const char *b, size_t max_len) {
  for (size_t i = 0; i < max_len; ++i) {
    if (tolower(a[i]) != tolower(b[i]))
      return false;
    if (a[i] == 0)
      return true;
  }

  return true;
}

int find_right_whitespace_end(const char *str, u32 length) {
  for (; length > 0 && isspace(str[length - 1]); --length)
    ;

  return length;
}

// NOTE: to_integral fns doesn't care about overflows

int to_integral(f32 &val, const char *str) {
  char *endptr;

  f32 parsed = strtof(str, &endptr);

  if (*endptr != 0 && !isspace(*endptr))
    return -1;

  val = parsed;

  return 0;
}

/// base == 0 is for decimals or [2, 36] see strtol
int to_integral(i32 &val, const char *str, int base) {
  char *endptr;

  i32 parsed = strtol(str, &endptr, base);

  if (*endptr != 0 && !isspace(*endptr))
    return -1;

  val = parsed;

  return 0;
}

/// base == 0 is for decimals or [2, 36] see strtol
int to_integral(u32 &val, const char *str, int base) {
  char *endptr;
  i32 parsed = strtol(str, &endptr, base);

  if (*endptr != 0 && !isspace(*endptr))
    return -1;

  if (parsed < 0)
    return -2;

  val = parsed;

  return 0;
}

int to_integral(f32 &val, const char *str, char **endptr) {
  assert(endptr != nullptr);

  f32 parsed = strtof(str, endptr);

  if (**endptr != 0 && !isspace(**endptr))
    return -1;

  val = parsed;

  return 0;
}

/// base == 0 is for decimals or [2, 36] see strtol
int to_integral(i32 &val, const char *str, char **endptr, int base) {
  assert(endptr != nullptr);

  i32 parsed = strtol(str, endptr, base);

  if (**endptr != 0 && !isspace(**endptr))
    return -1;

  val = parsed;

  return 0;
}

/// base == 0 is for decimals or [2, 36] see strtol
int to_integral(u32 &val, const char *str, char **endptr, int base) {
  assert(endptr != nullptr);

  i32 parsed = strtol(str, endptr, base);

  if (**endptr != 0 && !isspace(**endptr))
    return -1;

  if (parsed < 0)
    return -2;

  val = parsed;

  return 0;
}

template <class T> int to_array(T *arr, const char *str, u32 count) {
  assert(arr != nullptr);

  T val = 0;
  char *endptr = nullptr;

  for (u32 i = 0; i < count; ++i) {
    int status = to_integral(val, str, &endptr);
    if (status < 0)
      return status;
    arr[i] = val;
    str = endptr;
  }

  return 0;
}

template <class T> int to_array(T *arr, const char *str, u32 count, int base) {
  assert(arr != nullptr);

  T val = 0;
  char *endptr = nullptr;

  for (u32 i = 0; i < count; ++i) {
    int status = to_integral(val, str, &endptr, base);
    if (status < 0)
      return status;
    arr[i] = val;
    str = endptr;
  }

  return 0;
}

template <class T>
int to_array(T *arr, u32 &count, const char *str, u32 str_length,
             u32 max_count) {
  assert(arr != nullptr);

  char *endptr = nullptr;
  const char *str_end = str + find_right_whitespace_end(str, str_length);
  T val = 0;

  count = 0;

  for (u32 i = 0; i < max_count && str < str_end; ++i) {
    int status = to_integral(val, str, &endptr);
    if (status < 0)
      return status;
    arr[i] = val;
    str = endptr;
    ++count;
  }

  return 0;
}
template int to_array<f32>(f32 *, u32 &, const char *, u32, u32);
template int to_array<u32>(u32 *, u32 &, const char *, u32, u32);

template <class T>
int to_array(T *arr, u32 &count, const char *str, u32 str_length, u32 max_count,
             int base) {
  assert(arr != nullptr);

  char *endptr = nullptr;
  const char *str_end = str + find_right_whitespace_end(str, str_length);
  T val = 0;

  count = 0;

  for (u32 i = 0; i < max_count && str < str_end; ++i) {
    int status = to_integral(val, str, &endptr, base);
    if (status < 0)
      return status;
    arr[i] = val;
    str = endptr;
    ++count;
  }

  return 0;
}

template <class T>
int to_array(std::vector<T> &arr, const char *str, u32 str_length) {
  char *endptr = nullptr;
  const char *str_end = str + find_right_whitespace_end(str, str_length);

  for (u32 i = 0; str < str_end; ++i) {
    arr.emplace_back();
    int status = to_integral(arr.back(), str, &endptr);
    if (status < 0)
      return status;
    str = endptr;
  }

  return 0;
}
template int to_array(std::vector<u32> &arr, const char *str, u32 str_length);

int to_vector(V2 &v2, const char *str) { return to_array(v2.e, str, 2); }
int to_vector(V2u &v2, const char *str) { return to_array(v2.e, str, 2); }
  
int to_vector(V3 &v3, const char *str) { return to_array(v3.e, str, 3); }
int to_vector(V4 &v4, const char *str) { return to_array(v4.e, str, 4); }

} // namespace str
