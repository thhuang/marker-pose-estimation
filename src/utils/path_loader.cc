#include <experimental/filesystem>
#include "mpe/utils/path_loader.h"

std::vector<std::string> mpe::PathLoader::load() {
  std::vector<std::string> paths;
  for (const auto & entry : std::experimental::filesystem::directory_iterator(path_))
    paths.push_back(entry.path());
  return paths;
}