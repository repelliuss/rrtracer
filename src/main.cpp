#include "file.hpp"
#include "rapidxml/rapidxml.hpp"
#include "scene.hpp"
#include "str.hpp"
#include "xml.hpp"
#include "img.hpp"
#include "ray.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

using namespace rapidxml;

int main(int argc, char *argv[]) {
  int status;
  char *scene_description;
  umax size_scene_description;

  if(argc < 2) {
    fprintf(stderr, "No XML scene path is given as 1st argument!\n");
    return -1;
  }
  else if(argc < 3) {
    fprintf(stderr, "No output image path is given as 2nd argument!\n");
    return -1;
  }

  status = file::size(size_scene_description, argv[1]);
  if (status < 0)
    return status;

  // + 1 to null terminate
  scene_description = static_cast<char *>(malloc(size_scene_description + 1));
  scene_description[size_scene_description] = 0;

  status = file::read(scene_description, argv[1], size_scene_description);
  if (status < 0)
    goto on_err;

  {
    Scene scene;
    status = xml::to_scene(scene, scene_description);

    if(scene.cam.resolution.x == 0) {
      fprintf(stderr, "Camera X resolution is 0\n");
      goto on_err;
    }

    if (scene.cam.resolution.y == 0) {
      fprintf(stderr, "Camera Y resolution is 0!\n");
      goto on_err;
    }

    // TODO: handle according to HW
    int thread_count = 16;

    pthread_t pids[thread_count];
    int y_step = scene.cam.resolution.y / thread_count;
    std::vector<Color> colors[thread_count];

    ray::Input ray_in[thread_count];
    ray::ThreadInput tray_in[thread_count];

    for(int i = 0; i < thread_count; ++i) {
      ray_in[i].scene = &scene;
      if(i != thread_count - 1) {
        ray_in[i].y_range = v2u(i * y_step, i * y_step + y_step);
      } else {
        ray_in[i].y_range = v2u(i * y_step, scene.cam.resolution.y);
      }

      tray_in[i].colors = &colors[i];
      tray_in[i].in = &ray_in[i];

      pthread_create(&pids[i], NULL, ray::threaded_trace, &tray_in[i]);
    }

    std::vector<Color> all_colors;

    for(int i = 0; i < thread_count; ++i) {
      pthread_join(pids[i], NULL);
      all_colors.insert(all_colors.end(), colors[i].begin(), colors[i].end());
    }

    size_t count = all_colors.size();

    img::Input img_in {
      .data = all_colors.data(),
      .count = static_cast<u32>(count),
      .resolution = scene.cam.resolution,
      .output_path = argv[2],
    };

    img::write_to_ppm(img_in);
  }

on_err:
  free(scene_description);
  return status;
}
