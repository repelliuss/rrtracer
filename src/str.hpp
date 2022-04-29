#pragma once

#include <stddef.h>

#include "vector.hpp"

namespace str {
  bool is_match_ignore_case(const char *a, const char *b, size_t len);
  
  template<class T> int to_array(T *arr, const char *str, u32 count);
  // NOTE: only valid type that can use base conversion
  int to_array(u32 *arr, const char *str, u32 count, int base);
  
  int to_integral(float &val, const char *str, char **endptr);
  int to_vector(V2 &v2, const char *str);
  int to_vector(V3 &v3, const char *str);
  int to_vector(V4 &v4, const char *str);
}
