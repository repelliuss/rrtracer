#include "scene.hpp"

#include <string.h>
#include <stdio.h>

int material_by_id(Material *&material, std::vector<Material> &materials,
                   const char *name) {
  if (!name) {
    fprintf(stderr, "Null material name lookup!\n");
    return -1;
  }

  for(Material &mat : materials) {
    if (strcmp(mat.id, name) == 0) {
      material = &mat;
      return 0;
    }
  }

  return -1;
}
