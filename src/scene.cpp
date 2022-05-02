#include "scene.hpp"

#include <string.h>

int material_by_id(Material *&material, Material *materials, u32 material_count,
                   const char *name) {
  if(!name) return -1;
  
  for (u32 i = 0; i < material_count; ++i) {
    if (strcmp(materials[i].id, name) == 0) {
      material = materials + i;
      return 0;
    }
  }

  return -1;
}
