#include "utils.h"

#include <string>

namespace utils {

bool IsInt(const std::string &str) {
  for (const char &c : str) {
    if (std::isdigit(c) == 0) return false;
  }
  return true;
};

std::string GetFileNameFromPath(const std::string &file_path) {
  int extension_position = file_path.find('.');
  return file_path.substr(0, extension_position);
};

} // namepsce utils
