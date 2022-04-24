#include "file.hpp"

#include <stdio.h>

int file::read(char *&out, const std::filesystem::path path, umax size) {
  int status = 0;
  FILE *fp = std::fopen(path.c_str(), "r");

  if (!fp) {
    fprintf(stderr, "Failed to open file %s\n", path.c_str());
    status = -1;
  } else if (std::fread(out, 1, size, fp) != size) {
    fprintf(stderr, "Failed to read file %s\n", path.c_str());
    status = -1;
  }

  std::fclose(fp);

  return status;
}

int file::size(umax &size, std::filesystem::path path) {
  std::error_code ec;
  size = std::filesystem::file_size(path, ec);
  if (ec) {
    fprintf(stderr, "Failed to read file size of file %s : %s\n", path.c_str(),
            ec.message().c_str());
    return -1;
  }

  return 0;
}
