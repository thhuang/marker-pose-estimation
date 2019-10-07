#ifndef MPE_UTILS_DRAW_CUBE_H_
#define MPE_UTILS_DRAW_CUBE_H_

#include <opencv2/core/mat.hpp>

namespace mpe {

void draw_cube(cv::InputOutputArray, int ,cv::InputArray, 
               cv::InputArray, cv::InputArray, cv::InputArray, float);

}  // namespace mpe

#endif  // MPE_UTILS_DRAW_CUBE_H_