#pragma once

#include "vector.hpp"

#include <vector>
#include <string>

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
  V3 intensity; 
};

struct Material {
  std::string id;
  V3 ambient;
  V3 diffuse;
  V3 specular;
  f32 phong;
  V3 reflectance;
};

int material_by_id(Material *&material, std::vector<Material> &materials,
                   const char *name);

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
  constexpr V3 normal() const {
    V3 ba = b - a;
    V3 ca = c - a;
    return cross(ba, ca);
  }
};

// REVIEW: mesh id is not needed in xml
struct Mesh {
  std::vector<u32> triangle_ids;
  std::vector<TriangleFace> faces;

  Material *material;
};

struct Scene {
  u32 max_ray_trace_depth;
  V3 bg_color;
  Camera cam;

  std::vector<AmbientLight> ambient_lights;
  std::vector<PointLight> point_lights;
  std::vector<Material> materials;
  std::vector<V3> vertices;
  std::vector<Mesh> meshes; // NOTE: objects field in xml
};
