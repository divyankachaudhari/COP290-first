#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <regex>
#include <cstdlib>
#include <pthread.h>
#include "functions.h"

#define NUM_THREADS 2

#define mod (ll) 1e9 + 7
using namespace cv;
using namespace std;

struct userdata {
    Mat im;
    vector<Point2f> points;
};

struct queue_struct {
  int num;
  int queue_density;
};

void* startProcessing(void* args);
vector <Mat> imageQueue;
vector<Mat> backgroundQueue;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int subdivide(const Mat img, const int rowDivisor, const int colDivisor, vector<Mat> blocks)
{
    /* Checking if the image was passed correctly */
    if(!img.data || img.empty())
        cerr << "Problem Loading Image" << endl;

    /* Cloning the image to another for visualization later, if you do not want to visualize the result just comment every line related to visualization */
    Mat maskImg = img.clone();
    /* Checking if the clone image was cloned correctly */
    if(!maskImg.data || maskImg.empty())
        cerr << "Problem Loading Image" << endl;

    // check if divisors fit to image dimensions
    if(img.cols % colDivisor == 0 && img.rows % rowDivisor == 0)
    {
        for(int y = 0; y < img.cols; y += img.cols / colDivisor)
        {
            for(int x = 0; x < img.rows; x += img.rows / rowDivisor)
            {
                blocks.push_back(img(cv::Rect(y, x, (img.cols / colDivisor), (img.rows / rowDivisor))).clone());
                rectangle(maskImg, Point(y, x), Point(y + (maskImg.cols / colDivisor) - 1, x + (maskImg.rows / rowDivisor) - 1), CV_RGB(255, 0, 0), 1); // visualization

                // imshow("Image", maskImg); // visualization
                // waitKey(0); // visualization
            }
        }
    }else if(img.cols % colDivisor != 0)
    {
        cerr << "Error: Please use another divisor for the column split." << endl;
        exit(1);
    }else if(img.rows % rowDivisor != 0)
    {
        cerr << "Error: Please use another divisor for the row split." << endl;
        exit(1);
    }
return 0;
}


int main(int argc, char** argv) {
//--------------------------------taking user input points to warp perspective

    Mat im_src = imread(argv[1]);
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
    cout<< "Accessing video" << endl;
    ofstream out_file("out.txt");
    Mat frame;
    vid >> frame;
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
            cout<< "Trying to enter the loop"<< endl;
            Mat warped_frame = Mat::zeros(size,CV_8UC1);  /// f grayscale karna hai abhi
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, data, h);
            // imshow("View corrected", warped_frame);
            // imshow("bg_warp", bg_warp);
            int one = 1;
            cout<< "trying to subdivide image"<< endl;
            subdivide(cropped_warped_frame, one, 4, imageQueue);
            subdivide(cropped_bg_warp, one, 4, backgroundQueue);

            // print_pixels(subtracted_warped);
            // imshow("subtracted", subtracted_warped_cropped);

            cout<< "Entering threads"<< endl;
            //vector<struct queue_struct> td;
            struct queue_struct td[NUM_THREADS];
            float queue_d =0;
              /* Create the threads */
             pthread_t tids[NUM_THREADS];
             for(int i = 0; i < NUM_THREADS; i++) {

               td[i].num = i;
                 pthread_create(&tids[i], NULL, startProcessing, (void *)&td[i]);
               }

             /* Reap the threads */
             for(int i = 0; i < NUM_THREADS; i++) {
                 pthread_join(tids[i], NULL);
                queue_d += td[i].queue_density;
               }

             imageQueue.clear();
             backgroundQueue.clear();




            // float dynamic_d = movingDensity(prev_frame, cropped_warped_frame);

            // prev_frame = cropped_warped_frame;
            out_file << d << " " << to_string(queue_d) <<  endl;
            cout << d << " " << queue_d <<endl;
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


void* startProcessing(void* args) {
   /* Each thread grabs an image from imageQueue, removes it from the
      queue, and then processes it. The grabbing and removing are done
      under a lock */
      cout<< "In the thread" << endl;
      float queue_d = 0;
      struct queue_struct *args_struct =  (struct queue_struct*) args;
  Mat image;
  Mat image2;
  Mat emptyImage;
  /* Get the first image for each thread */
  pthread_mutex_lock(&mutex1);
  if(!imageQueue.empty()) {
     image = imageQueue[0];
     image2 = backgroundQueue[0];
     Mat subtracted_warped_cropped = subtract_bg(image2, image);

     queue_d += queueDensity(subtracted_warped_cropped);
     vector<Mat>::iterator it;

     it = imageQueue.begin();
     it = imageQueue.erase(it);

     vector<Mat>::iterator it2;

     it2 = backgroundQueue.begin();
     it2 = backgroundQueue.erase(it2);
  }
  pthread_mutex_unlock(&mutex1);

  while(!image.empty())
    {
      /* Process the image - right now I just want to display it */
      // namedWindow("window", CV_WINDOW_AUTOSIZE);
      // imshow("window", image);
      // sleep(10);

      /* Obtain the next image in the queue */
      pthread_mutex_lock(&mutex1);
      if(!imageQueue.empty()) {
        image = imageQueue[0];
        image2 = backgroundQueue[0];
        Mat subtracted_warped_cropped = subtract_bg(image2, image);

        queue_d += queueDensity(subtracted_warped_cropped);

        vector<Mat>::iterator it;
        it = imageQueue.begin();
        it = imageQueue.erase(it);
        vector<Mat>::iterator it2;

        it2 = backgroundQueue.begin();
        it2 = backgroundQueue.erase(it2);
      } else {
        image = emptyImage;
        image2 = emptyImage;
      }
      pthread_mutex_unlock(&mutex1);
    }
    args_struct->queue_density = queue_d;

   pthread_exit(NULL);
}
