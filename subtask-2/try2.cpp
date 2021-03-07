// #include <fstream.h>
#include <opencv2/opencv.hpp>
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

struct userdata {
    Mat im;
    vector<Point2f> points;
};


void mouseHandler(int event, int x, int y, int flags, void* data_ptr) {
    if  (event == 1) {
        // Taking input of the mouse click
        userdata *data = ((userdata *) data_ptr);
        // Using small red/(black in grayscale) dots/circles wherever the mouse clicks for user to know
        circle(data->im, Point(x,y),2,Scalar(0,0,600), 5, cv::LINE_AA);
        // To show the Image with Red dots
        imshow("Image", data->im);
        // Saving the co-ordinates of mouse click in data
        data->points.push_back(Point2f(x,y));

    }
}

int checkImage(bool check) {
  if (check == false) {
      cout << "Mission failed succesfully: could not save the image. Try again?" << endl;
      // wait for any key to be pressed
      cin.get();
      return -1;
  }
  return 0;
}
bool isImageFile(string str) {
  // Regex to check valid image file extension.
  const regex pattern("[^\\s]+(.*?)\\.(jpg|jpeg|png|gif|JPG|JPEG|PNG|GIF)$");
  if (str.empty()){ return false; }
  if(regex_match(str, pattern)){ return true;}
  else { return false;}
}

void cropImage(Mat &croppedImage, Mat &bird_view, vector<pair<int, int>> &mouse_clicks, vector<pair<int, int>> &crop_this, userdata &data, Mat &h) {
  for(int i=0; i<4; i++) {
      mouse_clicks[i].ff = data.points[i].x;
      mouse_clicks[i].ss = data.points[i].y;

      Mat pt1 = (Mat_<double>(3, 1) << mouse_clicks[i].ff, mouse_clicks[i].ss, 1);
      Mat pt2 = h*pt1;
      pt2 /= pt2.at<double>(2);

      crop_this[i].ff = pt2.at<double>(0);
      crop_this[i].ss = pt2.at<double>(1);
  }
  int crop_w = crop_this[2].ff - crop_this[0].ff;
  int crop_h = crop_this[1].ss - crop_this[0].ss;
  Mat ROI(bird_view, Rect(crop_this[0].ff, crop_this[0].ss, crop_w, crop_h));

  ROI.copyTo(croppedImage);
}

void destPoints(vector<Point2f> &pts_dst){
  pts_dst.pb(Point2f(472,52));
  pts_dst.pb(Point2f(472, 830));
  pts_dst.pb(Point2f(800, 830));
  pts_dst.pb(Point2f(800, 52));

}

inline bool isFileExist (const std::string& name) {
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

// Mat backgroundSubtractImage(Mat warpCroppedFrame){
//
//   Ptr<BackgroundSubtractor> pBackSub;
//   Mat subtractedImage;
//   pBackSub = createBackgroundSubtractorMOG2();
//   pBackSub->apply(warpCroppedFrame, subtractedImage);
//
//   return subtractedImage;
// }

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
            if(v > 10) white++;
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
    calcOpticalFlowFarneback(previousFrame, currentFrame, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

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
return movingDensity;
} // what the fuck is happening in this function

Mat filterImage(Mat subtractedImage){
  Mat croppedFilteredFrame;



  return croppedFilteredFrame;
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

int main(int argc, char** argv) {
//--------------------------------taking user input points to warp perspective

    // cout<<"Enter file name/destination: ";
    // //string name = argv[1];
    // string name;
    // waitKey(0);
    // cin>> name;
    //
    // // Check if file exists; return if it doesn't
    // if(checkFile(isFileExist(name), isImageFile(name)) == -1) {return 0;}
    string name = "bg.png";

    Mat im_src = imread(name);
    cvtColor(im_src, im_src, COLOR_BGR2GRAY);
    Size size = im_src.size();
    Mat im_dst = Mat::zeros(size,CV_8UC1);
    // cvtColor(im_dst, im_dst, COLOR_BGR2GRAY);

    // Create vector and add destination points to it
    vector<Point2f> pts_dst;
    destPoints(pts_dst);

    // Set data for mouse event
    Mat im_temp = im_src.clone();
    // cvtColor(im_temp, im_temp, COLOR_BGR2GRAY);
    userdata data;
    data.im = im_temp;

    cout << "Choose four points in anti-clockwise fashion, starting from top-left. \n Then press Enter." << endl;
    
    // Show image and wait for mouse clicks
    imshow("Image", im_temp);
    setMouseCallback("Image", mouseHandler, &data);
    waitKey(0);

    // Calculate the homography & warp source image to destination
    Mat h = findHomography(data.points, pts_dst);
    warpPerspective(im_src, im_dst, h, size);

    Mat bg_warp = Mat::zeros(size,CV_8UC1);  /// f grayscale karna hai abhi
    warpPerspective(im_src, bg_warp, h, size);

    Mat cropped_bg_warp;
    vector<pair<int, int>> mouse_clicks(4, {0, 0});
    vector<pair<int, int>> crop_this(4, {0, 0});
    cropImage(cropped_bg_warp, bg_warp, mouse_clicks, crop_this, data, h);
    // cvtColor(bg_warp, bg_warp, COLOR_BGR2GRAY);
//------------------------------------------------------------------------------------------
    VideoCapture vid("trafficvideo.mp4");
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
    cropImage(cropped_warped_frame, warped_frame, mouse_clicks, crop_this, data, h);
    imshow("View corrected", warped_frame);
    // imshow("bg_warp", bg_warp);
    Mat subtracted_warped_cropped = subtract_bg(cropped_bg_warp, cropped_warped_frame);
    // print_pixels(subtracted_warped);
    imshow("subtracted", subtracted_warped_cropped);
    // float v = queueDensity(subtracted_warped_cropped);
    // out_file << to_string(v) << endl;

    Mat prev_frame = cropped_warped_frame;
    int c=1;
    ll d=0;
    while(1) {
        // c += 1;
        d += 1;
        // c %= 5;
        Mat frame;
        vid >> frame;
        if(frame.empty()) break;
        // cvtColor(frame, frame, COLOR_BGR2GRAY);
        // if(c == 1) {
        if(d) {
            // imshow("Frame", frame);
            Mat warped_frame = Mat::zeros(size,CV_8UC1);  /// f grayscale karna hai abhi
            cvtColor(frame, frame, COLOR_BGR2GRAY);
            warpPerspective(frame, warped_frame, h, size);
            Mat cropped_warped_frame;
            cropImage(cropped_warped_frame, warped_frame, mouse_clicks, crop_this, data, h);
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
