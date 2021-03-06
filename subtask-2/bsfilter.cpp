#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat backgroundSubtractImage(Mat warpCroppedFrame){

  Ptr<BackgroundSubtractor> pBackSub;
  Mat subtractedImage;
  pBackSub = createBackgroundSubtractorMOG2();
  pBackSub->apply(warpCroppedFrame, subtractedImage);

  return subtractedImage;
}

Mat filterImage(Mat subtractedImage){
  Mat croppedFilteredFrame;



  return croppedFilteredFrame;
}
