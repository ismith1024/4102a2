#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/opencv.hpp"
//#include<vector>
#include<iostream>
#include<fstream>

enum SignType{
    NO_MATCH,
    STOP_SIGN,
    SPEED_LIMIT_40_SIGN,
    SPEED_LIMIT_80_SIGN     
};

class ImageClassifier{
    
private:
    static cv::Mat speed_80;
    static cv::Mat speed_40;    

public:
    ImageClassifier();
    ~ImageClassifier();
    SignType classifySign(cv::Mat);
    
};