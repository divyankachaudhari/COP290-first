#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include <chrono>
#include "functions.h"

#define mod (ll) 1e9 + 7
using namespace cv;
using namespace std;

struct userdata {
    Mat im;
    vector<Point2f> points;
};

struct queue_struct {
  int num;
  float queue_density;
  float dynamic_density;
};

void* startProcessing(void* args);

vector <Mat> imageQueue;
vector<Mat> prevImageQueue;
vector<Mat> backgroundQueue;

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv) {
//------------------------------------------------------------------------------------------
    // Taking the input of number of threads
    int num_threads;
    cout<<"Enter number of threads: ";
    cin>> num_threads;

    // Inputing background image for subtraction
    Mat im_src = imread(argv[1]);

    // Check if file exists; return if it doesn't
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
    destroyAllWindows();
    auto begin = std::chrono::high_resolution_clock::now();
    clock_t start = clock();

    // Calculate the homography & warp source image to destination
    Mat h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);

    Mat bg_warp = Mat::zeros(size,CV_8UC1);
    warpPerspective(im_src, bg_warp, h, size);

    Mat cropped_bg_warp;
    cropImage(cropped_bg_warp, bg_warp, data, h);

//------------------------------------------------------------------------------------------
    // Capturing and opening video
    VideoCapture vid(argv[2]);
    double n = vid.get(CAP_PROP_FRAME_COUNT);
    if(!vid.isOpened()) {
        cout<<"Error unable to open video"<<endl;
        return -1;
    }
    // Opening file to save data for each thread
    string name = "method3_";
    if(num_threads < 10) name += '0';
    name += to_string(num_threads);
    name += ".txt";
    ofstream out_file(name);

    Mat frame;
    vid >> frame;

    Mat warped_frame = Mat::zeros(size,CV_8UC1);
    cvtColor(frame, frame, COLOR_BGR2GRAY);
    warpPerspective(frame, warped_frame, h, size);
    Mat cropped_warped_frame;
    cropImage(cropped_warped_frame, warped_frame,data, h);
    Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);


    Mat prev_frame = cropped_warped_frame;
    // Going in the main computation loop
//------------------------------------------------------------------------------------------
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
            // image manipulation
            Mat warped_frame = Mat::zeros(size,CV_8UC1);
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, data, h);

            // Spatially divide image into strips
            subdivide(cropped_warped_frame, 1, num_threads, imageQueue);
            subdivide(cropped_bg_warp, 1, num_threads, backgroundQueue);
            subdivide(prev_frame, 1, num_threads, prevImageQueue);

            struct queue_struct td[num_threads];
            float queue_d =0;
            float dynamic_d =0;

            // Create the threads
             pthread_t tids[num_threads];
             for(int i = 0; i < num_threads; i++) {

               td[i].num = i;
                 pthread_create(&tids[i], NULL, startProcessing, (void *)&td[i]);
               }

             // Reap the threads
             for(int i = 0; i < num_threads; i++) {
                 pthread_join(tids[i], NULL);
                queue_d += td[i].queue_density;
                dynamic_d += td[i].dynamic_density;
               }

             imageQueue.clear();
             backgroundQueue.clear();
             prevImageQueue.clear();

           prev_frame = cropped_warped_frame;

           // Output the results
           out_file << d << " " << to_string(queue_d) << " " << to_string(dynamic_d) << endl;
           cout << d << " " << queue_d << " " << dynamic_d <<endl;
            d %= mod;
        }

        char c = (char)waitKey(25);
        if(c == 27) break;
    }
//------------------------------------------------------------------------------------------
    // Printing CPU clock and wall clock time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    double time = (double)(clock() - start)/CLOCKS_PER_SEC;

    printf("Wall clock time taken: %.3f seconds.\n", elapsed.count() * 1e-9);
    printf("CPU clock time taken: %.5f seconds.\n", time);

    out_file << "CPU clock time taken: " << to_string(time) << endl;
    out_file << "Wall clock time taken: " << to_string(elapsed.count() * 1e-9) << endl;

    // Closing files
    out_file.close();
    vid.release();
    destroyAllWindows();
    return 0;
}

//------------------------------------------------------------------------------------------

// Processing function for threads
void* startProcessing(void* args) {
   /* Each thread grabs an image from imageQueue, removes it from the
      queue, and then processes it. The grabbing and removing are done
      under a lock */

      float queue_d = 0;
      float dynamic_d =0;
      struct queue_struct *args_struct =  (struct queue_struct*) args;
  Mat image;
  Mat image2;
  Mat image3;
  Mat emptyImage;

  /* Get the first image for each thread */
  pthread_mutex_lock(&mutex1);
  if(!imageQueue.empty()) {
    image = imageQueue[0];
    image2 = backgroundQueue[0];
    image3 = prevImageQueue[0];

    queue_d += queueDensity(subtract_bg(image2, image));
    dynamic_d += movingDensity(image, image3);

    vector<Mat>::iterator it;
    it = imageQueue.begin();
    it = imageQueue.erase(it);

    vector<Mat>::iterator it2;
    it2 = backgroundQueue.begin();
    it2 = backgroundQueue.erase(it2);

    vector<Mat>::iterator it3;
    it3 = prevImageQueue.begin();
    it3 = prevImageQueue.erase(it3);
  }
  pthread_mutex_unlock(&mutex1);

  while(!image.empty())
    {
      /* Obtain the next image in the queue */
      pthread_mutex_lock(&mutex1);
      if(!imageQueue.empty()) {
        image = imageQueue[0];
        image2 = backgroundQueue[0];
        image3 = prevImageQueue[0];

        queue_d += queueDensity(subtract_bg(image2, image));
        dynamic_d += movingDensity(image, image3);

        vector<Mat>::iterator it;
        it = imageQueue.begin();
        it = imageQueue.erase(it);

        vector<Mat>::iterator it2;
        it2 = backgroundQueue.begin();
        it2 = backgroundQueue.erase(it2);

        vector<Mat>::iterator it3;
        it3 = prevImageQueue.begin();
        it3 = prevImageQueue.erase(it3);
      } else {
        image = emptyImage;
        image2 = emptyImage;
        image3 = emptyImage;
      }
      pthread_mutex_unlock(&mutex1);
    }
    args_struct->queue_density = queue_d;
    args_struct->dynamic_density = dynamic_d;

   pthread_exit(NULL);
}
