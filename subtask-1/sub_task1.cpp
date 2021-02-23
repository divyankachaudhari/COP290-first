#include <opencv2/opencv.hpp>

#define pb push_back
#define fast ios_base::sync_with_stdio(false), cin.tie(NULL), cout.tie(NULL);
#define endl "\n"
#define ff first
#define ss second


using namespace cv;
using namespace std;

struct userdata {
    Mat im;
    vector<Point2f> points;
};


void mouseHandler(int event, int x, int y, int flags, void* data_ptr) {
    if  (event == EVENT_LBUTTONDOWN) {
        userdata *data = ((userdata *) data_ptr);
        circle(data->im, Point(x,y),3,Scalar(0,0,255), 5, cv::LINE_AA);
        imshow("Image", data->im);
        if (data->points.size() < 4) {
            data->points.push_back(Point2f(x,y));
        }
    }
}

int main( int argc, char** argv) {
    fast;
    // Read source image.
    // Mat im_src = imread("book1.jpg");
    // String name;
    string name = argv[1];
    Mat im_src = imread(name);

    Size size = im_src.size();
    Mat im_dst = Mat::zeros(size,CV_8UC3);

    // Create a vector of destination points.
    vector<Point2f> pts_dst;

    // points as given by maam // choose from top left anti-clockwise
    pts_dst.pb(Point2f(472,52));
    pts_dst.pb(Point2f(472, 830));
    pts_dst.pb(Point2f(800, 830));
    pts_dst.pb(Point2f(800, 52));

    // Set data for mouse event
    Mat im_temp = im_src.clone();
    userdata data;
    data.im = im_temp;

    cout << "Anti-clockwise fashion, starting from top-left" << endl;

    // show image and wait 
    imshow("Image", im_temp);
    // set the callback function for any mouse event
    setMouseCallback("Image", mouseHandler, &data);
    waitKey(0);

    // calculate the homography
    Mat h = findHomography(data.points, pts_dst);
    // Warp source image to destination
    warpPerspective(im_src, im_dst, h, size);

    // Show image
    imshow("Image", im_dst);
    bool check1 = imwrite("birds_eye_view.jpg", im_dst); 
    // if the image is not saved 
	if (check1 == false) { 
	    cout << "Mission - Saving the image, FAILED" << endl; 
	  
	    // wait for any key to be pressed 
	    cin.get(); 
	    return -1; 
	} 
	cout << "Successfully saved the uncropped birds view image. " << endl; 

    // now crop
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
	
    Mat bird_view = imread("birds_eye_view.jpg", 1);
    int crop_w = crop_this[2].ff - crop_this[0].ff;
    int crop_h = crop_this[1].ss - crop_this[0].ss;
    Mat ROI(bird_view, Rect(crop_this[0].ff, crop_this[0].ss, crop_w, crop_h));
    Mat croppedImage;
    ROI.copyTo(croppedImage);

    imshow("cropped_birds_view", croppedImage);
    
    bool check2 = imwrite("cropped_birds_view.jpg", croppedImage);
    // if the image is not saved 
    if (check2 == false) { 
        cout << "Mission - Saving the image, FAILED" << endl; 
      
        // wait for any key to be pressed 
        cin.get(); 
        return -1; 
    } 
    cout << "Successfully saved the cropped birds view image. " << endl; 

    waitKey(0);

    return 0;
}

