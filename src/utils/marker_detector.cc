#include "mpe/utils/marker_detector.h"
using namespace std;
using namespace cv;
using namespace cv::aruco;

namespace mpe {

MarkerDetector::MarkerDetector() {
  dictionary_ = getPredefinedDictionary(DICT_6X6_50);
}

void MarkerDetector::detect(Mat image, 
                            vector<vector<Point2f>>& corners,
                            vector<int>& ids) {
  detectMarkers(image, dictionary_, corners, ids);
}

}  // namespace mpe
