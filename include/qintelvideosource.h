#ifndef QINTELVIDEOSOURCE_H
#define QINTELVIDEOSOURCE_H

#include "pxcsession.h"
#include "pxcsmartptr.h"
#include "pxccapture.h"
#include "pxcscheduler.h"

#include "intelvideosource.h"

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QIntelVideoSource: public QThread
{
    Q_OBJECT
public:
    QIntelVideoSource(QMutex &lock, bool &pause);

    uchar* getRGBBuffer() {return rgbdataptr_;}
    uchar* getScaledDepthBuffer() {return depthdataptr_;}
    unsigned short* getOriginalDepthBuffer() {return depthoriginal_;}

    int frameHeight(IntelVideoSource::ImageType type) {return device_.height(type);}
    int frameWidth(IntelVideoSource::ImageType type) {return device_.width(type);}

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

    IntelVideoSource device_;

    uchar *depthoriginal_; //original depth data
    uchar *depthdataptr_; //ready-to-show-depth
    uchar *rgbdataptr_; //ready-to-show-rgb

    bool pause_;

    QMutex &lock_;
    QMutex runLock_;

    const double MAX_DEPTH; //a constant to convert to greyscale

};

#endif // QINTELVIDEOSOURCE_H
