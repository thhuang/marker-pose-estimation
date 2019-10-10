#ifndef MPE_UTILS_MARKER_DETECT_H_
#define MPE_UTILS_MARKER_DETECT_H_

#include <opencv2/aruco.hpp>
#include <opencv2/core/types.hpp>
#include <vector>

namespace mpe {

class MarkerDetector {
 private:
  cv::Ptr<cv::aruco::Dictionary> dictionary_;

 public:
  MarkerDetector();
  void detect(cv::Mat, std::vector<std::vector<cv::Point2f>>&,
              std::vector<int>&);
};

}  // namespace mpe

#endif  // MPE_UTILS_MARKER_DETECT_H_