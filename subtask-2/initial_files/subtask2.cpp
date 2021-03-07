#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct userdata {
    Mat im;
    vector<Point2f> points;
};
