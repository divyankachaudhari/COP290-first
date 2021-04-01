#include <bits/stdc++.h>
#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>

#define pb push_back
#define fast ios_base::sync_with_stdio(false), cin.tie(NULL), cout.tie(NULL);
#define endl "\n"
#define ff first
#define ss second
#define ll long long int
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
// int frame_number;
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

void mouseHandler(int event, int x, int y, int flags, void* data_ptr) {
    if  (event == 1) {
        // Taking input of the mouse click
        userdata *data = ((userdata *) data_ptr);
        // Using small red dots/circles wherever the mouse clicks for user to know
        circle(data->im, Point(x,y),2,Scalar(0,0,600), 5, cv::LINE_AA);
        // To show the Image with black dots
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
      cout << "Error : image not found" << endl;
      // wait for any key to be pressed
      cin.get();
      return -1;
  }
  return 0;
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

  // userdata data;
  data.im = inputImage;
  cout << "Choose four points in anti-clockwise fashion, starting from top-left. \nThen press Enter." << endl;
  imshow("Image", inputImage);
  setMouseCallback("Image", mouseHandler, &data);
  waitKey(0);

  destroyAllWindows();
  return data;
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

void print_pixels(Mat image) {
    for(int i=0; i<image.rows; i++) {
        for(int j=0; j<image.cols; j++) {
            float v = (float)image.at<uchar>(i, j);
            cout<<v<<" ";
        }
        cout<<endl;
    }
}

float queueDensity(Mat croppedFilteredFrame){
    float white = 0.0;
    for(int i=0; i<croppedFilteredFrame.rows; i++) {
        for(int j=0; j<croppedFilteredFrame.cols; j++) {
            float v = (float)croppedFilteredFrame.at<uchar>(i, j);

            if(v > 8) white++;
            // cout<<v<<" ";
        }
        // cout<<endl;
    }
    return (white)/((float)croppedFilteredFrame.total());
    // float whiteArea = countNonZero(croppedFilteredFrame);
    // float totalArea = croppedFilteredFrame.total();
    // float queueDensity = whiteArea/totalArea;
    // return queueDensity;
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
    // Size size1 = im_src.size();
    // cout<<size1<<endl;
    // resize(im_src, im_src, Size(), fx, fy, INTER_AREA);
    Size size = im_src.size();
    // cout<<size<<endl;
    Mat im_dst = Mat::zeros(size,CV_8UC1);
    // cvtColor(im_dst, im_dst, COLOR_BGR2GRAY);

    // Create vector and add destination points to it
    vector<Point2f> pts_dst;
    pts_dst.pb(Point2f(472,52));
    pts_dst.pb(Point2f(472, 830));
    pts_dst.pb(Point2f(800, 830));
    pts_dst.pb(Point2f(800, 52));

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

    // time(&method4_start);


    // high_resolution_clock::time_point t1 = high_resolution_clock::now();

    // Calculate the homography & warp source image to destination
    h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);
    Mat bg_warp = Mat::zeros(size,CV_8UC1);
    warpPerspective(im_src, bg_warp, h, size);

    cropped_bg_warp;
    cropImage(cropped_bg_warp, bg_warp, data, h);
    // resize(cropped_bg_warp, cropped_bg_warp, Size(), fx, fy, INTER_AREA);

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
    // Mat prev_frame = cropped_warped_frame;

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
    // int time = (method4_end - method4_start);

    double time = (double)(clock() - start)/CLOCKS_PER_SEC;
    // time /= num_threads;

    printf("Time taken: %.5f seconds.\n", time);

    // for(int i=0; i<d; i++) {
    //     out_file << i+1 << " " << ans[i] << endl;
    // }
    out_file << "Time taken: " << to_string(time) << endl;
    out_file.close();
    vid.release();
    destroyAllWindows();
    return 0;
}
