#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include<fstream>

///////////
////
//// Usage notes: requires c++14
//// To compile: g++ -std=c++14 a2q2.cpp -o a2q2 `pkg-config --libs opencv`
////
//// openCV version used:
//// OpenCV version : 3.3.1
//// Major version : 3
//// Minor version : 3
//// Subminor version : 1
////

////PURPOSE
/*Write a program that finds Canny edges in the file tracks.jpg, and then finds lines
in these edges using the Hough transform. The easiest way to do this is to take the
programs edge.cpp and houghlines.cpp that are in the assignment files and then
combine them. The key parameter in edge detection is the edge threshold which
can be varied by the track bar as in edge.cpp (the other parameters worked fine for
me). Then for a given edge threshold find the lines in the resulting edges using the
Hough transform. Then draw these lines in the image as is done in the program
houghlines.cpp. Again, the Hough parameters in houghlines.cpp worked well for
me. The goal is to have as output an image similar to detected track_lines.jpg,
with between two and ten detected lines (no more or less). You should submit
your software along with the final output image. This is an exercise in adjusting
the Canny and Hough transform parameters to give the desired results.*/

int main(int argc, const char** argv){
    
    //cv::Mat used in transforms
    cv::Mat gray, edge, cedge, cdst;
    
    //Edge threshold that generated 8 lines for me
    int edgeThresh = 100;
    
    //Parse args and open file if valid
    if(argc < 2){
        std::cout << "Usage: ./a2q2 <filename>" << std::endl;
        exit(-1);
    }
    
    std::string filename = argv[1];

    cv::Mat image = cv::imread(filename, 1);  
    if(image.empty()) {        
        std::cout << "Can not open " << filename << std::endl;
        exit(-2);
    } else{
        std::cout << "Attempting to find lines in image " << filename << std::endl;
    }
    
    //create a grayscale version of the image
    cedge.create(image.size(), image.type());
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Window for edge map
    cv::namedWindow("Edge map", 1);

    // blur the grayscale image
    cv::blur(gray, edge, cv::Size(5,5));

    // Run the edge detector on grayscale
    cv::Canny(edge, edge, edgeThresh, edgeThresh*3, 3);
    cedge = cv::Scalar::all(0);
    image.copyTo(cedge, edge);

    //Apply the Hough transform to the Canny edges and display the number of lines on the console
    cv::cvtColor(edge, cdst, cv::COLOR_GRAY2BGR);
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edge, lines, 1, CV_PI/180, 100, 0, 0 );    
    std::cout << "Hough Lines -- located " << lines.size() << " lines in image." << std::endl;

    //draw the Hough lines onto the source image
    for(size_t i = 0; i < lines.size(); i++){
        float rho = lines[i][0];
        float theta = lines[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cv::line( image, pt1, pt2, cv::Scalar(0,0,255), 3, CV_AA);
    }    
    
    //Display the results: edge map and Hough lines
    cv::imshow("source", image);
    cv::imshow("Edge map", cedge);
    
    cv::waitKey();

    //Write the tagged image to file
    std::string final_output_name = "Result--" + filename;
    cv::imwrite(final_output_name, image);
    
    return 0;    
}
