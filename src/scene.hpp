#pragma once

#include "vector.hpp"

struct Camera {
  V3 pos;
  V3 gaze;
  V3 up;
  V4 near_plane;
  V2 resolution;
  f32 dist;
};

struct AmbientLight {
  V3 color;
};

// REVIEW: point light id is not needed in xml
struct PointLight {
  V3 pos;
  V3 color;
};

// REVIEW: may need "id" field, or not because materials doesn't change
struct Material {
  V3 ambient;
  V3 diffuse;
  V3 specular;
  f32 phong;
  V3 reflactance;
};

// REVIEW: mesh id is not needed in xml
struct Mesh {
  // REVIEW: may not be needed
  // NOTE: should point to Scene.vertices
  V3 *vertices;

  u32 *triangles; // NOTE: faces field in xml
  u32 triangle_count;
  
  Material &material;
};

struct Scene {
  u32 max_ray_trace_depth;
  V3 bg_color;
  Camera cam;
  
  AmbientLight *ambient_lights;
  u32 ambient_light_count;
  
  PointLight *point_lights;
  u32 point_light_count;
  
  Material *materials;
  u32 material_count;
  
  V3 *vertices;
  u32 vertex_count;
  
  Mesh *meshes; // NOTE: objects field in xml
  u32 mesh_count;
};
