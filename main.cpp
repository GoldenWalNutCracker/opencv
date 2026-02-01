#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;

int main()
{
	std::cout << "opencv version" << CV_VERSION << std::endl;
	Mat img = Mat::zeros(640, 480, CV_32Fc3);
	circle(img, Point(320, 340), 100, Scalar(255, 255, 255), 3);
	imshow("test", img);
	waitKey(0);
	return 0;
}