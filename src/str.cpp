#include "str.hpp"
#include "vector.hpp"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

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

int to_integral(f32 &val, const char *str, char **endptr) {
  val = strtof(str, endptr);

  if (**endptr == 0 || isspace(**endptr))
    return 0;

  return -1;
}

/// base == 0 is for decimals or [2, 36] see strtol
int to_integral(u32 &val, const char *str, char **endptr, int base = 0) {
  val = strtol(str, endptr, base);

  if (**endptr == 0 || isspace(**endptr))
    return 0;

  return -1;
}

template <class T> int to_array(T *arr, const char *str, u32 count) {
  assert(arr != nullptr);

  T val = 0;
  char *endptr = nullptr;

  // REVIEW: should I decompose this for loop?
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

  // REVIEW: should I decompose this for loop?
  for (u32 i = 0; i < count; ++i) {
    int status = to_integral(val, str, &endptr, base);
    if (status < 0)
      return status;
    arr[i] = val;
    str = endptr;
  }

  return 0;
}

int to_vector(V2 &v2, const char *str) { return to_array(v2.e, str, 2); }
int to_vector(V3 &v3, const char *str) { return to_array(v3.e, str, 3); }
int to_vector(V4 &v4, const char *str) { return to_array(v4.e, str, 4); }

} // namespace str
