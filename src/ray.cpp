#include "ray.hpp"
#include "log.hpp"

#include <assert.h>
#include <cmath>
#include <limits>

namespace ray {

constexpr f32 shadow_epsilon = 1e-6;
constexpr f32 intersect_epsilon = 1e-2;

template <class T> constexpr T safe_div_or_0(T num, f32 denom) {
  if (denom != 0.0f) {
    return num * (1.0f / denom);
  }

  assert(false && "no safe division");

  return num * 0;
}

inline V3 barycentric(const Point3 &p, const TriangleFace &tri) {
  V3 ab = tri.b - tri.a;
  V3 ac = tri.c - tri.a;
  V3 ap = p - tri.a;

  f32 d_b_b = dot(ab, ab);
  f32 d_b_c = dot(ab, ac);
  f32 d_c_c = dot(ac, ac);
  f32 d_p_b = dot(ap, ab);
  f32 d_p_c = dot(ap, ac);

  f32 inv_denom = 1.0f / (d_b_b * d_c_c - d_b_c * d_b_c);

  V3 result;

  result.v = (d_c_c * d_p_b - d_b_c * d_p_c) * inv_denom;
  result.w = (d_b_b * d_p_c - d_b_c * d_p_b) * inv_denom;
  result.u = 1 - result.v - result.w;

  return result;
}

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

  // TODO: maybe check if parallel to plane?

  f32 A = determinant(ba, ca, ray.direction);
  if(A == 0.0f)
    return std::numeric_limits<f32>::max();

  f32 beta = determinant(oa, ca, ray.direction) / A;
  f32 gamma = determinant(ba, oa, ray.direction) / A;

  if(beta + gamma <= 1 && 0 <= beta && 0 <= gamma) {
    f32 t = determinant(ba, ca, oa) / A;
    if(t > intersect_epsilon)
      return t;
  }

  return std::numeric_limits<f32>::max();
}

// REVIEW: no inversely square drop?
constexpr Color ambient(const AmbientLight &light, const Material *mat) {
  return light.color * mat->ambient;
}

constexpr f32 max(f32 a, f32 b) { return a > b ? a : b; }

constexpr f32 clamp_max(f32 v, f32 max) { return v > max ? max : v; }

constexpr V3 clamp_max(V3 v, f32 max) {
  return v3(clamp_max(v.x, max), clamp_max(v.y, max), clamp_max(v.z, max));
}

struct LightHitSpec {
  const Material *mat;
  const V3 *normal;
  const V3 *wo;
  const V3 *wi;
};

constexpr Color diffuse(const LightHitSpec &in) {
  const Material *mat = in.mat;
  const V3 &normal = *in.normal;
  const V3 &wi = *in.wi;

  V3 val = mat->diffuse * max(0, dot(norm(wi), norm(normal)));
  return val;
}

constexpr Color specular(const LightHitSpec &in) {
  const Material *mat = in.mat;
  const V3 &normal = *in.normal;
  const V3 &wo = *in.wo;
  const V3 &wi = *in.wi;

  V3 sum = wo + wi;
  V3 h = safe_div_or_0(sum, length(sum));

  return mat->specular * pow(max(0, dot(norm(normal), h)), mat->phong);
}

constexpr int in_shadow(const Ray &shadow_ray, f32 light_dist,
                        const Scene &scene) {
  f32 t_max = std::numeric_limits<f32>::max();

  for (u32 i = 0; i < scene.mesh_count; ++i) {
    const Mesh *cur_mesh = &scene.meshes[i];

    for (u32 j = 0; j < cur_mesh->face_count; ++j) {
      const TriangleFace &cur_face = cur_mesh->faces[j];
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

struct HitData {
  const Point3 pos;
  const Material *material;
  const V3 normal;
  const V3 wo;
};

constexpr Color hit_color(const HitData &hit, LightHitSpec &light_spec,
                          const Scene &scene) {
  Color c = v3(0, 0, 0);

  for (u32 i = 0; i < scene.point_light_count; ++i) {
    const PointLight &light = scene.point_lights[i];
    const V3 wi = light.pos - hit.pos;
    const Ray shadow_ray = {
        .origin = hit.pos + wi * shadow_epsilon,
        .direction = wi,
    };

    const f32 light_dist = length(wi);

    if (in_shadow(shadow_ray, light_dist, scene))
      continue;

    const V3 irradiance =
        safe_div_or_0(light.intensity, light_dist * light_dist);

    light_spec.wi = &wi;
    c += (diffuse(light_spec) + specular(light_spec)) * irradiance;
  }

  return c;
}

void *threaded_trace(void *arg) {
  ThreadInput *tin = static_cast<ThreadInput *>(arg);
  trace(tin->colors, tin->in);

  return 0;
}

int trace(std::vector<Color> *colors, Input *in) {
  const Scene &scene = *in->scene;
  const Camera &cam = scene.cam;

  V2u resolution = cam.resolution;
  // TODO: to input, doesn't depend on anything here
  Plane near_plane = near_plane_of_cam(cam);
  V2u pixel = v2u(0, in->y_range.beg);

  for (; pixel.y < in->y_range.end; ++pixel.y) {
    for (pixel.x = 0; pixel.x < resolution.x; ++pixel.x) {
      std::vector<HitData> hits;
      Ray ray = ray_between(cam.pos, pixel_on_plane(pixel, near_plane));

      for (u32 depth = 0; depth <= scene.max_ray_trace_depth; ++depth) {
        f32 t_min = std::numeric_limits<f32>::max();
        const Mesh *hit = nullptr;
        V3 hit_normal;

        for (u32 i = 0; i < scene.mesh_count; ++i) {
          const Mesh *cur_mesh = &scene.meshes[i];

          for (u32 j = 0; j < cur_mesh->face_count; ++j) {
            const TriangleFace &cur_face = cur_mesh->faces[j];
            f32 t_intersect = intersects_at(ray, cur_face);

            if (t_intersect < t_min) {
              t_min = t_intersect;
              hit = cur_mesh;
              hit_normal = cur_face.normal();
            }
          }
        }

        if (!hit)
          break;

        HitData hit_data = {
            .pos = ray.at(t_min),
            .material = hit->material,
            .normal = hit_normal,
            .wo = -ray.direction,
        };
        hits.push_back(hit_data);

        ray.origin = hit_data.pos;

        ray.direction =
            2 * dot(norm(hit_data.wo), norm(hit_normal)) * norm(hit_normal) - norm(hit_data.wo);
      }

      if (!hits.empty()) {
        // light calculations
        const HitData &initial_hit = hits.front();
        Color color = v3(0, 0, 0);

        for (u32 i = 0; i < scene.ambient_light_count; ++i) {
          color += ambient(scene.ambient_lights[i], initial_hit.material);
        }

        V3 wo = initial_hit.wo;
        LightHitSpec light_spec = {
            .mat = initial_hit.material,
            .normal = &initial_hit.normal,
            .wo = &wo,
            .wi = nullptr,
        };

        color += hit_color(initial_hit, light_spec, scene);

        Color reflected = v3(0, 0, 0);
        for (u32 i = 1; i < hits.size(); ++i) {
          light_spec.mat = hits[i].material;
          light_spec.normal = &hits[i].normal;
          light_spec.wo = &wo;
          reflected += hit_color(hits[i], light_spec, scene);
        }

        color += reflected * initial_hit.material->reflactance;

        color = clamp_max(color, 255);

        colors->push_back(color);
      } else {
        colors->push_back(scene.bg_color);
      }
    }
  }

  return 0;
}
} // namespace ray
