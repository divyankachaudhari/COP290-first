#include <opencv2/opencv.hpp>
#include <iostream>
#include "bsfilter.h"

using namespace cv;
using namespace std;

Mat subtract_bg(Mat background, Mat current) {
    Size size = background.size();
    Mat sub_image_return = Mat::zeros(size,CV_8UC1);
    for (int y = 0; y < sub_image_return.rows; y++) {
        for (int x = 0; x < sub_image_return.cols; x++) {
                    // Subtract the two images
                    sub_image_return.at<uchar>(y, x) = background.at<uchar>(y, x) - current.at<uchar>(y, x);
        }
    }
    return sub_image_return;
}

Mat filterImage(Mat subtractedImage){
  Mat croppedFilteredFrame;

  threshold(subtractedImage, croppedFilteredFrame, 200, 255, 3);

  return croppedFilteredFrame;
}

void print_pixels(Mat image) {
    for(int i=0; i<image.rows; i++) {
        for(int j=0; j<image.cols; j++) {
            float v = (float)image.at<uchar>(i, j);
            cout<<v<<" ";
        }
        cout<<endl;
    }
}
