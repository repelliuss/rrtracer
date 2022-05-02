#pragma once

#include "vector.hpp"

#include <stdio.h>

inline void log_vector(V3 v) {
  printf("x: %f  y: %f  z:  %f\n", v.x, v.y, v.z);
}


inline void log_vector(v3u v) {
  printf("x: %u  y: %u  z:  %u\n", v.x, v.y, v.z);
}
