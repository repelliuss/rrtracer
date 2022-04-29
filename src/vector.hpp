#pragma once

#include "types.hpp"

union V2 {
  f32 e[2];
  struct {
    f32 x, y;
  };
};

union V3 {
  f32 e[3];
  struct {
    f32 x, y, z;
  };
  struct {
    f32 r, g, b;
  };
};

union V4 {
  f32 e[4];
  struct {
    f32 x, y, z, w;
  };
  struct {
    f32 left, right, bottom, top;
  };
};
