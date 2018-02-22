#ifndef IMCLASS_H
#define IMCLASS_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include<vector>
#include<iostream>
#include<fstream>

enum SignType{
    NO_MATCH,
    STOP_SIGN,
    SPEED_LIMIT_40_SIGN,
    SPEED_LIMIT_80_SIGN     
};



class ImageClassifier{

    static const int WARPED_XSIZE = 200;
    static const int WARPED_YSIZE = 300;
    
    
private:
    cv::Mat speed_80;
    cv::Mat speed_40;

public:
    ImageClassifier();
    ~ImageClassifier();
    int prepare(cv::Mat&, cv::Mat&);
    SignType classifySign(cv::Mat&);
    bool checkSignFor40(cv::Mat&, float);
    bool checkSignFor80(cv::Mat&, float);
    
};

#endif // IMCLASS_H