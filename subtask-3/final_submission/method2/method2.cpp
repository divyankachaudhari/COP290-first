#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include "functions.h"

#define mod (ll) 1e9 + 7

using namespace cv;
using namespace std;

float fx, fy;

struct userdata {
    Mat im;
    vector<Point2f> points;
};


int main(int argc, char** argv) {

    //---taking user input points to warp perspective
    cout<<"Enter horizontal scaling parameter: \n";
    cin>>fx;
    cout<<"Enter vertical scaling parameter: \n";
    cin>>fy;
    Mat im_src = imread(argv[1]);

    // // Check if file exists; return if it doesn't
    if(checkFile(isFileExist(argv[1]), isImageFile(argv[1])) == -1) {return 0;}

    cvtColor(im_src, im_src, COLOR_BGR2GRAY);
    Size size = im_src.size();

    Mat im_dst = Mat::zeros(size,CV_8UC1);

    // Create vector and add destination points to it
    vector<Point2f> pts_dst;
    pts_dst.pb(Point2f(472,52));
    pts_dst.pb(Point2f(472, 830));
    pts_dst.pb(Point2f(800, 830));
    pts_dst.pb(Point2f(800, 52));

    // Set data for mouse event
    Mat im_temp = im_src.clone();

    // Show image and wait for mouse clicks
    userdata data = gettingInitialData(im_temp);
    clock_t start = clock();

    // Calculate the homography & warp source image to destination
    Mat h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);
    Mat bg_warp = Mat::zeros(size,CV_8UC1);
    warpPerspective(im_src, bg_warp, h, size);

    Mat cropped_bg_warp;
    cropImage(cropped_bg_warp, bg_warp, data, h);
    resize(cropped_bg_warp, cropped_bg_warp, Size(), fx, fy, INTER_AREA);

    // Start video processing - opening video
    VideoCapture vid(argv[2]);
    // VideoCapture vid("trafficsmall.mp4");

    double n = vid.get(CAP_PROP_FRAME_COUNT);
    // n - number of frames in Video

    if(!vid.isOpened()) {
        cout<<"Error unable to open video"<<endl;
        return -1;
    }

    // naming output files with proper format
    string name = "method2_";
    string str_fx = to_string(fx);
    str_fx = str_fx.substr(0, 4);
    string str_fy = to_string(fy);
    str_fy = str_fy.substr(0, 4);
    name += str_fx;
    name += "_";
    name += str_fy;
    name += ".txt";

    // opening file
    ofstream out_file(name);
    Mat frame;
    vid >> frame;

    Mat warped_frame = Mat::zeros(size,CV_8UC1);
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    warpPerspective(frame, warped_frame, h, size);
    Mat cropped_warped_frame;
    cropImage(cropped_warped_frame, warped_frame,data, h);
    resize(cropped_warped_frame, cropped_warped_frame, Size(), fx, fy, INTER_AREA);

    Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);
    // print_pixels(subtracted_warped);


    Mat prev_frame = cropped_warped_frame;
    int c = 0;
    ll d = 0;
    float prev_queu, prev_dyna;
    while(1) {
        c += 1;
        d += 1;
        c %= 2;
        Mat frame;
        vid >> frame;
        if(frame.empty()) break;
        if(c == 1) {
            Mat warped_frame = Mat::zeros(size,CV_8UC1);
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, data, h);
            resize(cropped_warped_frame, cropped_warped_frame, Size(), fx, fy, INTER_AREA);
            Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);
            float queue_d = queueDensity(subtracted_warped_cropped);

            // float dynamic_d = movingDensity(prev_frame, cropped_warped_frame);

            prev_frame = cropped_warped_frame;
            prev_queu = (queue_d);
            // prev_dyna = (dynamic_d);
            // out_file << d << " " << to_string(queue_d) << " " << to_string(dynamic_d) << endl;
            // cout << d << " " << queue_d << " " << dynamic_d <<endl;
            d %= mod;
        }
        cout << d << " " << prev_queu << endl;
        out_file << d << " " << to_string(prev_queu) << " " << endl;

        char c = (char)waitKey(25);
        if(c == 27) break;
    }

    double time = (double)(clock() - start)/CLOCKS_PER_SEC;
    printf("Time taken: %.5fs\n", time);
    out_file<<"Time taken: "<<to_string(time)<<endl;

    // printf("Time taken: %.5fs\n", (double)(clock() - start)/CLOCKS_PER_SEC);
    out_file.close();
    vid.release();
    destroyAllWindows();
    return 0;
}
