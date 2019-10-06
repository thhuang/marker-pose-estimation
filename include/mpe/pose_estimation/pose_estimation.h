#ifndef MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_
#define MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_

#include <vector>
#include <string>
#include <opencv2/core/types.hpp>

namespace mpe {

class PoseEstimation {
 private:
  float _marker_size;  // Marker actual size in meter
  cv::Mat _intrinsic_matrix;  // Camera intrinsic matrix
  cv::Mat _distortion_coeff;  // Camera distortion coefficient
  cv::Mat _image;  // Input image
  cv::Mat _image_out;  // Output image
  std::vector<int> _ids;  // Detected marker indices
  std::vector<std::vector<cv::Point2f>> _corners;  // Detected marker edges
  std::vector<cv::Vec3d> _translation, _rotation;  // Pose parameters
  cv::FileStorage fs;  // File storage for estimation output
  
  void _help(const std::string);
  void _read_config();
  void _estimate_pose(const std::string, const std::string);
  void _draw();
  void _output(const std::string);

 public:
  PoseEstimation();
  void estimate(std::string);
};

}  // namespace mpe

#endif  // MPE_POSE_ESTIMATION_POSE_ESTIMATION_H_