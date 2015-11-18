#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>
#include "syncqueue.h"

#include "kinect2videosource.h"

/*Kinect video data los -> put the reader in a separate thread*/

struct Intrinsics
{
    //Calibration
    float cx_;
    float cy_;
    float fx_;
    float fy_;
};


class Buffer2ImageConverter
{
public:
    Buffer2ImageConverter(QSize displaySize, QSize rgb_prealloc, QSize depth_prealloc);

    void convertRGB2show(unsigned char *orig, QSize osize);
    QImage convertRGB2save(const unsigned char *orig, QSize osize) const;

    void convertDepth2show(unsigned short *orig, QSize osize);
    QImage convertDepth2save(const unsigned short *orig, QSize osize) const;

    const QImage &getLastRGB() const{
        return rgbToShowLast_;
    }

    /*this is nasty*/
    unsigned char *getRGB2saveBuffer() const{
        return locBufferRGB2Show_;
    }
    size_t getRGB2saveBufferSize() const{
        return rgb_.width()*rgb_.height()*3;
    }

    const QImage &getLastDepth() const{
        return depthToShowLast_;
    }

private:
    unsigned char *locBufferRGB2Show_;
    unsigned char *locBufferDepth2Show_;
    unsigned char *locBufferDepth2Save_;

    QSize rgb_;
    QSize depth_;
    QSize displaySize_;

    QImage rgbToShowLast_;
    QImage depthToShowLast_;
};

class QVideoSource: public QThread
{
    Q_OBJECT
public:
    QVideoSource(QMutex &lock, QSize displaySize = QSize(640,480) );

    uchar* getOriginalRGBBuffer() {return rgbBuffer_;}
    unsigned char getRGBPixelSize() const {return device_.pixelSize(Kinect2VideoSource::ImageType::IMAGE_RGB);}

    unsigned short* getOriginalDepthBuffer() {return (unsigned short*)static_cast<void *>(depthBuffer_);}
    unsigned char  getDepthPixelSize() const {return  device_.pixelSize(Kinect2VideoSource::ImageType::IMAGE_DEPTH);}

    unsigned short* getOriginalIrBuffer() {return (unsigned short*)static_cast<void *>(irBuffer_);}


    QSize getFrameSize(Kinect2VideoSource::ImageType type){
        switch(type){
            case Kinect2VideoSource::ImageType::IMAGE_RGB:
                return QSize(color_width_,color_height_);
            case Kinect2VideoSource::ImageType::IMAGE_DEPTH:
            case Kinect2VideoSource::ImageType::IMAGE_IR:
                return QSize(depth_width_,depth_height_);
            default:
            return QSize();
        }
    }

    int getFrameWidth(Kinect2VideoSource::ImageType type) {
        switch(type){
            case Kinect2VideoSource::ImageType::IMAGE_RGB:
                return color_width_;
            case Kinect2VideoSource::ImageType::IMAGE_DEPTH:
            case Kinect2VideoSource::ImageType::IMAGE_IR:
                return depth_width_;
            default:
            return 0;
        }
    }

    int getFrameHeight(Kinect2VideoSource::ImageType type) {
        switch(type){
            case Kinect2VideoSource::ImageType::IMAGE_RGB:
                return color_height_;
            case Kinect2VideoSource::ImageType::IMAGE_DEPTH:
            case Kinect2VideoSource::ImageType::IMAGE_IR:
                return depth_height_;
            default:
            return 0;
        }

    }

    void run();
    void pause();
    void resume();

    const QImage &getLastRGB() const {return converter_->getLastRGB();}
    const QImage &getLastDepth() const {return converter_->getLastDepth();}

    void setQueue(SyncQueue<QMutex,ToFFrame> *queue){
        runLock_.lock();
            queue_ = queue;
            queueset_ = true;
        runLock_.unlock();
    }
    void unsetQueue(){
        runLock_.lock();
            queueset_ = false;
            queue_ = nullptr;
        runLock_.unlock();
    }

    QImage getRGBToSave() const {return converter_->convertRGB2save(rgbBuffer_,QSize(color_width_,color_height_));}
    QImage getDepthToSave() const {return converter_->convertDepth2save((const unsigned short *)depthBuffer_,QSize(depth_width_,depth_height_));}

    void createPC(pc &point_cloud) const;

    Intrinsics getDepthIntrinsics(){
        return depthIntrinsics_;
    }


signals:
    /*if we sit in a separate slot, can be connected in a standard way, i.e. quied (no other way around)
      do not use in the same thread with queueing - it is emited from an infinite loop ->
      will never be executed and will cause memory overflow*/
    void framesReady();

private:
    QWaitCondition pauseCond_;

    Kinect2VideoSource device_;
    Intrinsics depthIntrinsics_;
    //converter is in the video thread, because otherwise it makes GUI slow
    Buffer2ImageConverter *converter_;

    unsigned char *depthBuffer_; //receive depth data
    size_t Ndb_;
    unsigned char *irBuffer_; //receive ir data
    size_t Nir_;
    unsigned char *rgbBuffer_; //receive bgr data
    size_t Nrgb_;

    int depth_width_,depth_height_;
    int color_width_,color_height_;

    volatile bool pause_;

    QMutex &lock_;
    QMutex runLock_;

    SyncQueue<QMutex,ToFFrame> *queue_;
    bool queueset_;

    const double MAX_DEPTH; //a constant to convert to greyscale
};

#endif // VIDEOSOURCE_H
