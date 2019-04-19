# Computer Vision COMP4102
### Assignment 2 - Sign Classifier; Hough Lines
This project contains two parts:
 - Using the opencv C++ library and traditional computer vision, classify street signs
 - Using a Canny edge detector, find the Hough Lines in an image

### Table of Contents

1. [Libraries and Installation](#installation)
2. [Project Description](#motivation)
3. [Files](#files)
4. [Findings](#results)

## Libraries and Installation <a name="installation"></a>

Libraries used:
The project uses the openCV library for linear algebra operations.  The library is licensed using the 3-clause BSD license, linked here:
https://opencv.org/license/

OpenCV version : 3.3.1
Major version : 3
Minor version : 3
Subminor version : 1

Both parts of the project are written un C++ using the C++14 standard.

## Project Description<a name="motivation"></a>

### Part 1:
The purpose of the exercise is to use traditional computer vision to classify street signs.
 - Convert the image to grayscale and supress noise using gaussian blur
 - Detect edges using an adaptive Canny detector (thresholds obtained form image intensity statistics)
 - Detect contours using these edges, and fit polygons
 - Close the polygons usign a convex hull
 - Also determine the area of the polygons using convex hull
 - Sort the polygons by size (area)
 - Examine the *k* largest polygons and count verices
 - If an octagon is found, classify as a stop sign
 - Else, if a quadrilateral is found, warp the contour to match the dimensions of specific examples for comparison
 - If the computed similarity is within *detection_threshold* of an example, classify the sign as such
 - If no matches are found, return *no_match*  

![80 sign](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q1-SignClassifier/Result--speedsign12.jpg "80 Sign")  
80 Sign  
![Unknown sign](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q1-SignClassifier/Result--speedsign4.jpg "Unknown Sign")  
Sign not in classification examples  
![40 sign](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q1-SignClassifier/Result--speedsign5.jpg "40 Sign")  
40 Sign  
![Stop sign](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q1-SignClassifier/Result--stop4.jpg "Stop Sign")  
Stop sign  


### Part 2:
The purpose of this exercise is to find Canny edges a file, and then find lines in these edges using the Hough transform.  

![Source](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q2-HoughLines/track.jpg "Source")  
Source image  
![Result](https://github.com/ismith1024/Computer-Vision-Assignment-2/blob/master/Q2-HoughLines/Result--track.jpg "Result")  
Result  

## Files <a name="files"></a>

### Part 1:
 - To Compile:
    ``g++ -std=c++14 a2q1.cpp imageclassifier.cpp -o a2q1 `pkg-config --libs opencv` ``
 - PLEASE NOTE: The image filename is passed to the program as a command line parameter (as opposed to a hardcoded string like the template code):
    `./a2q1 speedsign5.jpg`


### Part 2:
 - To compile:
    ``g++ -std=c++14 a2q2.cpp -o a2q2 `pkg-config --libs opencv` ``
 - The filename is a parameter as in Q1
    `./a2q2 track.jpg`

## Findings<a name="results"></a>

### Part 1:

 - The image classifier relies on looking exclusively at the 6 largest polygons to find signs.  This is to avoid false positive stop signs when arbitrary octagons are found in the background noise, and can be interpreted as "I assume the sign is close to the camera".
 - The Canny threshold is adaptive and is computed using the median pixel value in the image, so this assumes the grayscale image of the sign has high contrast
 - The blur size assmes that the images are of comparable pixel dimensions to the image files used in the assignment -- cv::Size(4,4) worked best for me
 - The minimum matchtemplate() threshold for the 40/80 signs is set slightly higher than the matchtemplate() value for the 100 km/h sign.  I am assuming that the 100 sign in the assignment is "typical" when setting the minimum threshold for 40/80.  In a real application, I would look at the statistical values for many 100 km/h signs to set the 40/80 threshold.

### Part 2:

I was able to obtain 8 lines by setting edgeThresh to 100.








