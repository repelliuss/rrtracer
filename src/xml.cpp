#include "xml.hpp"
#include "str.hpp"

#include "rapidxml/rapidxml.hpp"

#include <cstring>
#include <stdio.h>

using namespace rapidxml;

// TODO: can be implemented incrementally, be careful about meshes as they need
// material ref

// TODO: make unit parsers and a generic array?

// NOTE: Parsed arrays' capacity are fixed

// TODO: not everyone should print node not found error, like lights

namespace xml {

constexpr const char *fmt_node_not_found =
    "Failed to find '%s' node in description XML\n";

constexpr const char *fmt_bad_format =
  "Failed to parse '%s' node in XML\n";

constexpr const char *fmt_bad_value =
  "Failed to get value of '%s' node in XML\n";

// TODO: should take node_name size optionally and pass to first_node()
xml_node<> *first_node(const xml_node<> *parent, const char *node_name) {
  xml_node<> *node = parent->first_node(node_name);

  if (node == nullptr) {
    fprintf(stderr, fmt_node_not_found, node_name);
  }

  return node;
}

char *first_node_value(const xml_node<> *parent, const char *node_name) {
  xml_node<> *node = first_node(parent, node_name);

  if (node)
    return node->value();

  fprintf(stderr, fmt_bad_value, node_name);

  return nullptr;
}

template <class T>
constexpr int node_to_vector(T &vector, const xml_node<> *parent,
                             const char *vector_node_name) {
  const char *val = first_node_value(parent, vector_node_name);

  if (val == nullptr) {
    return -1;
  }

  return str::to_vector(vector, val);
}

template <class T>
constexpr int node_to_integral(T &integral, const xml_node<> *parent,
                               const char *integral_node_name) {
  const char *val = first_node_value(parent, integral_node_name);

  if (val == nullptr) {
    return -1;
  }

  return str::to_integral(integral, val);
}

int node_to_camera(Camera &cam, const xml_node<> *parent,
                   const char *cam_node_name) {
  xml_node<> *cam_root = first_node(parent, cam_node_name);
  int status = 0;

  if (cam_root == nullptr)
    return -1;

  status |= node_to_vector(cam.pos, cam_root, "position");
  status |= node_to_vector(cam.gaze, cam_root, "gaze");
  status |= node_to_vector(cam.up, cam_root, "up");
  status |= node_to_vector(cam.near_plane, cam_root, "nearplane");
  status |= node_to_integral(cam.near_dist, cam_root, "neardistance");
  status |= node_to_vector(cam.resolution, cam_root, "imageresolution");

  cam.orientation.v = cam.up;
  cam.orientation.w = -cam.gaze;
  cam.orientation.u = cross(cam.orientation.v, cam.orientation.w);

  if (status < 0) {
    fprintf(stderr, fmt_bad_format, "camera");
    return -1;
  }

  return 0;
}

// NOTE: It is not an error if there is no light in the scene.
int node_to_ambient_lights(AmbientLight *lights, u32 &count,
                           const xml_node<> *parent,
                           const char *lights_node_name) {
  xml_node<> *lights_root = first_node(parent, lights_node_name);

  if (lights_root == nullptr)
    return 0;

  count = 0;

  for (xml_node<> *ambient_light_node = first_node(lights_root, "ambientlight");
       ambient_light_node != nullptr;
       ambient_light_node = ambient_light_node->next_sibling("ambientlight")) {

    int status =
        str::to_vector(lights[count].color, ambient_light_node->value());

    if (status < 0) {
      fprintf(stderr, fmt_bad_format, "ambientlight");
      return status;
    }

    ++count;
  }

  return 0;
}

// NOTE: It is not an error if there is no light in the scene.
int node_to_point_lights(PointLight *lights, u32 &count,
                         const xml_node<> *parent,
                         const char *lights_node_name) {
  xml_node<> *lights_root = first_node(parent, lights_node_name);

  if (lights_root == nullptr)
    return 0;

  count = 0;

  for (xml_node<> *point_light_node = first_node(lights_root, "pointlight");
       point_light_node != nullptr;
       point_light_node = point_light_node->next_sibling("pointlight")) {

    int status = 0;

    status |= node_to_vector(lights[count].pos, point_light_node, "position");
    status |=
        node_to_vector(lights[count].intensity, point_light_node, "intensity");

    if (status < 0) {
      fprintf(stderr, fmt_bad_format, "pointlight");
      return status;
    }

    ++count;
  }

  return 0;
}

// NOTE: It is not an error if there is no material in the scene.
int node_to_materials(Material *materials, u32 &count, const xml_node<> *parent,
                      const char *materials_node_name) {
  xml_node<> *material_roots = first_node(parent, materials_node_name);

  if (material_roots == nullptr)
    return 0;

  count = 0;

  for (xml_node<> *material_node = first_node(material_roots, "material");
       material_node != nullptr;
       material_node = material_node->next_sibling("material")) {
    int status = 0;
    Material &material = materials[count];
    const xml_attribute<> *attr = material_node->first_attribute("id");
    const char *id = attr->value();
    const u32 id_size = attr->value_size();

    // TODO: remove 255
    if(id == nullptr || id_size > 254) goto on_err;

    strncpy(material.id, id, id_size);
    material.id[255] = 0;

    
    status |= node_to_vector(material.ambient, material_node, "ambient");
    status |= node_to_vector(material.diffuse, material_node, "diffuse");
    status |= node_to_vector(material.specular, material_node, "specular");
    status |= node_to_integral(material.phong, material_node, "phongexponent");
    status |= node_to_vector(material.reflactance, material_node,
                             "mirrorreflectance");

    if (status < 0) {
    on_err:
      fprintf(stderr, fmt_bad_format, "material");
      return status;
    }

    ++count;
  }

  return 0;
}

template <class T>
int node_to_array(T *arr, u32 &count, const xml_node<> *parent,
                  const char *arr_node_name, u32 max_capacity) {
  xml_node<> *node = first_node(parent, arr_node_name);

  if (node == nullptr)
    return 0;

  const char *val = node->value();

  if (val == nullptr)
    return 0;

  return str::to_array(arr, count, val, node->value_size(), max_capacity);
}

template <class T>
int node_to_vertices(T *vertices, u32 &vertex_count, const xml_node<> *parent,
                     const char *vertices_node_name) {
  u32 integral_count;
  int status;

  status = node_to_array(reinterpret_cast<f32 *>(vertices), integral_count,
                         parent, vertices_node_name, Scene::vertex_capacity * 3);

  vertex_count = integral_count / 3;

  return status;
}

// NOTE: It is not an error if there is no material in the scene.
int node_to_scene_meshes(Scene &scene, const xml_node<> *parent,
                         const char *meshes_node_name) {
  xml_node<> *meshes_roots = first_node(parent, meshes_node_name);

  if (meshes_roots == nullptr)
    return 0;

  scene.mesh_count = 0;

  for (xml_node<> *mesh_node = first_node(meshes_roots, "mesh");
       mesh_node != nullptr; mesh_node = mesh_node->next_sibling("mesh")) {
    int status = 0;
    Mesh &mesh = scene.meshes[scene.mesh_count];

    status |=
        material_by_id(mesh.material, scene.materials, scene.material_count,
                       first_node_value(mesh_node, "materialid"));
    status |= node_to_array(mesh.triangles, mesh.triangle_count, mesh_node,
                            "faces", Scene::vertex_capacity * 3);

    if (status < 0) {
      fprintf(stderr, fmt_bad_format, "mesh");
      return status;
    }

    // NOTE: also remapping 1 indexed faces to 0 index
    mesh.face_count = mesh.triangle_count / 3;
    for (u32 i = 0; i < mesh.face_count; ++i) {
      mesh.faces[i].vertices[0] = scene.vertices[mesh.triangles[i * 3] - 1];
      mesh.faces[i].vertices[1] = scene.vertices[mesh.triangles[i * 3 + 1] - 1];
      mesh.faces[i].vertices[2] = scene.vertices[mesh.triangles[i * 3 + 2] - 1];
    }

    ++scene.mesh_count;
  }

  return 0;
}

/// Element order is not important.
int to_scene(Scene &scene, char *xml) {
  xml_document<> doc;
  xml_node<> *root;
  int status = 0;

  {
    doc.parse<0>(xml); // may throw

    root = doc.first_node("scene");
    if (root == nullptr) {
      fprintf(stderr, fmt_node_not_found, "scene");
      return -1;
    }
  }

  status |=
      node_to_integral(scene.max_ray_trace_depth, root, "maxraytracedepth");
  status |= node_to_vector(scene.bg_color, root, "background");
  status |= node_to_camera(scene.cam, root, "camera");
  status |= node_to_ambient_lights(scene.ambient_lights,
                                   scene.ambient_light_count, root, "lights");
  status |= node_to_point_lights(scene.point_lights, scene.point_light_count,
                                 root, "lights");
  status |= node_to_materials(scene.materials, scene.material_count, root,
                              "materials");
  status |=
      node_to_vertices(scene.vertices, scene.vertex_count, root, "vertexdata");
  status |= node_to_scene_meshes(scene, root, "objects");

  if (status < 0)
    fprintf(stderr, fmt_bad_format, "scene");

  return status;
}

} // namespace xml
