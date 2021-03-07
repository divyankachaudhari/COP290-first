#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <regex>
#include "warpAndCrop.h"

#define pb push_back
#define fast ios_base::sync_with_stdio(false), cin.tie(NULL), cout.tie(NULL);
#define endl "\n"
#define ff first
#define ss second
#define ll long long int
#define mod (ll) 1e9 + 7
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

void cropImage(Mat &croppedImage, Mat &bird_view, userdata &data, Mat &h) {
  vector<pair<int, int>> mouse_clicks(4, {0, 0});
  vector<pair<int, int>> crop_this(4, {0, 0});
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

bool isImageFile(string str) {
  // Regex to check valid image file extension.
  const regex pattern("[^\\s]+(.*?)\\.(jpg|jpeg|png|gif|JPG|JPEG|PNG|GIF)$");
  if (str.empty()){ return false; }
  if(regex_match(str, pattern)){ return true;}
  else { return false;}
}

bool isFileExist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

int checkFile(bool check1, bool check2) {
  if (check1 == false || check2 == false) {
      cout << "Hold up! \nMission failed succesfully: Could not find that image, or maybe that file isn't an image. \nDid you check if that file exists in this directory?" << endl;
      // wait for any key to be pressed
      cin.get();
      return -1;
  }
  return 0;
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
  Mat warpCroppedFrame;
  warpPerspective(sourceImage, im_dst, h, size);
  cropImage(warpCroppedFrame, sourceImage, data, h);

  return warpCroppedFrame;
}

userdata gettingInitialData(Mat inputImage){

  userdata data;
  data.im = inputImage;
  cout << "Choose four points in anti-clockwise fashion, starting from top-left. \n Then press Enter." << endl;
  imshow("Image", inputImage);
  setMouseCallback("Image", mouseHandler, &data);
  waitKey(0);

  return data;
}
