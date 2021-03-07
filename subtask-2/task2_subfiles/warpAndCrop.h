#ifndef WARPANDCROP_H
#define WARPANDCROP_H

#include <opencv2/opencv.hpp>
#include <iostream>

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
void mouseHandler(int event, int x, int y, int flags, void* data_ptr);

void cropImage(Mat &croppedImage, Mat &bird_view, vector<pair<int, int>> &mouse_clicks, vector<pair<int, int>> &crop_this, userdata &data, Mat &h);

void destPoints(vector<Point2f> &pts_dst);

Mat warpAndCrop(Mat sourceImage, userdata data, Mat h);

userdata gettingInitialData(Mat inputImage);

#endif
