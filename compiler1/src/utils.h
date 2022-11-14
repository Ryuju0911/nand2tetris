#ifndef COMPILER_UTILS_H_
#define COMPILER_UTILS_H_

#include <string>

namespace utils {

bool IsInt(const std::string &str);
std::string GetFileNameFromPath(const std::string &file_path);

} // namespace utils

#endif
