#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace std;
using namespace cv;

//Global variable for canny function



int main(int argc, const char** argv)

{ 
	VideoCapture capture("test1.MOV");

	if (!capture.isOpened()) return -1;

	namedWindow("test_window", CV_WINDOW_AUTOSIZE);


	//with hough

	while (1) {
	
		Mat frame;
		Mat edge;

		bool bSuccess = capture.read(frame); // read a new frame from video

		if (!bSuccess) {

			std::cout << "Cannot read the frame from video file" << std::endl;

			break;

		}


		CvMemStorage* storage = cvCreateMemStorage(0);

		CvSeq* lines = 0;

		cvtColor(frame, edge, COLOR_BGR2GRAY);//преобразование в черно-белый


		Mat dst;

		equalizeHist(edge, dst);//увеличение контраста

		Mat dst_blur;

		GaussianBlur(dst, dst_blur, Size(5, 5), 0, 0);//размытие по Гауссу

		Mat dst_canny;

		Canny(dst_blur, dst_canny, 180, 10, 3);//детектор границ Canny 

		
		
		IplImage* frame2 = cvCloneImage(&(IplImage)dst_canny);//преобразование из Mat в IplImage
		IplImage* out_frame = cvCloneImage(&(IplImage)frame);//преобразование из Mat в IplImage

		lines = cvHoughLines2(frame2, storage, 1, 1, CV_PI / 180, 20,20,5);//преобразоване Хаффа

		for (int i = 0; i < lines->total; i++) {
		
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);

			cvLine(out_frame, line[0], line[1], CV_RGB(0, 255, 0), 1, CV_AA, 0);

		
		}
		
		cvShowImage("test_window", out_frame);

		if (waitKey(30) == 27) {

			break;

		}

	
	}
	return 0;
}




