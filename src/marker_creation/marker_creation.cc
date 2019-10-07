#include <iostream>
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>
#include "mpe/utils/config_exception.h"
using namespace std;
using namespace mpe;

int main() {
  cout << "\nMarker Creation\n" << endl;

  // Read config
  cv::FileStorage fs("/input/config.yml", cv::FileStorage::READ);
  const int NUM_MARKERS = fs["num_markers"];  // less than 100
  const int MARKER_SIZE = fs["marker_size_pixel"];  // MARKER_SIZE x MARKER_SIZE pixels

  try {
    // Check parameters
    if (NUM_MARKERS < 1)
      throw ConfigException("num_marker < 1 pixel");
    if (NUM_MARKERS > 50)
      throw ConfigException("num_marker > 50 pixels");
    if (MARKER_SIZE < 32)
      throw ConfigException("marker_size < 32 pixels");
  } catch (const exception& e) {
    cerr << e.what() << endl;
    return -1;
  }

  cout << "Number of markers: " << NUM_MARKERS << endl;
  cout << "Marker size: " << MARKER_SIZE << endl;

  // Define ArUco dictionary
  auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50);

  // Create markers
  cout << "\nCreating markers...\n" << endl;
  for (int id = 0; id < NUM_MARKERS; ++id) {
    cv::Mat marker_image;
    cv::aruco::drawMarker(dictionary, id, MARKER_SIZE, marker_image);

    // Output
    stringstream ss;
    ss << "/output/marker_" << id << ".png";
    cv::imwrite(ss.str() , marker_image);
  }

  cout << "Markers are saved in output/ directory\n" << endl;

  return 0;
}