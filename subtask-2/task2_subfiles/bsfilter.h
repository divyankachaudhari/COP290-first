#ifndef BSFILTER_H
#define BSFILTER_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat subtract_bg(Mat background, Mat current);
Mat filterImage(Mat subtractedImage);
void print_pixels(Mat image);

#endif
