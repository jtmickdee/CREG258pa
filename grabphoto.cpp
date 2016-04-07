#include <opencv/cv.h>
//#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <X11/keysym.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

//initial min and max HSV filter values.
//these will be changed using trackbars
//int H_MIN = 0;
//int H_MAX = 255;
//int S_MIN = 0;
//int S_MAX = 255;
//int V_MIN = 0;
//int V_MAX = 255;
int H_MIN = 45;
int H_MAX = 255;
int S_MIN = 99;
int S_MAX = 186;
int V_MIN = 111;
int V_MAX = 255;
//dilation amount
int dilSize = 21;
//int snapPic = 0;

const char* trackbarWindowName = "Trackbars";
const char* normPic = "norm";
const char* hsvPic = "hsv";
const char * maskPic ="mask";
const char * dilPic ="dilation";

void trackbarCallback(int pos, void* ptr){

}

void createTrackbars(){
	//create window for trackbars
	cvNamedWindow(trackbarWindowName,0);
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, trackbarCallback );
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX );
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX );
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX );
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX );
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX );

}

int main(int argc, char **argv)
{
	/* Start the CV system and get the first v4l camera */
	cvInitSystem(argc, argv);
	CvCapture *cam = cvCreateCameraCapture(0);

	/* Create a window to use for displaying the images */
	cvNamedWindow(normPic, 0);
	cvMoveWindow(normPic, 200, 200);

	/* Create a window to use for displaying the images */
	cvNamedWindow(maskPic, 0);
	cvMoveWindow(maskPic, 850, 200);

	/* Create a window to use for displaying the images */
	cvNamedWindow(hsvPic, 0);
	cvMoveWindow(hsvPic, 525, 200);

	/* Create a window to use for displaying the images */
	cvNamedWindow(dilPic, 0);
	cvMoveWindow(dilPic, 1050, 200);

	//Only for calibration
	//createTrackbars();
	/* Display images until the user presses q */
	while (1) {
		//grabs picture from camera
		cvGrabFrame(cam);
		IplImage *img = cvRetrieveFrame(cam);
		CvSize size = cvGetSize(img);
		IplImage *hsv = cvCreateImage(size, IPL_DEPTH_8U, 3);

		//converts from bgr to hsv for object detection
		cvCvtColor(img, hsv, CV_BGR2HSV);
		CvMat *mask = cvCreateMat(size.height, size.width, CV_8UC1);
		cvInRangeS(hsv, cvScalar(0.11*256, 0.60*256, 0.20*256, 0),
	                cvScalar(0.14*256, 1.00*256, 1.00*256, 0), mask);
//unnecessary only for calibration
//		cvInRangeS(hsv, cvScalar(H_MIN, S_MIN,V_MIN, 0),
 //                       cvScalar(H_MAX, S_MIN, V_MAX, 0), mask);

		//morphological operatioins
		//Mat *dilDst = CreateMat(size.height, size.width, CV_8UC1);
		Mat dilDst;
		CvMat * dilDstPtr;
		Mat medBlur;
		Mat dilElem = getStructuringElement(MORPH_RECT, Size(2*dilSize +1, 2*dilSize+1), Point(dilSize, dilSize));
		dilate(cvarrToMat(mask), dilDst, dilElem);
		//dilDstPtr = &CvMat(dilDst);
		CvMat deprecated(dilDst);
		dilDstPtr = &deprecated;
		medianBlur(dilDst, medBlur, 31);
//trying another work around
/*
	IplConvKernel *se21 = cvCreateStructuringElementEx(21, 21, 10, 10, CV_SHAPE_RECT, NULL);
	IplConvKernel *se11 = cvCreateStructuringElementEx(11, 11, 5,  5,  CV_SHAPE_RECT, NULL);
	//cvClose(mask, mask, se21); // See completed example for cvClose definition
	//cvOpen(mask, mask, se11);  // See completed example for cvOpen  definition
	cvReleaseStructuringElement(&se21);
	cvReleaseStructuringElement(&se11);
*/
	IplImage *hough_in = cvCreateImage(size, 8, 1);
//	cvCopy(mask, hough_in, NULL);
	//cvCopy(dilDstPtr, hough_in, NULL);
	CvMat deprecated3(medBlur);
	CvMat * medBlurPtr = &deprecated3;
	cvCopy(medBlurPtr, hough_in, NULL);
        cvSmooth(hough_in, hough_in, CV_GAUSSIAN, 15, 15, 0, 0);

	/* Run the Hough function */
	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq *circles = cvHoughCircles(hough_in, storage,
		CV_HOUGH_GRADIENT, 4, size.height/10, 100, 40, 0, 0);
	//adds the circles to the objects
	int i;
	for (i = 0; i < circles->total; i++) {
             float *p = (float*)cvGetSeqElem(circles, i);
	     CvPoint center = cvPoint(cvRound(p[0]),cvRound(p[1]));
	     //CvScalar val = cvGet2D(mask, center.y, center.x);
	     //CvScalar val = cvGet2D(dilDstPtr, center.y, center.x);
	     CvScalar val = cvGet2D(medBlurPtr, center.y, center.x);
	     cout<<"X direction: "<<center.x<<" ";
	     cout<<"Y direction: "<<center.y<<"\n";
	     if (val.val[0] < 1) continue;
             cvCircle(img,  center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
             cvCircle(img,  center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
             cvCircle(mask, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
             cvCircle(mask, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
//             cvCircle(dilDstPtr, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
//             cvCircle(dilDstPtr, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
//             cvCircle(medBlurPtr, center, 3,             CV_RGB(0,255,0), -1, CV_AA, 0);
//             cvCircle(medBlurPtr, center, cvRound(p[2]), CV_RGB(255,0,0),  3, CV_AA, 0);
	}

		//Shows Image
		cvShowImage(normPic, img);
		cvShowImage(hsvPic, hsv);
		cvShowImage(maskPic, mask);
		//imshow(dilPic, dilDst);
		//imshow(dilPic, medBlur);
		cvShowImage(dilPic, medBlurPtr);
// 		if(snapPic == 1){
// 			imwrite("tennisball2.jpeg", cvarrToMat(dilDstPtr));
// 			snapPic = 0;
// }

		if (cvWaitKey(10) == XK_q){
			//only for callibration
			//printf("HMin: %d HMax: %d\n SMin: %d SMax: %d\n VMin: %d VMax: %d\n", H_MIN, H_MAX, S_MIN, S_MAX, V_MIN, V_MAX);

			return 0;
		}
		//cvReleaseImage(&img);
	}
}
