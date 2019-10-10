#ifndef MPE_UTILS_PATH_LOADER_H_
#define MPE_UTILS_PATH_LOADER_H_

#include <opencv2/aruco.hpp>
#include <opencv2/core/types.hpp>
#include <string>
#include <vector>

namespace mpe {

class PathLoader {
 private:
  std::string path_;
  PathLoader();

 public:
  PathLoader(std::string path) : path_{path} {};
  std::vector<std::string> load();
};

}  // namespace mpe

#endif  // MPE_UTILS_PATH_LOADER_H_