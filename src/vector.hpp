#pragma once

#include "types.hpp"

#include "math.h"

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
  struct {
    f32 u, v, w;
  };
};

using Point3 = V3;
using Color = V3;

///
/// V3
///

constexpr V3 v3(f32 a, f32 b, f32 c) { return {{a, b, c}}; }

constexpr V3 operator+(V3 a, V3 b) {
  return {{a.x + b.x, a.y + b.y, a.z + b.z}};
}

constexpr V3 operator+(V3 a, f32 s) {
  return {
      {a.x + s, a.y + s, a.z + s},
  };
}

constexpr V3 operator+=(V3 &a, V3 b) {
  a = a + b;
  return a;
}

// TODO: Remove zero check
constexpr V3 operator-(V3 a) {
  return {
    {
      -a.x == 0.0f ? 0.0f : -a.x,
      -a.y == 0.0f ? 0.0f : -a.y,
      -a.z == 0.0f ? 0.0f : -a.z
    },
  };
}

constexpr V3 operator-(V3 a, V3 b) {
  return {{a.x - b.x, a.y - b.y, a.z - b.z}};
}

constexpr V3 operator-(V3 a, f32 s) {
  return {
      {a.x - s, a.y - s, a.z - s},
  };
}

constexpr V3 operator*(V3 a, V3 b) {
  return {
      {a.x * b.x, a.y * b.y, a.z * b.z},
  };
}

constexpr V3 operator*(V3 u, f32 s) {
  return {
      {
          u.x * s,
          u.y * s,
          u.z * s,
      },
  };
}

constexpr V3 operator*(f32 s, const V3 &u) { return u * s; }

constexpr V3 cross(V3 a, V3 b) {
  return {{
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x,
  }};
}

constexpr f32 dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

constexpr f32 length_sqr(V3 a) { return dot(a, a); }

constexpr f32 length(V3 a) { return sqrt(length_sqr(a)); }

constexpr V3 norm(V3 a) { return a * (1.0f / length(a)); }

///
/// V4
///

union V4 {
  f32 e[4];
  struct {
    f32 x, y, z, w;
  };
  struct {
    f32 left, right, bottom, top;
  };
};

union v2i {
  i32 e[2];
  struct {
    i32 x, y;
  };

  v2i(V2 v) {
    x = static_cast<i32>(v.x);
    y = static_cast<i32>(v.y);
  }
};

union V2u {
  u32 e[2];
  struct {
    u32 x, y;
  };
  struct {
    u32 beg, end;
  };
};

constexpr V2u v2u(V2 v) {
  return {
      {static_cast<u32>(v.x), static_cast<u32>(v.y)},
  };
}

constexpr V2u v2u(u32 a, u32 b) { return {{a, b}}; }

union v3u {
  u32 e[3];
  struct {
    u32 x, y, z;
  };
  struct {
    u32 r, g, b;
  };

  v3u(V3 v) {
    x = static_cast<u32>(v.x);
    y = static_cast<u32>(v.y);
    z = static_cast<u32>(v.z);
  }
};
