#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <regex>

#define pb push_back
#define fast ios_base::sync_with_stdio(false), cin.tie(NULL), cout.tie(NULL);
#define endl "\n"
#define ff first
#define ss second
#define ll long long int
#define mod (ll) 1e9 + 7

using namespace cv;
using namespace std;

struct userdata;
int subdivide(const Mat &img, const int rowDivisor, const int colDivisor, vector<Mat> &blocks);
void mouseHandler(int event, int x, int y, int flags, void* data_ptr);
void cropImage(Mat &croppedImage, Mat &bird_view, userdata &data, Mat &h);
userdata gettingInitialData(Mat inputImage);
bool isImageFile(string str);
bool isFileExist (const std::string& name);
int checkFile(bool check1, bool check2);
void destPoints(vector<Point2f> &pts_dst);
Mat subtract_bg(Mat background, Mat current);
float queueDensity(Mat croppedFilteredFrame);
float movingDensity(Mat previousFrame, Mat currentFrame);







#endif
