#include <opencv2/opencv.hpp>
#include <iostream>
#include "densities.h"

using namespace cv;
using namespace std;

float queueDensity(Mat croppedFilteredFrame){
    float white = 0.0;
    for(int i=0; i<croppedFilteredFrame.rows; i++) {
        for(int j=0; j<croppedFilteredFrame.cols; j++) {
            float v = (float)croppedFilteredFrame.at<uchar>(i, j);

            if(v > 8) white++;
            // cout<<v<<" ";
        }
        // cout<<endl;
    }
    return (white)/((float)croppedFilteredFrame.total());
    threshold(croppedFilteredFrame, croppedFilteredFrame, 200, 255, 3);
    // float whiteArea = countNonZero(croppedFilteredFrame);
    // float totalArea = croppedFilteredFrame.total();
    // float queueDensity = whiteArea/totalArea;
    // return queueDensity;
}

float movingDensity(Mat previousFrame, Mat currentFrame){

  Mat flow(previousFrame.size(), CV_32FC2);
  calcOpticalFlowFarneback(previousFrame, currentFrame, flow,0.5, 3, 15, 3, 5, 1.2, 0);

  Mat flow_parts[2];

  split(flow, flow_parts);

  Mat magnitude, angle, magn_norm;
  cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);

  normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);

  angle *= ((1.f / 360.f)*(180.f / 255.f));

  Mat _hsv[3], hsv, hsv8, bgr;
  _hsv[0] = angle;
  _hsv[1] = Mat::ones(angle.size(), CV_32F);
  _hsv[2] = magn_norm;

  merge(_hsv, 3, hsv);
  hsv.convertTo(hsv8, CV_8U, 255.0);

  float movement = sum(hsv8)[2];
  float maxMovement = hsv8.total()*255.0;

  float movingDensity = movement/maxMovement;

  return 2.3*movingDensity;
}
