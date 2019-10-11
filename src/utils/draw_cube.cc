#include "mpe/utils/draw_cube.h"
#include <vector>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

void mpe::draw_cube(cv::InputOutputArray image, int index,
               cv::InputArray intrinsic_matrix, cv::InputArray distortion_coeff, 
               cv::InputArray rotation, cv::InputArray translation, 
               float cube_size) {
  float half = cube_size / 2;

  // Get cube edge points
  std::vector<cv::Point3f> edge_points = {
    cv::Point3f(-half, -half, 0),
    cv::Point3f( half, -half, 0),
    cv::Point3f( half,  half, 0),
    cv::Point3f(-half,  half, 0),
    cv::Point3f(-half, -half, cube_size),
    cv::Point3f( half, -half, cube_size),
    cv::Point3f( half,  half, cube_size),
    cv::Point3f(-half,  half, cube_size),
    cv::Point3f(0, 0, 0)
  };

  // Calculate projected points
  std::vector<cv::Point2f> edge_point_project;
  cv::projectPoints(edge_points, rotation, translation, intrinsic_matrix, distortion_coeff, edge_point_project);

  // Add index
  auto text = std::to_string(index);
  int font_face = cv::FONT_HERSHEY_SCRIPT_SIMPLEX; 
  double font_scale = 5;
  int thickness = 5;
  int baseline;
  cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
  cv::Point origin(edge_point_project[8].x - text_size.width / 2,
                edge_point_project[8].y + text_size.height / 2);
  cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(0, 0, 0), thickness);

  // Draw cube edges lines
  auto x_color = cv::Scalar(66, 64, 203);
  auto y_color = cv::Scalar(102, 193, 134);
  auto z_color = cv::Scalar(223, 169, 46);
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