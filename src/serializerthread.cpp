#include "serializerthread.h"
#include "imagerecorder.h"

#include <QString>

#include <iostream>

QSerializerThread::QSerializerThread(const QString &outputdir,int maxBufferSize,
                                     QSize rgbsize,QSize dsize, QSize irsize):
    outputdir_(outputdir),queue_(maxBufferSize),
    rgbsize_(rgbsize),dsize_(dsize),irsize_(irsize)
{
    stop_ = false;
    framecounter_ = 0;
    maxFrames_ = -1;
}

void QSerializerThread::run(){

     while(1){
         /*here we have busy wait, since if the queue is empty, we proceed;
          * however, during the recording this is unlikely
          * and the thread is started only in case there is something to record;
          * furthermore, we assume there is a single serialization thread running here
          * otherwise either the front and pop is blocking, or empty+front_and_pop should
          * be locked. */

         /*
          * Buffer(depthBuffer_,Ndb_),
                                 Buffer(rgbBuffer_,Nrgb_),
                                 Buffer(irBuffer_,Nir_)))
                                 */
         if(!queue_.empty()){
            std::cerr <<"process frame: " << queue_.size() << std::endl;
            ToFFrame frame = queue_.front_and_pop();

            QString fnamenum = QString("%1").arg(framecounter_, 20, 10, QChar('0'));

            QString pathDO = outputdir_ +  "/ImDepthOrig" + fnamenum + ".bin";
            QString pathRGB = outputdir_ + "/ImRGB" + fnamenum + ".png";
            QString pathIR = outputdir_ + "/ImIR" + fnamenum + ".bin";


            ImageRecorder::saveDepth(pathDO.toLatin1().data(),(unsigned short *)std::get<0>(frame).getData(),
                                     dsize_.width(),dsize_.height());
            ImageRecorder::saveRGB(pathRGB.toLatin1().data(),std::get<1>(frame).getData(),
                                   rgbsize_.width(),rgbsize_.height());
            ImageRecorder::saveIr(pathIR.toLatin1().data(),(unsigned short *)std::get<2>(frame).getData(),
                                   irsize_.width(),irsize_.height());

            framecounter_++;

            if(maxFrames_>0 && framecounter_>=maxFrames_){
                std::cerr <<"exiting on maxFrames_ exceeded" <<std::endl;
                return;
            }


         }else{
             std::cerr <<"queue is empty" <<std::endl;
             runLock_.lock();
             if(stop_){
                 runLock_.unlock();
                 return;
             }
             runLock_.unlock();

             msleep(10);
         }

     }
}
