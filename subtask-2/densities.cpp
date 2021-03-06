#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

float queue_density(Mat croppedFilteredFrame){

  float white_area = countNonZero(croppedFilteredFrame);
  float total_area = croppedFilteredFrame.total();
  float queue_density = white_area/total_area;

  return queue_density;
}
