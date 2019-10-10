#include <experimental/filesystem>
#include "mpe/utils/path_loader.h"
using namespace std;
using namespace std::experimental::filesystem;
using namespace mpe;

vector<string> PathLoader::load() {
  vector<string> paths;
  for (const auto & entry : directory_iterator(path_))
    paths.push_back(entry.path());
  return paths;
}