#include "qvideosource.h"

#include <opencv2/opencv.hpp>
#include <QVideoFrame>
#include <iostream>


using namespace std;

QVideoSource::QVideoSource(QMutex &lock, bool &pause): device_(CV_CAP_OPENNI),MAX_DEPTH(5000),lock_(lock), pause_(pause)
{
    cout << "OpenCV version installed: " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << endl;

    device_.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_VGA_30HZ );
    device_.set(CV_CAP_PROP_OPENNI_REGISTRATION, true); //seems not to work too gut

    width_ = device_.get(CV_CAP_PROP_FRAME_WIDTH);
    height_ = device_.get(CV_CAP_PROP_FRAME_HEIGHT);

    depthdataptr_ = new uchar[width_*height_*3];
    rgbdataptr_ = new uchar[width_*height_*3];
    depthoriginal_ = new unsigned short[width_*height_];

}

void QVideoSource::pause(){
    runLock_.lock();

    pause_ = true;

    runLock_.unlock();

}

void QVideoSource::resume(){

    runLock_.lock();

    pause_ = false;

    runLock_.unlock();

    pauseCond_.wakeAll();

}

void QVideoSource::run()
{
/*1. the declaration could be as well put inside the loop -> the constructor will be called each time,
     it is ok -> the buffer is used, not allocated, but it is still overhead
  2. Use direct buffer; Mat does not seem to be working too good with QImage


    QImage rgb(rgbBuffer_.data,rgbBuffer_.size().width,rgbBuffer_.size().height,QImage::Format_RGB888);
    QImage depth(scaledDepthBuffer_.data,scaledDepthBuffer_.size().width,scaledDepthBuffer_.size().height,QImage::Format_RGB888);*/

    while(1){


        runLock_.lock();

        if(pause_){

            pauseCond_.wait(&runLock_); //locks even if lock is not locked...

        }

        runLock_.unlock();

        device_.grab();

        /*the buffer bgrBuffer_ could be used directly, but seems that retrieve have a tendency to reallocate it*/
        device_.retrieve( depthBuffer_, CV_CAP_OPENNI_DEPTH_MAP);
        device_.retrieve( bgrBuffer_, CV_CAP_OPENNI_BGR_IMAGE );

        lock_.lock();

            convertColor();

        lock_.unlock();

/*So the story
try 1: path data through a signal good thing: implicit data sharing: if rgb and depth get modified, they are copied, otherwise a shallow copy is created
          bad news: rgb and depth share buffers, which basically mean they dont know when they get modified;
          endeffect: no copying, but some frames will look strange; locking does not entirely help also, because copying seems to happen in a separate thread
try 2: give pointers to buffers; no arguments for a signal*/
        emit framesReady();

        msleep(10); //dont sleep more then 100 -> bad images coming out
    }
}

void QVideoSource::convertColor()
{
    int rowcount = depthBuffer_.rows;
    int colcount = depthBuffer_.cols;
    const unsigned short *idataptr;
    const uchar *bgrdataptr;

    if (depthBuffer_.isContinuous() & bgrBuffer_.isContinuous())
    {
        colcount*= rowcount;
        rowcount = 1;
    }

    for(int i=0; i<rowcount; i++){

        idataptr = depthBuffer_.ptr<unsigned short>(i);
        bgrdataptr = bgrBuffer_.ptr(i);

        for(int j=0; j<colcount; j++){
            depthdataptr_[3*j+3*i*colcount] = idataptr[j]/MAX_DEPTH*256.0;
            depthdataptr_[3*j+1+3*i*colcount] = idataptr[j]/MAX_DEPTH*256.0;
            depthdataptr_[3*j+2+3*i*colcount] = idataptr[j]/MAX_DEPTH*256.0;
            rgbdataptr_[3*j+3*i*colcount] = bgrdataptr[3*j+2];
            rgbdataptr_[3*j+1+3*i*colcount] = bgrdataptr[3*j+1];
            rgbdataptr_[3*j+2+3*i*colcount] = bgrdataptr[3*j];
            depthoriginal_[j+i*colcount] = idataptr[j];
        }
    }
}

