#include "qvideosource.h"

#include <QVideoFrame>
#include <iostream>


using namespace std;

Buffer2ImageConverter::Buffer2ImageConverter(QSize displaySize, QSize rgb_prealloc, QSize depth_prealloc){
    rgb_= rgb_prealloc;
    depth_= depth_prealloc;
    displaySize_= displaySize;

    locBufferRGB2Show_ = new uchar[rgb_prealloc.width()*rgb_prealloc.height()*3];
    locBufferDepth2Show_ = new uchar[depth_prealloc.width()*depth_prealloc.height()*3];
    locBufferDepth2Save_ = new uchar[depth_prealloc.width()*depth_prealloc.height()*sizeof(short)];
}

//make
void Buffer2ImageConverter::convertRGB2show(unsigned char *orig, QSize osize){

    if(rgb_ != osize){
        rgbToShowLast_ = QImage();
    }

    for(int i=0; i <osize.width()*osize.height(); i++){
        locBufferRGB2Show_[3*i] = orig[4*i];
        locBufferRGB2Show_[3*i+1] =  orig[4*i+1];
        locBufferRGB2Show_[3*i+2] =   orig[4*i+2];
    }

    rgbToShowLast_ = (QImage(locBufferRGB2Show_,osize.width(),osize.height(),QImage::Format_RGB888).scaledToWidth(displaySize_.width()));
}

//do not update anything
QImage Buffer2ImageConverter::convertRGB2save(const unsigned char *orig, QSize osize) const {

    if(rgb_ != osize){
        return QImage();
    }


    return QImage(locBufferRGB2Show_,osize.width(),osize.height(),QImage::Format_RGB888);
}

//toshow -> RGB image with 3 identical channels
void Buffer2ImageConverter::convertDepth2show(unsigned short *orig, QSize osize){

    if(depth_ != osize){
        depthToShowLast_ = QImage();
    }

    for(int i=0; i <osize.width()*osize.height(); i++){
        locBufferDepth2Show_[3*i] = orig[i]/20;
        locBufferDepth2Show_[3*i+1] =  locBufferDepth2Show_[3*i];
        locBufferDepth2Show_[3*i+2] =  locBufferDepth2Show_[3*i];
    }

    depthToShowLast_ = (QImage(locBufferDepth2Show_,osize.width(),osize.height(),QImage::Format_RGB888)).scaledToWidth(displaySize_.width());
}

QImage Buffer2ImageConverter::convertDepth2save(const unsigned short *orig, QSize osize) const{

    if(depth_ != osize){
        return QImage();
    }

    memcpy(locBufferDepth2Save_,orig,osize.width()*osize.height()*sizeof(short));

    return QImage(locBufferDepth2Save_,osize.width(),osize.height(),QImage::Format_ARGB4444_Premultiplied);
}

QVideoSource::QVideoSource(QMutex &lock, QSize displaySize): MAX_DEPTH(5000),lock_(lock)
{

    device_.open();

    std::cerr << "device: opened" << std::endl;
    depth_width_ = device_.width(Kinect2VideoSource::ImageType::IMAGE_DEPTH);
    depth_height_ = device_.height(Kinect2VideoSource::ImageType::IMAGE_DEPTH);

    color_width_ = device_.width(Kinect2VideoSource::ImageType::IMAGE_RGB);
    color_height_ = device_.height(Kinect2VideoSource::ImageType::IMAGE_RGB);

    converter_ = new Buffer2ImageConverter(displaySize,QSize(color_width_,color_height_),QSize(depth_width_,depth_height_));

    Ndb_ = device_.expectedBufferSize(Kinect2VideoSource::IMAGE_DEPTH);
    Nir_ = device_.expectedBufferSize(Kinect2VideoSource::IMAGE_IR);
    Nrgb_ = device_.expectedBufferSize(Kinect2VideoSource::IMAGE_RGB);

    depthBuffer_ = new unsigned char[Ndb_];
    rgbBuffer_ = new unsigned char[Nrgb_];
    irBuffer_ = new unsigned char[Nir_];

    depthIntrinsics_.fx_ = device_.getDepthIntrinsics().FocalLengthX;
    depthIntrinsics_.fy_ = device_.getDepthIntrinsics().FocalLengthY;
    depthIntrinsics_.cx_ = device_.getDepthIntrinsics().PrincipalPointX;
    depthIntrinsics_.cy_ = device_.getDepthIntrinsics().PrincipalPointY;

    queue_ = nullptr;
    pause_ = false;
    queueset_ = false;
}

void QVideoSource::createPC(pc &point_cloud) const
{
    device_.map(depthBuffer_,rgbBuffer_,point_cloud);
}

void QVideoSource::pause(){

    runLock_.lock();

    pause_ = true;

    runLock_.unlock();
}

void QVideoSource::resume(){

    pause_ = false;

    pauseCond_.wakeAll();

}

void QVideoSource::run()
{
/*1. the declaration could be as well put inside the loop -> the constructor will be called each time,
     it is ok -> the buffer is used, not allocated, but it is still overhead
  2. Use direct buffer; Mat does not seem to be working too good with QImage;*/

    while(1){

        bool success = true;

        //if saving is in a separate thread, the part that needs guards is only inside if block
        lock_.lock();

            success = success & device_.retrieve(depthBuffer_, Kinect2VideoSource::IMAGE_DEPTH);
            success = success & device_.retrieve(rgbBuffer_, Kinect2VideoSource::IMAGE_RGB );
            success = success & device_.retrieve(irBuffer_, Kinect2VideoSource::IMAGE_IR);

            if(success){
                converter_->convertDepth2show(static_cast<unsigned short*>((void*)depthBuffer_),QSize(depth_width_,depth_height_));
                converter_->convertRGB2show(rgbBuffer_,QSize(color_width_,color_height_));

            }
        lock_.unlock();

/*
        if(!success){
            //retrive will return false quite often
            msleep(10);
            continue;
        }
*/

        runLock_.lock();

        if(pause_){

            pauseCond_.wait(&runLock_); //locks even if lock is not locked...

        }

        if(success){
            if(queueset_ & queue_ != nullptr){

            //note that the queue will reject some frames if full
                queue_->push(ToFFrame(Buffer(depthBuffer_,Ndb_),
                             Buffer(converter_->getRGB2saveBuffer(),
                                    converter_->getRGB2saveBufferSize()),
                             Buffer(irBuffer_,Nir_)));
            }
        }

        runLock_.unlock();

/*So the story
try 1: path data through a signal good thing: implicit data sharing: if rgb and depth get modified, they are copied, otherwise a shallow copy is created
          bad news: rgb and depth share buffers, which basically mean they dont know when they get modified;
          endeffect: no copying, but some frames will look strange; locking does not entirely help also, because copying seems to happen in a separate thread
try 2: give pointers to buffers; no arguments for a signal*/

        emit framesReady();

        msleep(10); //dont sleep more then 100 -> bad images coming out
    }
}

