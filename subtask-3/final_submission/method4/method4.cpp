#include <bits/stdc++.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include "functions.h"
#define mod (ll) 1e9 + 7

using namespace cv;
using namespace std;

float fx, fy;
Mat im_src;
Mat h;
Mat cropped_bg_warp;
int num_threads;

time_t method4_end, method4_start;

struct data_for_threads {
Size size;
int start_frame;
string vid_name;
int skip;
float queue_d;
};

vector<float> ans;

struct userdata {
    Mat im;
    vector<Point2f> points;
};

userdata data;

void* queue_threads(void *arg) {
    struct data_for_threads *arg_struct = (struct data_for_threads*) arg;

    VideoCapture vid(arg_struct->vid_name);
    Mat frame;
    int c=0;
    int startf = arg_struct->start_frame;
    while(c < startf) {
        vid >> frame;
        c++;
    }
    while(1) {
        vid >> frame;
        if(frame.empty()) break;

        c ++;
        if((c-startf)%arg_struct->skip == 0) {
            Mat warped_frame = Mat::zeros(arg_struct->size, CV_8UC1);
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, arg_struct->size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, data, h);

            Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);

            float queue_d = queueDensity(subtracted_warped_cropped);

            ans[c] = queue_d;
            cout << c << " " << queue_d << "\n";
        }
    }

    // Mat frame = arg_struct->frame;
    // arg_struct->queue_d = queue_d;
    pthread_exit(0);

}


int main(int argc, char** argv) {

//--------------------------------taking user input points to warp perspective

    im_src = imread(argv[1]);

    // // Check if file exists; return if it doesn't
    if(checkFile(isFileExist(argv[1]), isImageFile(argv[1])) == -1) {return 0;}

    cvtColor(im_src, im_src, COLOR_BGR2GRAY);
    Size size = im_src.size();
    Mat im_dst = Mat::zeros(size,CV_8UC1);

    // Create vector and add destination points to it
    vector<Point2f> pts_dst;
    destPoints(pts_dst);

    // Set data for mouse event
    Mat im_temp = im_src.clone();

    // Show image and wait for mouse clicks
    data = gettingInitialData(im_temp);

    cout<<"Enter the number of threads: \n";
    cin>>num_threads;
    if(num_threads <= 1) {
        cout<<"Error: Number of threads should be greater than 1\n";
        return -1;
    }

    clock_t start = clock();

    // Calculate the homography & warp source image to destination
    h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);
    Mat bg_warp = Mat::zeros(size,CV_8UC1);
    warpPerspective(im_src, bg_warp, h, size);

    cropped_bg_warp;
    cropImage(cropped_bg_warp, bg_warp, data, h);

//------------------------------------------------------------------------------------------
    VideoCapture vid(argv[2]);

    double n = vid.get(CAP_PROP_FRAME_COUNT);
    // print number of frames
    // cout << n;

    if(!vid.isOpened()) {
        cout<<"Error unable to open video"<<endl;
        return -1;
    }

    // int num_threads;

    struct data_for_threads args[num_threads+5];
    // struct data_for_threads temp;
    pthread_t ptids[num_threads];
    ans.resize((int) n + 5);

    string name = "method4_";
    if(num_threads < 10) name += '0';
    name += to_string(num_threads);
    name += ".txt";
    ofstream out_file(name);

    int split = n/num_threads;

    for(int i=0; i<num_threads; i++) {
        args[i].vid_name = argv[2];
        args[i].size = size;
        args[i].skip = num_threads;
        args[i].start_frame = i;
        pthread_create(&ptids[i], NULL, queue_threads, (void *) &args[i]);
    }

    for(int i=0; i<num_threads; i++) {
        pthread_join(ptids[i], NULL);
    }



    time(&method4_end);
    double time = (double)(clock() - start)/CLOCKS_PER_SEC;
    // time /= num_threads;

    printf("Time taken: %.5f seconds.\n", time);
    out_file << "Time taken: " << to_string(time) << endl;
    out_file.close();
    vid.release();
    destroyAllWindows();
    return 0;
}
