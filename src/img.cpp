#include "img.hpp"

#include <string>
#include <fstream>

namespace img {
  // TODO: refactor to use file utils
  int write_to_ppm(Input in) {
    std::ofstream fs(in.output_path);

    fs << "P3\n" << in.resolution.x << ' ' << in.resolution.y << "\n255\n";

    for(u32 i = 0; i < in.resolution.y; ++i) {
      for(u32 j = 0; j < in.resolution.x; ++j) {
	V3 vec = in.data[j + (i * in.resolution.x)];
	v3u uvec(vec);

	fs << uvec.r << ' ' << uvec.g << ' ' << uvec.b << '\n';
      }
    }

    return 0;
  }
}
