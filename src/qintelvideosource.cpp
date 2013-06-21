#include "qintelvideosource.h"

QIntelVideoSource::QIntelVideoSource(QMutex &lock, bool &pause):MAX_DEPTH(1024),lock_(lock), pause_(pause)
{
    device_ = new IntelVideoSource();

    depthdataptr_ = new uchar[device_->width(IntelVideoSource::IMAGE_DEPTH)*device_->height(IntelVideoSource::IMAGE_DEPTH)*3];
    rgbdataptr_ = new uchar[device_->expectedBufferSize(IntelVideoSource::IMAGE_RGB)];
    depthoriginal_ = new uchar[device_->expectedBufferSize(IntelVideoSource::IMAGE_DEPTH)];
    rgboriginal_ =  new uchar[device_->expectedBufferSize(IntelVideoSource::IMAGE_RGB)];

    depthshortptr_ =  new unsigned short[device_->width(IntelVideoSource::IMAGE_DEPTH)*device_->height(IntelVideoSource::IMAGE_DEPTH)];
}

void QIntelVideoSource::pause(){
    runLock_.lock();

    pause_ = true;

    runLock_.unlock();

}

void QIntelVideoSource::resume(){

    runLock_.lock();

    pause_ = false;

    runLock_.unlock();

    pauseCond_.wakeAll();

}

void QIntelVideoSource::run()
{
    /*1. the declaration could be as well put inside the loop -> the constructor will be called each time,
         it is ok -> the buffer is used, not allocated, but it is still overhead
      2. Use direct buffer; Mat does not seem to be working too good with QImage*/

        while(1){

            runLock_.lock();

            if(pause_){

                pauseCond_.wait(&runLock_); //locks even if lock is not locked...

            }

            runLock_.unlock();

            device_->retrieve(depthoriginal_,IntelVideoSource::IMAGE_DEPTH);
            device_->retrieve(rgboriginal_,IntelVideoSource::IMAGE_RGB);

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

void QIntelVideoSource::convertColor()
{
    /*Here convert depth values*/

    int bufferSize = device_->expectedBufferSize(IntelVideoSource::IMAGE_DEPTH)/device_->sizeInBytes(IntelVideoSource::IMAGE_DEPTH);
    int inBytes = device_->sizeInBytes(IntelVideoSource::IMAGE_DEPTH);
    unsigned short value;

    /*copy original data*/
    //memcpy(depthshortptr_,depthdataptr_,device_->expectedBufferSize(IntelVideoSource::IMAGE_DEPTH));

    for(int i=0; i<bufferSize; i++){
        value = *((unsigned short *)(depthoriginal_+i*inBytes));

        if (value > MAX_DEPTH){
            value = 0;
        }

        depthshortptr_[i] = value;

        depthdataptr_[3*i] = ((float)value)/MAX_DEPTH*256;
        depthdataptr_[3*i+1] = depthdataptr_[3*i];
        depthdataptr_[3*i+2] = depthdataptr_[3*i];
    }

    bufferSize = device_->expectedBufferSize(IntelVideoSource::IMAGE_RGB)/device_->sizeInBytes(IntelVideoSource::IMAGE_RGB);

    for(int i=0; i<bufferSize; i++){
        rgbdataptr_[3*i] = rgboriginal_[3*i+2];
        rgbdataptr_[3*i+1] = rgboriginal_[3*i+1];
        rgbdataptr_[3*i+2] = rgboriginal_[3*i];
    }
}
