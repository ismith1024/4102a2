#include"imageclassifier.h"

///perspectivetransform points order:
// Top left 
// Bottom left
// Bottom bottom 
// Top right 

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
    cv::blur(thresh, target, cv::Size(5, 5));
    return 0;
}



//////////////
/// ImageClassifier::classifysign
/// Finds the closest match among stop sign, speed 40, speed 80 
/// Uses Canny(), findContours(), and approxPolyDP(). 
SignType ImageClassifier::classifySign(cv::Mat& aSign){
    
    cv::Mat theSign;
    prepare(aSign, theSign);
    
    ///////////////////////////////////////////////////////////////////////////////////
    //////PARMS TO EXPERIMENT WITH
    // CANNY 
    const int canny_thresh = 120;  //lower Canny threshold  -- default was 120
    const float canny_mult = 2.0;  //multiplier for the upper threshold -- default was 2
    //APPROXPOLYDP 
    const float epsilon = 0.04;   //contour approximation -- default was 0.008
    //SEARCHDEPTH -- only want to look at the largest contours for sign outlines
    const int searchSize = 4;
    //CLASSIFIER CONFIDENCE
    const float classConf = 0.6;
    ///////////////////////////////////////////////////////////////////////////////////
    float best40 = 0.0;
    float best80 = 0.0;
    
    cv::Mat trans( 2, 4, CV_32FC1 );
    trans = cv::Mat::zeros( theSign.rows, theSign.cols, theSign.type() );
    
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

        cv::approxPolyDP(e1, polygon, (epsilon * perimieter), true); //0.008  
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
        //for(auto& e1: *it) allPoints.push_back(e1);
        //std::cout << it->size() << std::endl;
        
        if(it->size() == 8) {
            ret = STOP_SIGN;
            //return ret;
        }
    }
    
    ////// Some convex hull stuff in case the contour isn't closed
    //cv::convexHull(allPoints, bigHull);
    //polygons.insert(polygons.begin(), bigHull);
    
    
    ////// Perspective transform to find speed sign
    std::vector<cv::Point2f> dest;
        dest.push_back(cv::Point2f(0.0, 0.0));
        dest.push_back(cv::Point2f(0.0, 0.0 + ImageClassifier::WARPED_YSIZE -1));
        dest.push_back(cv::Point2f(0.0 + ImageClassifier::WARPED_XSIZE -1, 0.0 + ImageClassifier::WARPED_YSIZE -1));
        dest.push_back(cv::Point2f(0.0 + ImageClassifier::WARPED_XSIZE -1, 0.0));

    // they are used when it is determined that it is a speed limit sign and not a stop sign. 
    //ref: http://opencvexamples.blogspot.com/2014/01/perspective-transform.html
    if(ret != STOP_SIGN){
        
        //check the n largest polygons with four sides
        for(auto it = polygons.begin(); it != polygons.end() && it != (polygons.begin() + searchSize); ++it){
            if(it->size() == 4){
                               
                //vector to hold the four points of the skewed image
                //need to add the points in the correct order
                std::vector<cv::Point2f> source;
                float tl = std::numeric_limits<float>::infinity();
                float br = 0;
                cv::Point2f topLeft;
                cv::Point2f bottomRight;
                cv::Point2f bottomLeft;
                cv::Point2f topRight;
                int lowerY = 0;
                
                for(auto& e1: *it){
                    if(e1.x + e1.y > br) {
                        br = e1.x + e1.y;
                        bottomRight = e1;
                    }
                    if(e1.x + e1.y < tl){
                        tl = e1.x + e1.y;
                        topLeft = e1;
                    }
                }
                
               for(auto& e1: *it){
                    if(e1.x + e1.y != tl && e1.x + e1.y != br){
                        if(e1.y > lowerY){;
                            if(lowerY == 0){
                                bottomLeft = e1;
                            } else{
                                topRight = bottomLeft;
                                bottomLeft = e1;
                            }                            
                            lowerY = e1.y;
                        } else topRight = e1;
                    }
                }
                source.push_back(topLeft);
                source.push_back(bottomLeft);
                source.push_back(bottomRight);
                source.push_back(topRight);     
                
                cv::Mat warped_result = cv::Mat(cv::Size(ImageClassifier::WARPED_XSIZE, ImageClassifier::WARPED_YSIZE), 0);
                
                //getPerspectiveTransform
                trans = cv::getPerspectiveTransform(source, dest);
                
                //warpPerspective                  
                cv::warpPerspective(theSign, warped_result, trans, warped_result.size());

                //Check the warped and resized image for the best match with the templates
                float chk40 = checkSignFor40(warped_result, classConf);
                float chk80 = checkSignFor80(warped_result, classConf);
                                
                if(chk40 > best40) best40 = chk40;
                if(chk80 > best80) best80 = chk80;
                
            }     
            
        }        
    }   
    
    //if both 40 and 80 were under the  detection threshold, fail
    //else take the better match
    if(ret != STOP_SIGN){
        if(best40 == 0.0 && best80 == 0.0) ret = NO_MATCH;
        else if (best40 > best80) ret = SPEED_LIMIT_40_SIGN;
        else ret = SPEED_LIMIT_80_SIGN;
    }
    
    ///////////////    
    /// Draw the polygons to help visualize what the classifier is doing
    //ref: https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
    cv::Mat drawing = cv::Mat::zeros(theSign.size(), CV_8UC3);
    
    cv::RNG rng(12345);    
    for( int i = 0; i < searchSize + 1 && i < polygons.size(); i++ ){
       cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, polygons, i, color, 2, 8, hierarchy, 0, cv::Point() );
    }
    

    /// Show in a window
    cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Contours", drawing );
    cv::waitKey(0);
    
    return ret;
}


///Finally the routine matchtemplate( , , cv2.TM_CCOEFF_NORMED) is the best way to compare the stored template (from the .bmp files) for the 80/40 speed limit signs and the warped input sub-image.  

//////// imageclassifier::checksignfor40()
/// Checks to see if the sign is a 40 km/h speed sign, within the confidence specified
/// Returns the classifier confidence if it exceeds the threshold
/// And 0.0 otherwise
/// Reference: 
/// https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
float ImageClassifier::checkSignFor40(cv::Mat& sample, float conf){
    
    const char* image_window = "Source Image";
    const char* result_window = "Result window";
    const char* template_window = "Template window";
        
    /// Source image to display
    cv::Mat img = sample;
    cv::Mat templ = speed_40;
    
    cv::Mat img_display;
    img.copyTo(img_display);
  
    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    cv::Mat result;
    
    result.create( result_rows, result_cols, CV_32FC1 );

    /// Do the matching, worked better without normalizing
    int match_method = CV_TM_CCOEFF_NORMED;    
    cv::matchTemplate( img, templ, result, match_method );
    //cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;
    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
        { matchLoc = minLoc; }
    else
        { matchLoc = maxLoc; }
        
    std::cout << "40 Min value: " << minVal << " ... Max Val: " << maxVal << std::endl;

    /// Show me what you got
    cv::rectangle( img_display, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    
    cv::imshow( image_window, img_display );
    cv::imshow( result_window, result );
    cv::imshow( template_window, templ );
    
    cv::waitKey(0);
    
    if(minVal > conf) return minVal;
    
    return 0.0;
}    
 

//////// imageclassifier::checksignfor80()
/// Checks to see if the sign is a 80 km/h speed sign, within the confidence specified
/// Returns the classifier confidence if it exceeds the threshold
/// And 0.0 otherwise
/// Reference: 
/// https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/template_matching/template_matching.html
float ImageClassifier::checkSignFor80(cv::Mat& sample, float conf){
    
    const char* image_window = "Source Image";
    const char* result_window = "Result window";
    const char* template_window = "Template window";    
    
    /// Source image to display
    cv::Mat img = sample;
    cv::Mat templ = speed_80;
    
    cv::Mat img_display;
    img.copyTo(img_display);
  
    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    cv::Mat result;
    
    result.create( result_rows, result_cols, CV_32FC1 );

    /// Do the matching, worked better without normalizing
    int match_method = CV_TM_CCOEFF_NORMED;    
    cv::matchTemplate( img, templ, result, match_method );
    //cv::normalize( result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;
    cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
        { matchLoc = minLoc; }
    else
        { matchLoc = maxLoc; }
        
    std::cout << "80 Min value: " << minVal << " ... Max Val: " << maxVal << std::endl;

    /// Show me what you got
    cv::rectangle( img_display, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), cv::Scalar::all(0), 2, 8, 0 );
    
    cv::imshow( image_window, img_display );
    cv::imshow( result_window, result );
    cv::imshow( template_window, templ );
    
    cv::waitKey(0);
    
    if(minVal > conf) return minVal;
    
    return 0.0;    
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
