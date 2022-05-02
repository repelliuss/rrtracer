#include "ray.hpp"
#include "log.hpp"

#include <assert.h>
#include <cmath>
#include <limits>

namespace ray {

struct HitData {
  Point3 pos;
  Material *material;
  V3 normal;
  V3 wo;
};

namespace constant {
constexpr f32 shadow_epsilon = 1e-4;
constexpr f32 intersect_epsilon = 1e-4;
constexpr f32 max_float = std::numeric_limits<f32>::max();
} // namespace constant

constexpr f32 determinant(const V3 &col0, const V3 &col1, const V3 &col2) {
  f32 term1 = col0.x * (col1.y * col2.z - col2.y * col1.z);
  f32 term2 = col0.y * (col2.x * col1.z - col1.x * col2.z);
  f32 term3 = col0.z * (col1.x * col2.y - col1.y * col2.x);

  return term1 + term2 + term3;
}

constexpr f32 intersects_at(const Ray &ray, const TriangleFace &tri) {
  V3 oa = tri.a - ray.origin;
  V3 ba = tri.a - tri.b;
  V3 ca = tri.a - tri.c;


  f32 A = determinant(ba, ca, ray.direction);
  if (A == 0.0f)
    return constant::max_float;

  f32 beta = determinant(oa, ca, ray.direction) / A;
  f32 gamma = determinant(ba, oa, ray.direction) / A;

  if (beta + gamma <= 1 && 0 <= beta && 0 <= gamma) {
    f32 t = determinant(ba, ca, oa) / A;
    if (t > constant::intersect_epsilon)
      return t;
  }

  return constant::max_float;
}

constexpr f32 max(f32 a, f32 b) { return a > b ? a : b; }

constexpr f32 clamp_max(f32 v, f32 max) { return v > max ? max : v; }

constexpr V3 clamp_max(V3 v, f32 max) {
  return v3(clamp_max(v.x, max), clamp_max(v.y, max), clamp_max(v.z, max));
}

constexpr Color diffuse(const HitData &hit, const V3 &wi) {
  return hit.material->diffuse * max(0, dot(wi, hit.normal));
}

constexpr Color ambient(const AmbientLight &light, const Material *mat) {
  return light.color * mat->ambient;
}

constexpr Color specular(const HitData &hit, const V3 &wi) {
  V3 h = norm(hit.wo + wi);
  return hit.material->specular *
         pow(max(0, dot(hit.normal, h)), hit.material->phong);
}

inline int in_shadow(const Ray &shadow_ray, f32 light_dist,
                     const Scene &scene) {
  f32 t_max = constant::max_float;

  for (const Mesh &cur_mesh : scene.meshes) {
    for (const TriangleFace &cur_face : cur_mesh.faces) {
      f32 t_intersect = intersects_at(shadow_ray, cur_face);

      if (t_intersect < t_max) {
        f32 obj_dist = length(shadow_ray.at(t_intersect) - shadow_ray.origin);

        if (obj_dist < light_dist)
          return 1;
      }
    }
  }

  return 0;
}

constexpr Color hit_color(const HitData *hits, u32 hits_size,
                          const Scene &scene) {
  Color next_color = v3(0, 0, 0);

  for (i32 hi = hits_size - 1; hi >= 0; --hi) {
    Color cur_color = v3(0, 0, 0);
    const HitData &hit = hits[hi];

    for(const AmbientLight &light : scene.ambient_lights) {
      cur_color += ambient(light, hit.material);
    }

    for(const PointLight &light : scene.point_lights) {
      const V3 wi = light.pos - hit.pos;
      const f32 light_dist = length(wi);
      const V3 norm_wi = norm(wi);

      const Ray shadow_ray = {
          .origin = hit.pos + norm_wi * constant::shadow_epsilon,
          .direction = norm_wi,
      };

      if (in_shadow(shadow_ray, light_dist, scene))
        continue;

      const V3 irradiance =
          light.intensity * (1.0f / (light_dist * light_dist));

      cur_color +=
          (diffuse(hit, norm_wi) + specular(hit, norm_wi)) * irradiance;
    }

    next_color = cur_color + next_color * hit.material->reflactance;
  }

  return next_color;
}

void *threaded_trace(void *arg) {
  ThreadInput *tin = static_cast<ThreadInput *>(arg);
  trace(tin->colors, tin->in);

  return 0;
}

int trace(std::vector<Color> *colors, Input *in) {
  const Scene &scene = *in->scene;
  const Camera &cam = scene.cam;
  const V2u &resolution = cam.resolution;
  const Plane near_plane = near_plane_of_cam(cam);
  
  V2u pixel = v2u(0, in->y_range.beg);

  for (; pixel.y < in->y_range.end; ++pixel.y) {
    for (pixel.x = 0; pixel.x < resolution.x; ++pixel.x) {

      HitData hits[scene.max_ray_trace_depth + 1];
      u32 hits_size = 0;
      Ray ray = ray_between(cam.pos, pixel_on_plane(pixel, near_plane));

      for (u32 depth = 0; depth <= scene.max_ray_trace_depth; ++depth) {
        f32 t_min = constant::max_float;
        const Mesh *hit = nullptr;
        V3 hit_normal;

	for(const Mesh &cur_mesh : scene.meshes) {
	  for(const TriangleFace &cur_face : cur_mesh.faces) {
            f32 t_intersect = intersects_at(ray, cur_face);

            if (t_intersect < t_min) {
              t_min = t_intersect;
              hit = &cur_mesh;
              hit_normal = cur_face.normal();
            }
          }
        }

        if (!hit)
          break;

        hits[depth].pos = ray.at(t_min);
        hits[depth].material = hit->material;
        hits[depth].normal = norm(hit_normal);
        hits[depth].wo = norm(-ray.direction);
        ++hits_size;

        if (length(hits[depth].material->reflactance) <= constant::shadow_epsilon)
          break;

        ray.direction =
            2 * dot(hits[depth].wo, hits[depth].normal) * hits[depth].normal -
            hits[depth].wo;
        ray.origin =
            hits[depth].pos + ray.direction * constant::intersect_epsilon;
      }

      if (hits_size > 0) {
        colors->push_back(clamp_max(hit_color(hits, hits_size, scene), 255));
      } else {
        colors->push_back(scene.bg_color);
      }
    }
  }

  return 0;
}
} // namespace ray
