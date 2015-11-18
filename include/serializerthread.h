#ifndef SERIALIZERTHREAD
#define SERIALIZERTHREAD

#include <QThread>
#include <QSize>

#include "syncqueue.h"


typedef SyncQueue<QMutex,ToFFrame> ToFFrameQueue;

class QSerializerThread: public QThread
{
public:
    QSerializerThread(const QString &outputdir, int maxBufferSize,
                      QSize rgbsize, QSize dsize, QSize irsize);

    SyncQueue<QMutex,ToFFrame> *getQueue(){
        return &queue_;
    }

    void setMaxFramesToSave(int maxFrames){
        maxFrames_ = maxFrames;
    }

    void stopAfterQueueEmpty(){
        runLock_.lock();
        stop_ = true;
        runLock_.unlock();
    }

    void run();

private:
    ToFFrameQueue queue_;
    QString outputdir_;
    volatile bool stop_;
    long framecounter_;
    int maxFrames_;
    QSize rgbsize_,dsize_,irsize_;
    QMutex runLock_;

};

#endif
