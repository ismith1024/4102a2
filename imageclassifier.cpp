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
    cv::Mat src_gray;
    cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
    cv::blur(src_gray, /*src_gray*/target, cv::Size(5, 5));
    //cv::Mat warped_result = cv::Mat(cv::Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());

    return 0;
}



//////////////
/// ImageClassifier::classifysign
/// Finds the closest match among stop sign, speed 40, speed 80 
///
SignType ImageClassifier::classifySign(cv::Mat& theSign){
   

    //To do this you should use the routines Canny, findContours, and approxPolyDP. 
    
    //find Canny edges
    const int canny_thresh = 120;  //where canny_thresh is set to 120
    cv::Mat detEdges;
    cv::Canny(theSign, detEdges, canny_thresh, (2 * canny_thresh), 3); 
    
    //find contours
    std::vector<std::vector<cv::Point>> contours;
    //cv::MatVector contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(detEdges, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    
    //Fit polygons to the contours    
    std::vector<std::vector<cv::Point>> polygons;
    //cv::MatVector polygons;
    
    //std::cout << "Found " << contours.size() << " contours." << std::endl;
    //int i = -1;
    
    for(auto& e1: contours){
        //std::cout << "Contour #" << ++i << ": " << e1.size() << " points" << std::endl;
        std::vector<cv::Point> polygon;
        cv::approxPolyDP(e1, polygon, (e1.size() * .008), true);
        polygons.push_back(polygon);
    }
    
    std::cout << "Polygons detected: " << polygons.size() << std::endl;
    /*for(auto& e1: polygons){
        for(auto& e2: e1){
            std::cout << "Point: " << e2 << std::endl;
        }        
    }*/
    
    //These are the parameter values which worked for me so you can use them if you want, but you do not need to use them. 
//The second part of the program will use the routines 

    //getPerspectiveTransform
    //warpPerspective along with images speed_40.bmp, and speed_80.bmp to tell the type of speed limit sign
    // but they are used only when you have determined that you are indeed looking at a speed limit sign and not a stop sign. 
    
    //Finally the routine matchtemplate( , , cv2.TM_CCOEFF_NORMED) is the best way to compare the stored template (from the .bmp files) for the 80/40 speed limit signs and the warped input sub-image.
    
   
    
    
    return NO_MATCH;
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
