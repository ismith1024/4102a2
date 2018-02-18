#include"imageclassifier.h"


ImageClassifier::ImageClassifier(){
    speed_80 = cv::imread("speed_80.bmp", 0);
    speed_40 = cv::imread("speed_40.bmp", 0);

    if(speed_80.data == NULL || speed_40.data == NULL){
        throw "Files not found";
    }
    
}

ImageClassifier::~ImageClassifier(){
    
}


//////////////////////////
/// ImageClassifier:: prepare(Mat&)
/// Convert image to gray and blur it
int ImageClassifier::prepare(cv::Mat& src, cv::Mat& target){
    const int thr = 128;
    const int maxVal = 255;
    
    cv::Mat src_gray;
    cv::Mat thresh;
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cv::threshold(src_gray, thresh, thr, maxVal,cv::THRESH_BINARY);
    cv::blur(/*src_gray*/ thresh, target, cv::Size(5, 5));
    //cv::threshold(target, targ2, 60, 255, cv::THRESH_BINARY);//[1];
    //cv::Mat warped_result = cv::Mat(cv::Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());
    //target = targ2;
    return 0;
}



//////////////
/// ImageClassifier::classifysign
/// Finds the closest match among stop sign, speed 40, speed 80 
/// Uses Canny(), findContours(), and approxPolyDP(). 
SignType ImageClassifier::classifySign(cv::Mat& theSign){
    
    ///////////////////////////////////////////////////////////////////////////////////
    //////PARMS TO EXPERIMENT WITH
    // CANNY 
    const int canny_thresh = 120;  //lower Canny threshold  -- default was 120
    const float canny_mult = 2.0;  //multiplier for the upper threshold -- default was 2
    //APPROXPOLYDP 
    const float epsilon = 0.04;   //contour approximation -- default was 0.008
    //SEARCHDEPTH -- only want to look at the largest contours for sign outlines
    const int searchSize = 4;
    ///////////////////////////////////////////////////////////////////////////////////
    
    //default return = fail
    SignType ret = NO_MATCH;
    
    //find Canny edges
    cv::Mat detEdges;
    cv::Canny(theSign, detEdges, canny_thresh, (canny_mult * canny_thresh), 3); 
    
    //find contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(detEdges, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    
    //Fit polygons to the contours    
    std::vector<std::vector<cv::Point>> polygons;
    
    //detet simple polygon
    //ref: https://www.pyimagesearch.com/2016/02/08/opencv-shape-detection/
    for(auto& e1: contours){
        std::vector<cv::Point> polygon;
        std::vector<cv::Point> convexHull;
        
        //arc length of the contour
        float perimieter = cv::arcLength(e1, true);

        cv::approxPolyDP(e1, polygon, (epsilon * perimieter), true); //0.008  ////TODO: should this be epsilon, or (epsilton * e1.size())?
        //cv::convexHull(polygon, convexHull);
        //polygons.push_back(convexHull);
        polygons.push_back(polygon);
    }
    
    std::cout << "Polygons detected: " << polygons.size() << std::endl;
    
    //sort the polygons by area -- the sign is likely to be one of the largest contours
    sort(polygons.begin(), polygons.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) -> bool{ 
        std::vector<cv::Point> hull1;
        std::vector<cv::Point> hull2;
        cv::convexHull(a, hull1);
        cv::convexHull(b, hull2);
        return cv::contourArea(hull1) > cv::contourArea(hull2); 
    });
    

    std::vector<cv::Point> bigHull;
    std::vector<cv::Point> allPoints;
    //check the corners of the polygons from largest to smallest.  If the largest is an octagon, return STOP_SIGN
    for(auto it = polygons.begin(); it != polygons.end() && it != (polygons.begin() + searchSize); ++it){ 
        for(auto& e1: *it) allPoints.push_back(e1);
        std::cout << it->size() << std::endl;
        
//if(it->size() == 8) octagons.push_back(*it);
        
        if(/*e1.*/it->size() == 8) {
            ret = STOP_SIGN;
            //return ret;
        }
        //else if(/*e1*/it->size() == 4) break;
    }
    
    //cv::convexHull(allPoints, bigHull);
    //polygons.insert(polygons.begin(), bigHull);
    
        //cv::Mat warped_result = cv::Mat(cv::Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());

   
////// TODO: Perspective transform    
    //getPerspectiveTransform
    //warpPerspective along with images speed_40.bmp, and speed_80.bmp to tell the type of speed limit sign
    // but they are used only when you have determined that you are indeed looking at a speed limit sign and not a stop sign. 
    
    //Finally the routine matchtemplate( , , cv2.TM_CCOEFF_NORMED) is the best way to compare the stored template (from the .bmp files) for the 80/40 speed limit signs and the warped input sub-image.
    
    ///////////////    
    /// Draw the polygons to help visualize what the classifier is doing
    //ref: https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
    cv::Mat drawing = cv::Mat::zeros(theSign.size(), CV_8UC3);
    
    cv::RNG rng(12345);    
    for( int i = 0; i < searchSize+1/*contours.size()* / && i < octagons.size()*/ ; i++ ){
       cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, polygons /*octagons*/, i, color, 2, 8, hierarchy, 0, cv::Point() );
    }
    

    /// Show in a window
    cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Contours", drawing );
    cv::waitKey(0);
    
    return ret;
}


/*The goal of the program you are going to write is to classify street signs, in
particular stop signs and two different speed limit signs (40km and 80km). There
are seven images that you will use to test your program; speedsign3.jpg – a 40km
speed sign, speedsign4.jpg – a 100km speed sign (no match), speedsign5.jpg – a
40km speed sign, speedsign12.jpg – an 80km speed sign, speedsign13.jpg – an
80km speed sign, speedsign14.jpg – an 80km speed sign, and finally stop4.jpg – a
stop sign. I have given you a program called classify_sign_template.cpp, which
you can extend to do the classification. The result will be a copy of the input
image with the appropriate label (a stop sign, speed limit 40, speed limit 80, or no
match) for each of these seven input images. There are two parts to the program,
the first decides on which type of sign you are looking at (stop sign or speed
limit). To do this you should use the routines Canny, findContours, and
approxPolyDP. To help you I am giving you some information on the parameters
of these routines. First of all, Canny(, , canny_thresh, canny_thresh*2, 3) where
canny_thresh is set to 120 (this worked for me). Also, findContours(, , ,
RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0)), and approxPolyDP(, ,
contours[i].size()*.008, true). These are the parameter values which worked
for me so you can use them if you want, but you do not need to use them. The
second part of the program will use the routines getPerspectiveTransform ,
warpPerspective along with images speed_40.bmp, and speed_80.bmp to tell the
type of speed limit sign, but they are used only when you have determined that
you are indeed looking at a speed limit sign and not a stop sign. Finally the
routine matchtemplate( , , cv2.TM_CCOEFF_NORMED) is the best way to compare
the stored template (from the .bmp files) for the 80/40 speed limit signs and the
warped input sub-image. Hand in the source code, and tell me the version of
OpenCV that you are using. You should also include the four output files that you
have produced, one for each of the seven test cases.*/
