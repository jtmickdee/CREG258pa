#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <stdio.h>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <ctime>
#include <time.h>

using namespace cv;
using namespace std;

const char* trackbarWindowName = "Trackbars";
const char* normPic = "norm";
const char* hsvPic = "hsv";
const char * maskPic ="mask";
const char * dilPic ="dilation";
const char * blurPic = "blurred";

int dilSize = 21;
int winX = 200;
int imgHeight = 480;
int imgWidth = 640;
int imgMiddle = imgWidth/2;

int main(int argc, char **argv)
{
  Mat hsv;
  Mat img;
  Mat mask;
  Mat dil;
  Mat blur;
  /* Start the CV system and get the first v4l camera */
	//cvInitSystem(argc, argv);
	//CvCapture *cam = cvCreateCameraCapture(0);

  //VideoCapture cap(0);
  raspicam::RaspiCam_Cv Camera;
  Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
  Camera.set(CV_CAP_PROP_FRAME_WIDTH, imgWidth);
  Camera.set(CV_CAP_PROP_FRAME_HEIGHT, imgHeight);
  Camera.open();
  if(!Camera.isOpened()){
    cout<<"Closing Program\n";
    return -1;
  }
  //sleep(3);

  cout<<"Setup Named Windows\n";
	/* Create a window to use for displaying the images */
	namedWindow(normPic, 0);
	moveWindow(normPic, 1*(winX + 25), 200);

	// /* Create a window to use for displaying the images */
	// namedWindow(maskPic, 0);
	// moveWindow(maskPic, 2*(winX + 25), 200);
  //
	// /* Create a window to use for displaying the images */
	// namedWindow(hsvPic, 0);
	// moveWindow(hsvPic, 1*(winX + 25), 200);
  //
	// /* Create a window to use for displaying the images */
	// namedWindow(dilPic, 0);
	// moveWindow(dilPic,  3*(winX + 25), 200);
  //
  // /* Create a window to use for displaying the images */
	// namedWindow(blurPic, 0);
	// moveWindow(blurPic,  4*(winX + 25), 200);

  cout<<"Beginning Loop\n";
  while (1) {
    int t = clock();
    Camera.grab();
    Camera.retrieve(img);

    cout<<"Getting Picture\n";

    //converts image to hsv
    cvtColor(img, hsv, CV_BGR2HSV);

    //blocks a certain spectrum of colors
    //this is for a tennis ball
    inRange(hsv,  Scalar(0.11*256, 0.60*256, 0.20*256, 0),
	                 Scalar(0.14*256, 1.00*256, 1.00*256, 0), mask);
//free mem
    hsv.release();

    //dilates the hsv picture to better see the color
    Mat dilElem = getStructuringElement(MORPH_ELLIPSE, Size(2*dilSize +1, 2*dilSize+1), Point(dilSize, dilSize));
    dilate(mask, dil, dilElem);

    //blurs it so then it will be easier to detect as a circle
    medianBlur(dil, blur,  31);
//free mem
   dil.release();

    //finds circles in pictures

    vector<Vec3f> circles;
    HoughCircles(blur, circles, CV_HOUGH_GRADIENT,
                 2, blur.rows/4, 200, 100 );
//free mem
    blur.release();

                 for( size_t i = 0; i < circles.size(); i++ )
             {
                  Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                  int radius = cvRound(circles[i][2]);
                  // draw the circle center
                  circle( img, center, 3, Scalar(0,255,0), -1, 8, 0 );
		  cout<<center<<endl;
                  // draw the circle outline
                  circle( img, center, radius, Scalar(0,0,255), 3, 8, 0 );
             }
    // CvSeq *circles = cvHoughCircles(hough_in, storage,
    // 	CV_HOUGH_GRADIENT, 4, size.height/10, 100, 40, 0, 0);
    cout<<(float)(clock() - t)/CLOCKS_PER_SEC<<"\n";
    cout<<"Showing Image\n";
    //shows the images in the appropriate windows
    imshow(normPic, img);
    // imshow(hsvPic, hsv);
    // imshow(maskPic, mask);
    // imshow(dilPic, dil);
    // imshow(blurPic, blur);

    //sleep(3);
    waitKey(0);
  }
                                         // Wait for a keystroke in the window
  return 0;
}
