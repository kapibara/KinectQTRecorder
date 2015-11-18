#include "badimagedetector.h"

#include <iostream>

BadImageDetector::BadImageDetector():NEAR_PIXEL(600),FAR_PIXEL(1300),MISSING_PIXEL(0)
{
    width_ = 512;
    height_ = 424;
    x_left_ = 0.3*width_;
    y_left_ = 0.2*height_;
    x_right_ = 0.7*width_;
    y_right_ = 0.8*height_;
    missingThr_ = 0.6;
    minDepthThr_ = 0.1;
    notFarThr_ = 0.3;
}

BadImageDetector::State BadImageDetector::checkImage(unsigned short *buffer)
{
    int missing = 0;
    int tooNear = 0;
    int nearPixels = 0;

    unsigned short minValidDepth = 4000;

    std::cerr<< x_left_ << ":" << x_right_
             << y_left_ << ":" << y_right_ << std::endl;

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

    std::cerr << "percentageMissing: " << percentageMissing << ":" << missing
              << "percentageTooNear: " << percentageTooNear << ":" << tooNear
              << "percentageNotFar" << percentageNotFar;

    if (percentageMissing > missingThr_ | percentageTooNear > minDepthThr_){
        return State::TOO_NEAR;
    }

    if (percentageNotFar < notFarThr_){
        return State::TOO_FAR;
    }

    return State::GOOD;
}
