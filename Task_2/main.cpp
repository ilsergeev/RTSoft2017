#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include <vector> 
#include <string>
#define cvCvtPixToPlane cvSplit
#define cvCopyImage( src, dst )         cvCopy( src, dst, 0 )
#define cvZero  cvSetZero


using namespace std;
using namespace cv;

IplImage* frame = 0;
IplImage* hsv_image = 0;
IplImage* h_before = 0;
IplImage* s_before = 0;
IplImage* v_before = 0;

IplImage* h_after = 0;
IplImage* s_after = 0;
IplImage* v_after = 0;

IplImage* summ = 0;

IplImage* src = 0;

int h_min = 132;
int h_max = 256;

int s_min = 107;
int s_max = 256;

int v_min = 182;
int v_max = 256;

std::vector< std::vector<cv::Point> > contours;
std::vector<Vec4i> hierachy;

const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = 1920 * 1080 / 1.5;

int trackObjectX(Mat summ_mat, int &x, Mat &frame_mat) { 

	cv::findContours(summ_mat, contours, hierachy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	double minArea = 0;
	bool objectFound = 0;

	if (hierachy.size() > 0) {

		int numObjects = hierachy.size();
		if (numObjects < MAX_NUM_OBJECTS) {
		
			for (int index = 0; index >= 0; index = hierachy[index][0]) {
			
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				if (area > MIN_OBJECT_AREA && area < MAX_OBJECT_AREA && area > minArea) {
				
					x = moment.m10 / area;
					objectFound = true;
					minArea = area;


				}
				else objectFound = false;

				if (objectFound = true) {
				
					cout << "X coordinate: " << x;
					cout << "!!" << hierachy.size();
				}
				else {};
			}
		}
	}

	return x;
}

int trackObjectY(Mat summ_mat, int &y, Mat &frame_mat) {

	cv::findContours(summ_mat, contours, hierachy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	double minArea = 0;
	bool objectFound = 0;

	if (hierachy.size() > 0) {

		int numObjects = hierachy.size();
		if (numObjects < MAX_NUM_OBJECTS) {

			for (int index = 0; index >= 0; index = hierachy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				if (area > MIN_OBJECT_AREA && area < MAX_OBJECT_AREA && area > minArea) {

					y = moment.m01 / area;
					objectFound = true;
					minArea = area;


				}
				else objectFound = false;

				if (objectFound = true) {

					cout << "X coordinate: " << y;
				}
				else {};
			}
		}
	}

	return y;
}
int main(int argc, const char** argv) {

	try {
		char* filename = "test.MOV";
		char* filename1 = "src.jpg";
		CvCapture* capture = cvCreateFileCapture(filename);
		src = cvLoadImage(filename1, 1);

		namedWindow("track_object", CV_WINDOW_AUTOSIZE);

		while (1) {


			frame = cvQueryFrame(capture);

			if (!frame) {

				cout << "No captured video";
				break;
			}

			hsv_image = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);

			h_before = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			s_before = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			v_before = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			h_after = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			s_after = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			v_after = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
			summ = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);


			cvCvtColor(frame, hsv_image, CV_RGB2HSV);

			cvCvtPixToPlane(hsv_image, h_before, s_before, v_before, 0);

			cvInRangeS(h_before, cvScalar(h_min), cvScalar(h_max), h_after);
			cvInRangeS(s_before, cvScalar(s_min), cvScalar(s_max), s_after);
			cvInRangeS(v_before, cvScalar(v_min), cvScalar(v_max), v_after);

			cvAnd(h_after, s_after, summ);
			cvAnd(summ, v_after, summ);

			Mat summ_mat = cv::cvarrToMat(summ);//Преобразование iplImage в Mat
			Mat frame_mat = cv::cvarrToMat(frame);//Преобразование iplImage в Mat

			//очищаем изображение
			cv::dilate(summ_mat, summ_mat, cv::Mat(), cv::Point(-1, -1), 3);

			cv::erode(summ_mat, summ_mat, cv::Mat(), cv::Point(-1, -1), 1);

			int x = 0;
			int y = 0;



			//roi try
			cvSetImageROI(frame, cvRect(trackObjectX(summ_mat, x, frame_mat), trackObjectY(summ_mat, y, frame_mat), 640, 360));
			//cvAddS(frame, cvScalar(200), frame);
			// обнулим изображение
			cvZero(frame);

			// копируем изображение
			cvCopyImage(src, frame);


			cvResetImageROI(frame);
			// показываем изображение

			cout << "width" << frame->width;
			cout << "height" << frame->height;
			cvShowImage("track_object", frame);

			if (waitKey(30) == 27)
			{
				break;
			}

		}

		cvReleaseImage(&frame);
		cvReleaseImage(&hsv_image);
		cvReleaseImage(&h_before);
		cvReleaseImage(&s_before);
		cvReleaseImage(&v_before);
		cvReleaseImage(&h_after);
		cvReleaseImage(&s_after);
		cvReleaseImage(&v_after);
		cvReleaseImage(&summ);
		cvReleaseImage(&src);

		cvDestroyAllWindows();
	

		return 0;
	}
	catch (int i)
	{
		exit(1);

	}
}