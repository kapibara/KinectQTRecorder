#ifndef BADIMAGEDETECTOR_H
#define BADIMAGEDETECTOR_H

class BadImageDetector
{
public:
    BadImageDetector();

    enum State {GOOD ,TOO_NEAR, TOO_FAR};

    void setImageSize(int width, int height)
    {
        width_ = width;
        height_ = height_;
    }

    void setMinDepthThr(double minDepthThr){
        minDepthThr_ = minDepthThr;
    }

    void setCriticalArea(int x, int y, int width, int height )
    {
        x_left_ = x;
        y_left_ = y;
        x_right_ = x + width;
        y_right_ = y + height;
    }

    void setMissingThreashold(double threashold){
        missingThr_ = threashold;
    }

    State checkImage(unsigned short *buffer);

private:
    bool isValid(unsigned short value){
        return value!=MISSING_PIXEL;
    }

    int sub2index(int i,int j){
        return i + j*width_;
    }

    int width_;
    int height_;

    int x_left_,y_left_, x_right_,y_right_;

    double missingThr_;
    double minDepthThr_;
    double notFarThr_;

    const unsigned short NEAR_PIXEL;
    const unsigned short FAR_PIXEL;
    const unsigned short MISSING_PIXEL;
};

#endif // BADIMAGEDETECTOR_H
