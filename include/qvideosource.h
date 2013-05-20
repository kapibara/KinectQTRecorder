#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <opencv2/opencv.hpp>

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>


/*Kinect video data los -> put the reader in a separate thread*/

class QVideoSource: public QThread
{
    Q_OBJECT
public:
    QVideoSource(QMutex &lock, bool &pause);

    uchar* getRGBBuffer() {return rgbdataptr_;}
    uchar* getScaledDepthBuffer() {return depthdataptr_;}
    unsigned short* getOriginalDepthBuffer() {return depthoriginal_;}

    int getFrameWidth() {return width_;}
    int getFrameHeight() {return height_;}

    void run();
    void pause();
    void resume();
signals:
    /*if we sit in a separate slot, can be connected in a standard way, i.e. quied (no other way around)
      do not use in the same thread with queueing - it is emited from an infinite loop ->
      will never be executed and will cause memory overflow*/
    void framesReady();

private:
    /* transform colors: BGR -> RGB; depth 16bit -> grayscale RGB888*/
    void convertColor();

    QWaitCondition pauseCond_;

    cv::VideoCapture device_;
    cv::Mat depthBuffer_; //receive depth data
    cv::Mat bgrBuffer_; //receive bgr data

    unsigned short *depthoriginal_; //original depth data
    uchar *depthdataptr_; //ready-to-show-depth
    uchar *rgbdataptr_; //ready-to-show-rgb

    int width_,height_;

    bool pause_;

    QMutex &lock_;
    QMutex runLock_;

    const double MAX_DEPTH; //a constant to convert to greyscale
};

#endif // VIDEOSOURCE_H
