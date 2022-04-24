#pragma once

#include "types.hpp"

#include <filesystem>

namespace file {
int read(char *&out, const std::filesystem::path path, umax size);
int size(umax &size, const std::filesystem::path path);
} // namespace file
