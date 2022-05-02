#pragma once

#include <stddef.h>

#include "vector.hpp"

namespace str {
bool is_match_ignore_case(const char *a, const char *b, size_t len);

template <class T> int to_array(T *arr, const char *str, u32 count);

// NOTE: only valid types that can use base conversion
int to_array(u32 *arr, const char *str, u32 count, int base);
int to_array(i32 *arr, const char *str, u32 count, int base);

template <class T>
int to_array(T *arr, u32 &count, const char *str, u32 str_length,
             u32 max_count);

// NOTE: only valid types that can use base conversion
int to_array(u32 *arr, u32 &count, const char *str, u32 str_length,
             u32 max_count, int base);
int to_array(i32 *arr, u32 &count, const char *str, u32 str_length,
             u32 max_count, int base);

// NOTE: in endptr versions, endptr can't be nullptr (asserted)

int to_integral(f32 &val, const char *str);
int to_integral(f32 &val, const char *str, char **endptr);

int to_integral(i32 &val, const char *str, int base = 0);
int to_integral(i32 &val, const char *str, char **endptr, int base = 0);

int to_integral(u32 &val, const char *str, int base = 0);
int to_integral(u32 &val, const char *str, char **endptr, int base = 0);

  
int to_vector(V2 &v2, const char *str);
int to_vector(V2u &v2, const char *str);
  
int to_vector(V3 &v3, const char *str);
int to_vector(V4 &v4, const char *str);
} // namespace str
