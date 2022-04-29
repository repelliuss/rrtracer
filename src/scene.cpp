#include "scene.hpp"

int material_by_id(Material *&material, Material *materials, u32 material_count,
                   i32 id) {
  for (u32 i = 0; i < material_count; ++i) {
    if(materials[i].id == id) {
      material = materials + i;
      return 0;
    }
  }

  return -1;
}
