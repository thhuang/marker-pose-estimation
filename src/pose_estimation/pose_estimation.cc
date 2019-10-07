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

using namespace std;
using namespace std::experimental::filesystem;
using namespace cv;
using namespace cv::aruco;
using namespace mpe;
using namespace Eigen;

AngleAxisd rvec_cv_to_eigen(Vec3d);

void draw_cube(cv::InputOutputArray, int ,cv::InputArray, 
                       cv::InputArray, cv::InputArray, cv::InputArray, float);

PoseEstimation::PoseEstimation() 
 : fs{"/output/estimation.yml", FileStorage::WRITE} {
  _read_config();
}

void PoseEstimation::_help(const string images_dir) {
  cout << "\nPose Estimation\n" << endl;
  cout << "This is a marker pose estimation program." << endl;
  cout << "Usage: pose_estimation [images_dir]" << endl
       << "(default: ./images --> The program will process every file in input/images/)\n" << endl;
}

void PoseEstimation::_read_config() {
  cout << "\nReading parameters from input/config.yml" << endl;
  cv::FileStorage fs("/input/config.yml", cv::FileStorage::READ);
  _marker_size = fs["marker_size_meter"];
  _intrinsic_matrix = (Mat_<double>(3, 3) << 
    fs["camera_intrinsic"]["fx"],                            0, fs["camera_intrinsic"]["cx"], 
                               0, fs["camera_intrinsic"]["fy"], fs["camera_intrinsic"]["cy"], 
                               0,                            0,                            1);
  _distortion_coeff = (Mat_<double>(5, 1) << fs["distortion_coeff"]["k1"], 
                                            fs["distortion_coeff"]["k2"],
                                            fs["distortion_coeff"]["p1"], 
                                            fs["distortion_coeff"]["p2"], 
                                            fs["distortion_coeff"]["k3"]);
  cout << "\nMarker size in meter: " << _marker_size << endl;
  cout << "\nCamera intrinsic matrix: \n" << _intrinsic_matrix << endl;
  cout << "\nCamera distortion coefficient \n" << _distortion_coeff << endl;
}

void PoseEstimation::_draw() {
  for (int i = 0; i < _ids.size(); i++) {
    draw_cube(_image_out, _ids[i], _intrinsic_matrix, _distortion_coeff, 
                      _rotation[i], _translation[i], _marker_size);
  }
}

void PoseEstimation::_output(const string output_path) {
  // Output image
  imwrite(output_path, _image_out);

  // Output pose
  map<int, MarkerPose> results;
  for (int i = 0; i < _ids.size(); ++i) {
    Quaterniond q(rvec_cv_to_eigen(_rotation[i]));
    stringstream ss;
    ss << "  Marker " << _ids[i] << '\n';
    ss << "    translation (x, y, z) = (";
    ss << _translation[i][0] << ", ";
    ss << _translation[i][1] << ", ";
    ss << _translation[i][2] << ")\n";
    ss << "    rotation (w, x, y, z) = (";
    ss << q.w() << ", ";
    ss << q.vec()[0] << ", ";
    ss << q.vec()[1] << ", ";
    ss << q.vec()[2] << ")" << endl;
    results[_ids[i]] = MarkerPose(_translation[i], q, ss.str());
  }
  // Output result (sorted according to the index of the marker)
  for (auto result : results) {
    stringstream marker_name;
    marker_name << "marker_" << result.first;
    cout << result.second.text;
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

  cout << endl;
}

void PoseEstimation::_estimate_pose(const string image_path, const string output_path) {
  // Read the image
  _image = imread(image_path);
  _image.copyTo(_image_out);

  // Detect markers
  MarkerDetector().detect(_image, _corners, _ids);

  if (_ids.size()) {
    // Estimate pose
    estimatePoseSingleMarkers(_corners, _marker_size, _intrinsic_matrix, 
                              _distortion_coeff, _rotation, _translation);

    // Draw axes for each detected marker
    _draw();
  }
  
  // Output
  _output(output_path);

}

void PoseEstimation::estimate(const string images_dir) {
  _help(images_dir);

  PathLoader path_loader(images_dir);
  
  for (auto image_path : path_loader.load()) {
    string filename = path(image_path).filename();
    string output_path = "/output/" + filename;
    cout << "Processing " << filename
         << " --> " << output_path.substr(1) << endl;

    fs << filename.substr(0, filename.find(".")) << "{";

    try {
      _estimate_pose(image_path, output_path);
    } catch (const exception& e) {
      cerr << e.what() << endl;
      continue;
    }
    
    fs << "}";
  }

  fs.release();

  cout << "Done!" << endl;
}

int main(int argc, char* argv[]) {

  // Path to the directory containing input images
  const string images_dir = argc > 1 ? argv[1] : "/input/images";
  PoseEstimation().estimate(images_dir);

  return 0;
}

AngleAxisd rvec_cv_to_eigen(Vec3d rvec_cv) {
  Vector3d rotation(rvec_cv[0], rvec_cv[1], rvec_cv[2]);
  double theta = rotation.norm();
  return AngleAxisd(theta, rotation / theta);
}

void draw_cube(InputOutputArray image, int index,
                       InputArray intrinsic_matrix, InputArray distortion_coeff, 
                       InputArray rotation, InputArray translation, 
                       float cube_size) {
    float half = cube_size / 2;

    // Get cube edge points
    std::vector<cv::Point3f> edge_points = {
      Point3f(-half, -half, 0),
      Point3f( half, -half, 0),
      Point3f( half,  half, 0),
      Point3f(-half,  half, 0),
      Point3f(-half, -half, cube_size),
      Point3f( half, -half, cube_size),
      Point3f( half,  half, cube_size),
      Point3f(-half,  half, cube_size),
      Point3f(0, 0, 0)
    };

    // Calculate projected points
    std::vector<cv::Point2f> edge_point_project;
    projectPoints(edge_points, rotation, translation, intrinsic_matrix, distortion_coeff, edge_point_project);

    // Add index
    string text = to_string(index);
    int font_face = FONT_HERSHEY_SCRIPT_SIMPLEX; 
    double font_scale = 5;
    int thickness = 5;
    int baseline;
    Size text_size = getTextSize(text, font_face, font_scale, thickness, &baseline);
    Point origin(edge_point_project[8].x - text_size.width / 2,
                 edge_point_project[8].y + text_size.height / 2);
    putText(image, text, origin, font_face, font_scale, Scalar(0, 0, 0), thickness);

    // Draw cube edges lines
    auto x_color = Scalar(66, 64, 203);
    auto y_color = Scalar(102, 193, 134);
    auto z_color = Scalar(223, 169, 46);
    int line_width = 5;
    cv::line(image, edge_point_project[0], edge_point_project[1], x_color, line_width);
    cv::line(image, edge_point_project[1], edge_point_project[2], y_color, line_width);
    cv::line(image, edge_point_project[2], edge_point_project[3], x_color, line_width);
    cv::line(image, edge_point_project[3], edge_point_project[0], y_color, line_width);
    cv::line(image, edge_point_project[0], edge_point_project[4], z_color, line_width);
    cv::line(image, edge_point_project[1], edge_point_project[5], z_color, line_width);
    cv::line(image, edge_point_project[2], edge_point_project[6], z_color, line_width);
    cv::line(image, edge_point_project[3], edge_point_project[7], z_color, line_width);
    cv::line(image, edge_point_project[4], edge_point_project[5], x_color, line_width);
    cv::line(image, edge_point_project[5], edge_point_project[6], y_color, line_width);
    cv::line(image, edge_point_project[6], edge_point_project[7], x_color, line_width);
    cv::line(image, edge_point_project[7], edge_point_project[4], y_color, line_width);
}