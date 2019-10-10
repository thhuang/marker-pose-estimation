#include <iostream>
#include <sstream>
#include <map>
#include <experimental/filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <Eigen/Geometry>
#include "mpe/pose_estimation/pose_estimation.h"
#include "mpe/pose_estimation/marker_pose.h"
#include "mpe/utils/marker_detector.h"
#include "mpe/utils/path_loader.h"
#include "mpe/utils/draw_cube.h"
// using namespace std;
// using namespace std::experimental::filesystem;
// using namespace cv;
// using namespace cv::aruco;
// using namespace mpe;
// using namespace Eigen;

mpe::PoseEstimation::PoseEstimation() 
 : fs{"/output/estimation.yml", cv::FileStorage::WRITE} {
  read_config();
}

void mpe::PoseEstimation::help(const std::string images_dir) {
  std::cout << "\nPose Estimation\n" << std::endl;
  std::cout << "This is a marker pose estimation program." << std::endl;
  std::cout << "Usage: pose_estimation [images_dir]" << std::endl
            << "(default: ./images --> The program will process every file in input/images/)\n" << std::endl;
}

void mpe::PoseEstimation::read_config() {
  std::cout << "\nReading parameters from input/config.yml" << std::endl;
  cv::FileStorage fs("/input/config.yml", cv::FileStorage::READ);
  marker_size_ = fs["marker_size_meter"];
  intrinsic_matrix_ = (cv::Mat_<double>(3, 3) << 
    fs["camera_intrinsic"]["fx"],                            0, fs["camera_intrinsic"]["cx"], 
                               0, fs["camera_intrinsic"]["fy"], fs["camera_intrinsic"]["cy"], 
                               0,                            0,                            1);
  distortion_coeff_ = (cv::Mat_<double>(5, 1) << fs["distortion_coeff"]["k1"], 
                                             fs["distortion_coeff"]["k2"],
                                             fs["distortion_coeff"]["p1"], 
                                             fs["distortion_coeff"]["p2"], 
                                             fs["distortion_coeff"]["k3"]);
  std::cout << "\nMarker size in meter: " << marker_size_ << std::endl;
  std::cout << "\nCamera intrinsic matrix: \n" << intrinsic_matrix_ << std::endl;
  std::cout << "\nCamera distortion coefficient \n" << distortion_coeff_ << std::endl;
}

void mpe::PoseEstimation::draw() {
  for (int i = 0; i < ids_.size(); i++) {
    draw_cube(image_out_, ids_[i], intrinsic_matrix_, distortion_coeff_, 
              rotation_[i], translation_[i], marker_size_);
  }
}

void mpe::PoseEstimation::output(const std::string output_path) {
  // Output image
  imwrite(output_path, image_out_);

  // Output pose
  std::map<int, MarkerPose> results;
  for (int i = 0; i < ids_.size(); ++i) {
    Eigen::Vector3d rotation(rotation_[i][0], rotation_[i][1], rotation_[i][2]);
    double theta = rotation.norm();
    Eigen::Quaterniond q(Eigen::AngleAxisd(theta, rotation / theta));
    std::stringstream ss;
    ss << "  Marker " << ids_[i] << '\n';
    ss << "    translation (x, y, z) = (";
    ss << translation_[i][0] << ", ";
    ss << translation_[i][1] << ", ";
    ss << translation_[i][2] << ")\n";
    ss << "    rotation (w, x, y, z) = (";
    ss << q.w() << ", ";
    ss << q.vec()[0] << ", ";
    ss << q.vec()[1] << ", ";
    ss << q.vec()[2] << ")" << std::endl;
    results[ids_[i]] = MarkerPose(translation_[i], q, ss.str());
  }
  // Output result (sorted according to the index of the marker)
  for (auto result : results) {
    std::stringstream marker_name;
    marker_name << "marker_" << result.first;
    std::cout << result.second.text;
    fs << marker_name.str() << "{";
    fs <<   "translation" << "{";
    fs <<     "x" << result.second.x;
    fs <<     "y" << result.second.y;
    fs <<     "z" << result.second.z;
    fs <<   "}";
    fs <<   "rotation" << "{";
    fs <<     "w" << result.second.qw;
    fs <<     "x" << result.second.qx;
    fs <<     "y" << result.second.qy;
    fs <<     "z" << result.second.qz;
    fs <<   "}";
    fs << "}";
  }

  std::cout << std::endl;
}

void mpe::PoseEstimation::estimate_pose(const std::string image_path, const std::string output_path) {
  // Read the image
  image_ = cv::imread(image_path);
  image_.copyTo(image_out_);

  // Detect markers
  MarkerDetector().detect(image_, corners_, ids_);

  if (ids_.size()) {
    // Estimate pose
    cv::aruco::estimatePoseSingleMarkers(corners_, marker_size_, intrinsic_matrix_, 
                                         distortion_coeff_, rotation_, translation_);

    // Draw axes for each detected marker
    draw();
  }
  
  // Output
  output(output_path);

}

void mpe::PoseEstimation::estimate(const std::string images_dir) {
  help(images_dir);

  PathLoader path_loader(images_dir);
  
  for (auto image_path : path_loader.load()) {
    std::string filename = std::experimental::filesystem::path(image_path).filename();
    std::string output_path = "/output/" + filename;
    std::cout << "Processing " << filename
         << " --> " << output_path.substr(1) << std::endl;

    fs << filename.substr(0, filename.find(".")) << "{";

    try {
      estimate_pose(image_path, output_path);
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      continue;
    }
    
    fs << "}";
  }

  fs.release();

  std::cout << "Done!" << std::endl;
}

int main(int argc, char* argv[]) {

  // Path to the directory containing input images
  const std::string images_dir = argc > 1 ? argv[1] : "/input/images";
  mpe::PoseEstimation().estimate(images_dir);

  return 0;
}