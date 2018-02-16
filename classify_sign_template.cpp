#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

/*#define WARPED_XSIZE 200
#define WARPED_YSIZE 300
*/
using namespace cv;
using namespace std;

Mat src; Mat src_gray, warped_result;
Mat speed_80, speed_40;
int canny_thresh = 120;


#define VERY_LARGE_VALUE 100000

#define NO_MATCH    0
#define STOP_SIGN            1
#define SPEED_LIMIT_40_SIGN  2
#define SPEED_LIMIT_80_SIGN  3

/** @function main */
int main(int argc, char** argv)
{
	int sign_recog_result = NO_MATCH;
	speed_40 = imread("speed_40.bmp", 0);
	speed_80 = imread("speed_80.bmp", 0);

	// you run your program on these three examples (uncomment the two lines below)
	//string sign_name = "stop4";
	//string sign_name = "speedsign12";
	//string sign_name = "speedsign13";
	//string sign_name = "speedsign14";
	//string sign_name = "speedsign3";
	string sign_name = "speedsign4";
	//string sign_name = "speedsign5";
	string final_sign_input_name = sign_name + ".jpg";
	string final_sign_output_name = sign_name + "_result" + ".jpg";

	/// Load source image and convert it to gray
	src = imread(final_sign_input_name, 1);

	/// Convert image to gray and blur it
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(5, 5));
	warped_result = Mat(Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());

	// here you add the code to do the recognition, and set the variable 
	// sign_recog_result to one of STOP_SIGN, SPEED_LIMIT_40_SIGN, SPEED_LIMIT_80_SIGN, or NO_MATCH

	string text;
	if (sign_recog_result == SPEED_LIMIT_40_SIGN) text = "Speed 40";
	else if (sign_recog_result == SPEED_LIMIT_80_SIGN) text = "Speed 80";
	else if (sign_recog_result == STOP_SIGN) text = "Stop";
	else if (sign_recog_result == NO_MATCH) text = "Fail";

	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 2;
	int thickness = 3;
	cv::Point textOrg(10, 130);
	cv::putText(src, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

	/// Create Window
	char* source_window = "Result";
	namedWindow(source_window, WINDOW_AUTOSIZE);
	imshow(source_window, src);
	imwrite(final_sign_output_name, src);

	waitKey(0);

	return(0);
}
