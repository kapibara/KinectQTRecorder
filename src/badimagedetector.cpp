#include "badimagedetector.h"

#include <iostream>

#include <opencv2/opencv.hpp>

BadImageDetector::BadImageDetector():NEAR_PIXEL(470),FAR_PIXEL(1200),MISSING_PIXEL(0)
{
    width_ = 640;
    height_ = 480;
    x_left_ = 0.2*width_;
    y_left_ = 0.1*height_;
    x_right_ = 0.8*width_;
    y_right_ = 0.9*height_;
    missingThr_ = 0.2;
    minDepthThr_ = 0.1;
    notFarThr_ = 0.5;
}

BadImageDetector::State BadImageDetector::checkImage(unsigned short *buffer)
{
    int missing = 0;
    int tooNear = 0;
    int nearPixels = 0;

    unsigned short minValidDepth = 4000;

    for(int i=x_left_; i< x_right_; i++){
        for(int j=y_left_; j< y_right_; j++){
            if(isValid(buffer[sub2index(i,j)])){
                if (buffer[sub2index(i,j)] < minValidDepth){
                    minValidDepth = buffer[sub2index(i,j)];
                }
                if (buffer[sub2index(i,j)] < NEAR_PIXEL){
                    tooNear++;
                }
                if (buffer[sub2index(i,j)] < FAR_PIXEL){
                    nearPixels++;
                }

            }else{
                missing++;
            }
        }
    }

    double percentageMissing = (double)missing/((x_right_ - x_left_)*(y_right_ - y_left_));
    double percentageTooNear = (double)tooNear/((x_right_ - x_left_)*(y_right_ - y_left_));
    double percentageNotFar = (double)nearPixels/((x_right_ - x_left_)*(y_right_ - y_left_));

    if (percentageMissing>missingThr_ | percentageTooNear>minDepthThr_){
        return State::TOO_NEAR;
    }

    if (percentageNotFar < notFarThr_){
        return State::TOO_FAR;
    }

    return State::GOOD;
}
