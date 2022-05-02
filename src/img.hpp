#pragma once

#include "types.hpp"
#include "vector.hpp"
#include <filesystem>

namespace img {
  struct Input {
    V3 *data;
    u32 count;
    V2u resolution;
    std::filesystem::path output_path;
  };
  
  int write_to_ppm(Input input);
}
