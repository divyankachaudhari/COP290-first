#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct userdata {
    Mat im;
    vector<Point2f> points;
};

void mouseHandler(int event, int x, int y, int flags, void* data_ptr) {
    if  (event == 1) {
        // Taking input of the mouse click
        userdata *data = ((userdata *) data_ptr);
        // Using small red dots/circles wherever the mouse clicks for user to know
        circle(data->im, Point(x,y),2,Scalar(0,0,600), 5, cv::LINE_AA);
        // To show the Image with Red dots
        imshow("Image", data->im);
        // Saving the co-ordinates of mouse click in data
        data->points.push_back(Point2f(x,y));

    }
}

void cropImage(Mat &croppedImage, Mat &bird_view, vector<pair<int, int>> &mouse_clicks, vector<pair<int, int>> &crop_this, userdata &data, Mat &h) {
  for(int i=0; i<4; i++) {
      mouse_clicks[i].ff = data.points[i].x;
      mouse_clicks[i].ss = data.points[i].y;
      // cout<<mouse_clicks[i].ff<<" "<<mouse_clicks[i].ss<<endl;

      Mat pt1 = (Mat_<double>(3, 1) << mouse_clicks[i].ff, mouse_clicks[i].ss, 1);
      Mat pt2 = h*pt1;
      pt2 /= pt2.at<double>(2);

      // cout<<pt2.at<double>(0)<<" "<<pt2.at<double>(1)<<" "<<pt2.at<double>(2)<<"kb"<<endl;
      crop_this[i].ff = pt2.at<double>(0);
      crop_this[i].ss = pt2.at<double>(1);
  }
  int crop_w = crop_this[2].ff - crop_this[0].ff;
  int crop_h = crop_this[1].ss - crop_this[0].ss;
  Mat ROI(bird_view, Rect(crop_this[0].ff, crop_this[0].ss, crop_w, crop_h));

  ROI.copyTo(croppedImage);
}

void destPoints(vector<Point2f> &pts_dst){
  pts_dst.pb(Point2f(472,52));
  pts_dst.pb(Point2f(472, 830));
  pts_dst.pb(Point2f(800, 830));
  pts_dst.pb(Point2f(800, 52));

}

Mat warpAndCrop(Mat sourceImage, userdata data, Mat h){
  Size size = sourceImage.size();
  Mat im_dst = Mat::zeros(size,CV_8UC3);
  vector<pair<int, int>> mouse_clicks(4, {0, 0});
  vector<pair<int, int>> crop_this(4, {0, 0});
  // vector<Point2f> pts_dst;
  // destPoints(pts_dst);
  // Mat h = findHomography(data.points, pts_dst);
  warpPerspective(sourceImage, im_dst, h, size);
  cropImage(warpCroppedFrame, bird_view, mouse_clicks, crop_this, data, h);

  return warpCroppedFrame;
}

userdata gettingInitialData(Mat inputImage){

  userdata data;
  data.im = inputImage;

  imshow("Image", inputImage);
  setMouseCallback("Image", mouseHandler, &data);
  waitKey(0);

  return data;
}
