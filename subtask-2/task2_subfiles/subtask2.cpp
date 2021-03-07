#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "bsfilter.h"
#include "densities.h"
#include "warpAndCrop.h"

#define mod (ll) 1e9 + 7
using namespace cv;
using namespace std;

// //TO DO:
// 1. Hardcoded file names, change
// 2. Clean the code
// 3. threshold add -- optional
// 4. comment the code
// 5. Make it faster

struct userdata {
    Mat im;
    vector<Point2f> points;
};


int main(int argc, char** argv) {
//--------------------------------taking user input points to warp perspective

    Mat im_src = imread(argv[1]);
    // // Check if file exists; return if it doesn't
    if(checkFile(isFileExist(argv[1]), isImageFile(argv[1])) == -1) {return 0;}

    cvtColor(im_src, im_src, COLOR_BGR2GRAY);
    Size size = im_src.size();
    Mat im_dst = Mat::zeros(size,CV_8UC1);
    // cvtColor(im_dst, im_dst, COLOR_BGR2GRAY);

    // Create vector and add destination points to it
    vector<Point2f> pts_dst;
    destPoints(pts_dst);

    // Set data for mouse event
    Mat im_temp = im_src.clone();

    // Show image and wait for mouse clicks
    userdata data = gettingInitialData(im_temp);

    // Calculate the homography & warp source image to destination
    Mat h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);

    Mat bg_warp = Mat::zeros(size,CV_8UC1);  /// f grayscale karna hai abhi
    warpPerspective(im_src, bg_warp, h, size);

    Mat cropped_bg_warp;
    cropImage(cropped_bg_warp, bg_warp, data, h);
    // cvtColor(bg_warp, bg_warp, COLOR_BGR2GRAY);
//------------------------------------------------------------------------------------------
    VideoCapture vid(argv[2]);
    // VideoCapture vid("trafficsmall.mp4");

    double n = vid.get(CAP_PROP_FRAME_COUNT);
    // print number of frames
    // cout << n;
    if(!vid.isOpened()) {
        cout<<"Error unable to open video"<<endl;
        return -1;
    }
    ofstream out_file("out.txt");
    Mat frame;
    vid >> frame;
    // cvtColor(frame, frame, COLOR_BGR2GRAY);
        // imshow("Frame", frame);
    Mat warped_frame = Mat::zeros(size,CV_8UC1);
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    warpPerspective(frame, warped_frame, h, size);
    Mat cropped_warped_frame;
    cropImage(cropped_warped_frame, warped_frame,data, h);
    imshow("View corrected", warped_frame);
    // imshow("bg_warp", bg_warp);
    Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);
    // print_pixels(subtracted_warped);
    imshow("subtracted", subtracted_warped_cropped);
    // float v = queueDensity(subtracted_warped_cropped);
    // out_file << to_string(v) << endl;

    Mat prev_frame = cropped_warped_frame;
    int c = 1;
    ll d = 0;
    while(1) {
        c += 1;
        d += 1;
        c %= 5;
        Mat frame;
        vid >> frame;
        if(frame.empty()) break;
        // cvtColor(frame, frame, COLOR_BGR2GRAY);
        if(c == 1) {
        //if(d) {
            // imshow("Frame", frame);
            Mat warped_frame = Mat::zeros(size,CV_8UC1);  /// f grayscale karna hai abhi
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, data, h);
            imshow("View corrected", warped_frame);
            // imshow("bg_warp", bg_warp);
            Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);
            // print_pixels(subtracted_warped);
            imshow("subtracted", subtracted_warped_cropped);
            float queue_d = queueDensity(subtracted_warped_cropped);

            float dynamic_d = movingDensity(prev_frame, cropped_warped_frame);

            prev_frame = cropped_warped_frame;
            out_file << d << " " << to_string(queue_d) << " " << to_string(dynamic_d) << endl;
            cout << d << " " << queue_d << " " << dynamic_d <<endl;
            d %= mod;
        }

        char c = (char)waitKey(25);
        if(c == 27) break;
    }
    out_file.close();
    vid.release();
    destroyAllWindows();
    return 0;
}
