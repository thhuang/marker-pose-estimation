#include "mpe/utils/marker_detector.h"

mpe::MarkerDetector::MarkerDetector() {
  dictionary_ = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);
}

void mpe::MarkerDetector::detect(cv::Mat image, 
                            std::vector<std::vector<cv::Point2f>>& corners,
                            std::vector<int>& ids) {
  cv::aruco::detectMarkers(image, dictionary_, corners, ids);
}
