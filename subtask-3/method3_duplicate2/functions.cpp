#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <regex>
#include "functions.h"

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


int subdivide(const Mat &img, const int rowDivisor, const int colDivisor, vector<Mat> &blocks)
{
    /* Checking if the image was passed correctly */
    if(!img.data || img.empty())
        cerr << "Problem Loading Image" << endl;
        //rowDivisor =1;
    /* Cloning the image to another for visualization later, if you do not want to visualize the result just comment every line related to visualization */
    Mat maskImg = img.clone();
    /* Checking if the clone image was cloned correctly */
    if(!maskImg.data || maskImg.empty())
        cerr << "Problem Loading Image" << endl;

    // check if divisors fit to image dimensions
    // if(img.cols % colDivisor == 0 && img.rows % rowDivisor == 0)
    // {
        for(int y = 0; y < img.cols - img.cols % colDivisor; y += img.cols / colDivisor)
        {
            for(int x = 0; x < img.rows; x += img.rows / rowDivisor)
            {
                blocks.push_back(img(cv::Rect(y, x, (img.cols / colDivisor), (img.rows / rowDivisor))).clone());
                rectangle(maskImg, Point(y, x), Point(y + (maskImg.cols / colDivisor) - 1, x + (maskImg.rows / rowDivisor) - 1), CV_RGB(255, 0, 0), 1); // visualization

                // imshow("Image", maskImg); // visualization
                // waitKey(0); // visualization
            }
        }
      // }
     if(img.cols % colDivisor != 0)
    {
      for(int y = img.cols - img.cols % colDivisor; y < img.cols - img.cols % colDivisor; y += img.cols / colDivisor)
      {
          for(int x = 0; x < img.rows; x += img.rows / rowDivisor)
          {

        // cerr << "Error: Please use another divisor for the column split." << endl;
        // exit(1);
        blocks.push_back(img(cv::Rect(y, 0, (img.cols), (img.rows / rowDivisor))).clone());
        rectangle(maskImg, Point(y, 0), Point(y + (maskImg.cols) - 1, 0 + (maskImg.rows / rowDivisor) - 1), CV_RGB(255, 0, 0), 1);
        imshow("Image", maskImg); // visualization
        waitKey(0); // visualization

      }
  }

    }else if(img.rows % rowDivisor != 0)
    {
        cerr << "Error: Please use another divisor for the row split." << endl;
        exit(1);
    }
return 0;
}

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

userdata gettingInitialData(Mat inputImage){

  userdata data;
  data.im = inputImage;
  cout << "Choose four points in anti-clockwise fashion, starting from top-left. \n Then press Enter." << endl;
  imshow("Image", inputImage);
  setMouseCallback("Image", mouseHandler, &data);
  waitKey(0);

  return data;
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

float queueDensity(Mat croppedFilteredFrame){
    float white = 0.0;
    for(int i=0; i<croppedFilteredFrame.rows; i++) {
        for(int j=0; j<croppedFilteredFrame.cols; j++) {
            float v = (float)croppedFilteredFrame.at<uchar>(i, j);

            if(v > 8) white++;
        }

    }
    return (white)/((float)croppedFilteredFrame.total());
    threshold(croppedFilteredFrame, croppedFilteredFrame, 200, 255, 3);

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
