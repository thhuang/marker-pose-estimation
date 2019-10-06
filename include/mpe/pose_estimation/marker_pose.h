#ifndef MPE_POSE_ESTIMATION_MARKER_POSE_H
#define MPE_POSE_ESTIMATION_MARKER_POSE_H

#include <string>
#include <opencv2/core/types.hpp>
#include <Eigen/Geometry>

namespace mpe {

struct MarkerPose {
  double x;
  double y;
  double z;
  double qw;
  double qx;
  double qy;
  double qz;
  std::string text;
  MarkerPose() = default;
  MarkerPose(cv::Vec3d t, Eigen::Quaterniond q, std::string text)
   : x{t[0]}, y{t[1]}, z{t[2]},
     qw{q.w()}, qx{q.vec()[0]}, qy{q.vec()[1]}, qz{q.vec()[2]},
     text{text} {}
};

}  // namespace mpe

#endif  // MPE_POSE_ESTIMATION_MARKER_POSE_H