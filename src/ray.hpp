#pragma once

#include "scene.hpp"
#include "vector.hpp"

#include <vector>

struct Ray {
  Point3 origin;
  V3 direction;

  constexpr Point3 at(f32 t) const { return origin + direction * t; }
};

struct Plane {
  Point3 top_left;
  V4 borders;
  V2u aspect_size;
  Orientation orientation;
};

constexpr Plane near_plane_of_cam(const Camera &cam) {
  Point3 middle = cam.pos + (-cam.orientation.w) * cam.near_dist;
  V4 borders = cam.near_plane;
  Point3 top_left = middle + borders.left * cam.orientation.u +
                    borders.top * cam.orientation.v;
  return {
      .top_left = top_left,
      .borders = borders,
      .aspect_size = cam.resolution,
      .orientation = cam.orientation,
  };
}

inline Point3 pixel_on_plane(V2u pixel, const Plane &plane) {
  f32 u_offset = (pixel.x + 0.5f) * (plane.borders.right - plane.borders.left) /
                 plane.aspect_size.x;
  f32 v_offset = (pixel.y + 0.5f) * (plane.borders.top - plane.borders.bottom) /
                 plane.aspect_size.y;

  return plane.top_left + (u_offset * plane.orientation.u) -
         (v_offset * plane.orientation.v);
}

constexpr Ray ray_between(Point3 beg, Point3 end) {
  return {
      .origin = beg,
      // NOTE: direction is normalized!
      .direction = end - beg,
  };
}

namespace ray {

struct Input {
  Scene *scene;
  V2u y_range;
};

struct ThreadInput {
  std::vector<Color> *colors;
  Input *in;
};

void *threaded_trace(void *arg);
  
int trace(std::vector<Color> *colors, Input *in);
} // namespace ray
