#ifndef DENSITIES_H
#define DENSITIES_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


float queueDensity(Mat croppedFilteredFrame);
float movingDensity(Mat previousFrame, Mat currentFrame);

#endif
