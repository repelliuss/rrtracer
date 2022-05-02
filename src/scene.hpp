#pragma once

#include "vector.hpp"

// REVIEW: Arrays being fixed
// REVIEW: Initial zero assignments. They should be initialized with zero when parsed in xml.cpp

// TODO: can we make arrays to its their own struct with counts?

struct Orientation {
  V3 u, v, w;
};

struct Camera {
  V3 pos;
  V3 gaze;
  V3 up;
  V4 near_plane;
  f32 near_dist;
  V2u resolution;
  Orientation orientation;
};

struct AmbientLight {
  V3 color;
};

// REVIEW: point light id is not needed in xml
struct PointLight {
  V3 pos;
  V3 intensity; // REVIEW: is this actually color?
};

struct Material {
  char id[256];
  V3 ambient;
  V3 diffuse;
  V3 specular;
  f32 phong;
  V3 reflactance;
};

int material_by_id(Material *&material, Material *materials, u32 material_count,
                   const char *id);

struct TriangleFace {
  union {
    V3 vertices[3];
    struct {
      V3 a;
      V3 b;
      V3 c;
    };
  };
  Color color;
  // REVIEW: can return -0 value on axis
  constexpr V3 normal() const {
    V3 ba = b - a;
    V3 ca = c - a;
    return cross(ba, ca);
  }
};

// REVIEW: mesh id is not needed in xml
struct Mesh {
  // REVIEW: may not be needed
  // NOTE: should point to Scene.vertices
  V3 *vertices;

  u32 triangles[1024 * 3]; // NOTE: faces field in xml
  u32 triangle_count = 0;

  TriangleFace faces[1024];
  u32 face_count;

  Material *material;
};

struct Scene {
  u32 max_ray_trace_depth;
  V3 bg_color;
  Camera cam;

  AmbientLight ambient_lights[16];
  u32 ambient_light_count = 0;

  PointLight point_lights[16];
  u32 point_light_count = 0;

  Material materials[16];
  u32 material_count = 0;

  // TODO: remove this
  static constexpr u32 vertex_capacity = 1024;
  static constexpr u32 integral_capacity = 16;

  V3 vertices[vertex_capacity];
  u32 vertex_count = 0;

  Mesh meshes[16]; // NOTE: objects field in xml
  u32 mesh_count = 0;
};
