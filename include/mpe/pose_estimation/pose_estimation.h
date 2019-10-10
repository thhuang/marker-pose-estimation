#ifndef MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_
#define MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_

#include <vector>
#include <string>
#include <opencv2/core/types.hpp>

namespace mpe {

class PoseEstimation {
 private:
  float marker_size_;  // Marker actual size in meter
  cv::Mat intrinsic_matrix_;  // Camera intrinsic matrix
  cv::Mat distortion_coeff_;  // Camera distortion coefficient
  cv::Mat image_;  // Input image
  cv::Mat image_out_;  // Output image
  std::vector<int> ids_;  // Detected marker indices
  std::vector<std::vector<cv::Point2f>> corners_;  // Detected marker edges
  std::vector<cv::Vec3d> translation_, rotation_;  // Pose parameters
  cv::FileStorage fs;  // File storage for estimation output
  
  void help(const std::string);
  void read_config();
  void estimate_pose(const std::string, const std::string);
  void draw();
  void output(const std::string);

 public:
  PoseEstimation();
  void estimate(std::string);
};

}  // namespace mpe

#endif  // MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_