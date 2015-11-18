#include "qmainwindow.h"
#include "imagerecorder.h"
#include "plotwidget.h"
#include "serializerthread.h"

#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>

#include <iostream>

using namespace std;

QMainWindow::QMainWindow(ResourceStorage &s,QWidget *parent): QWidget(parent)
{
    /*default values*/
    framesCounter_ = 5;
    videobufferSize_ = 100;
    basePath_ = QDir::currentPath();

    totalCounter_ = 0;

    rgbVideo_ = new QVideoWidget(this);
    rgbVideo_->setMaximumSize(QSize(640,480));

    depthVideo_ = new QVideoWidget(this);
    depthVideo_->setMaximumSize(QSize(640,480));

    record_ = new QPushButton();
    record_->setText(tr("Record"));

    recordVideo_ = new QPushButton();
    recordVideo_->setText(tr("Record Video"));

    settings_ = new QPushButton();
    settings_->setText(tr("Settings"));

    plotButton = new QPushButton();
    plotButton->setText(tr("Plot"));

    plot3Dwidget_ = new PlotWidget(this);
    plot3Dwidget_->setModal(true);

    screenshotDisplay_ = new QScreenshotDisplay(s.getResourceLocation(),this);
    screenshotDisplay_->setModal(true);

    shortcut_ = new QShortcut(this);
    shortcut_->setKey(Qt::Key_Tab);

    detector_ = new BadImageDetector();

    vsource_ = new QVideoSource(lock_);

    intrinsics_ = vsource_->getDepthIntrinsics();
    nearPlane_ = 0.1f;
    farPlane_ = 6000.0f;

    settingsWindow_ = new QSettingsWindow(this,
                                          framesCounter_,
                                          videobufferSize_,
                                          basePath_,
                                          intrinsics_.cx_,
                                          intrinsics_.cy_,
                                          intrinsics_.fx_,
                                          intrinsics_.fy_,
                                          nearPlane_,farPlane_);
    settingsWindow_->setModal(true);

    int channelSizeRGB = vsource_->getFrameWidth(Kinect2VideoSource::ImageType::IMAGE_RGB)*vsource_->getFrameHeight(Kinect2VideoSource::ImageType::IMAGE_RGB);
    int channelSizeDepth = vsource_->getFrameWidth(Kinect2VideoSource::ImageType::IMAGE_DEPTH)*vsource_->getFrameHeight(Kinect2VideoSource::ImageType::IMAGE_DEPTH);
    int channelSizeIr = vsource_->getFrameWidth(Kinect2VideoSource::ImageType::IMAGE_IR)*vsource_->getFrameHeight(Kinect2VideoSource::ImageType::IMAGE_IR);

    locBufferRGB_ = new uchar[vsource_->getRGBPixelSize()*channelSizeRGB];
    locBufferDepth_ = new unsigned short[channelSizeDepth];
    locBufferIR_ =  new unsigned short[channelSizeIr];

    QBoxLayout *vlayout = new QBoxLayout(QBoxLayout::LeftToRight); //videos

    vlayout->addWidget(rgbVideo_);
    vlayout->addWidget(depthVideo_);

    QBoxLayout *clayout = new QBoxLayout(QBoxLayout::LeftToRight); //control

    //clayout->addWidget(camPos_);
    clayout->addWidget(record_);
    clayout->addWidget(recordVideo_);
    clayout->addWidget(plotButton);
    clayout->addWidget(settings_);

    QBoxLayout *mlayout = new QBoxLayout(QBoxLayout::TopToBottom); //main

    mlayout->addLayout(vlayout);
    mlayout->addLayout(clayout);

    setLayout(mlayout);
    setWindowTitle(basePath_);
    resize(QSize(1280,600));

    serithread_ = nullptr;
    isrecording_ = false;

    connect(record_,SIGNAL(clicked()),this,SLOT(onRecord()));
    connect(recordVideo_,SIGNAL(clicked()),this,SLOT(onRecordVideo()));
    connect(vsource_,SIGNAL(framesReady()),this,SLOT(onNewFrame()));
    connect(settings_,SIGNAL(clicked()),this,SLOT(onSettingsClick()));
    connect(shortcut_,SIGNAL(activated()),record_,SIGNAL(clicked()));
    connect(plotButton,SIGNAL(clicked()),this,SLOT(onShowPlot()));
    connect(plot3Dwidget_,SIGNAL(closeplotwidget()),this,SLOT(onClosePlot()));

    vsource_->start();

}

void QMainWindow::onRecord(){

    if(!isrecording_){
        isrecording_ = true;
        record_->setEnabled(false);
        recordVideo_->setEnabled(false);

        BadImageDetector::State result = detector_->checkImage(locBufferDepth_);

        if(result != BadImageDetector::GOOD){
            vsource_->pause();
            currentPath_ = basePath_ + "/rBAD" + QString::number(totalCounter_) + "/";
            screenshotDisplay_->setLocation(currentPath_);
            screenshotDisplay_->setImageSource(&lastDepth_,result);
            screenshotDisplay_->exec();
            vsource_->resume();
       }else{
            currentPath_ = basePath_ + "/r" + QString::number(totalCounter_) + "/";
        }

        totalCounter_++;
        QDir().mkdir(currentPath_);

        serithread_ = new QSerializerThread(currentPath_,framesCounter_,
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_RGB),
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_DEPTH),
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_IR));

        serithread_->setMaxFramesToSave(framesCounter_);

        connect(serithread_,SIGNAL(finished()),this,SLOT(onSeriThreadFinished()));


        vsource_->setQueue(serithread_->getQueue());
        serithread_->start();
    }

}

void QMainWindow::onSeriThreadFinished(){
    /*in case the queue is not unset (i.e. the thread is called from record and not from record video*/
    vsource_->unsetQueue();

    if(serithread_!= nullptr){
        delete serithread_;
        serithread_ = nullptr;
    }
    isrecording_ = false;
    recordVideo_->setEnabled(true);
    recordVideo_->setText(tr("Record Video"));
    record_->setEnabled(true);
}

void QMainWindow::onRecordVideo(){
    if(!isrecording_){
        isrecording_ = true;
        record_->setEnabled(false);
        recordVideo_->setText(tr("Stop"));
        currentPath_ = basePath_ + "/r" + QString::number(totalCounter_) + "/";
        totalCounter_++;


        QDir().mkdir(currentPath_);

        serithread_ = new QSerializerThread(currentPath_,videobufferSize_,
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_RGB),
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_DEPTH),
                                        vsource_->getFrameSize(Kinect2VideoSource::ImageType::IMAGE_IR));
        connect(serithread_,SIGNAL(finished()),this,SLOT(onSeriThreadFinished()));


        vsource_->setQueue(serithread_->getQueue());
        serithread_->start();
    }else{
        vsource_->unsetQueue();
        recordVideo_->setEnabled(false);
        recordVideo_->setText(tr("Dumping bufferized frames"));
        serithread_->stopAfterQueueEmpty();
    }
}

void QMainWindow::onSettingsClick(){
    vsource_->pause();

    int result = settingsWindow_->exec();

    if(result == QDialog::Accepted){

        framesCounter_ = settingsWindow_->getFrameCount();
        videobufferSize_ = settingsWindow_->getVBCount();

        if (settingsWindow_->isDirChanged()){
            basePath_ = settingsWindow_->getPath();
            totalCounter_ = 0;
            setWindowTitle(basePath_);
        }


        QString path = settingsWindow_->getMarkupPath();
        if(!path.isNull()){
            QImage markup(path);

            rgbVideo_->setMarkup(&markup);
        }else{
            rgbVideo_->setMarkup(0);
        }

        intrinsics_.cx_ = settingsWindow_->getPrinciplePoint().first;
        intrinsics_.cy_ = settingsWindow_->getPrinciplePoint().second;
        intrinsics_.fx_ = settingsWindow_->getFocalLength().first;
        intrinsics_.fy_ = settingsWindow_->getFocalLength().second;
        nearPlane_ = settingsWindow_->getNearFarPlane().first;
        farPlane_ = settingsWindow_->getNearFarPlane().second;
    }

    vsource_->resume();
}

void QMainWindow::onShowPlot()
{
    vsource_->pause();

    lock_.lock();
    plot3Dwidget_->getPCReference().clear();
    vsource_->createPC(plot3Dwidget_->getPCReference());
    lock_.unlock();

    std::cerr <<"list length: " << plot3Dwidget_->getPCReference().size() << std::endl;

    plot3Dwidget_->resize(640,480);
    plot3Dwidget_->show();
}

void QMainWindow::onClosePlot()
{
     vsource_->resume();
}

void QMainWindow::onNewFrame()
{

    lock_.lock();

        lastRGB_ = vsource_->getLastRGB();
        lastDepth_ = vsource_->getLastDepth();

        size_t pixels = vsource_->getFrameWidth(Kinect2VideoSource::ImageType::IMAGE_DEPTH)*vsource_->getFrameHeight(Kinect2VideoSource::ImageType::IMAGE_DEPTH);

        memcpy(locBufferDepth_,vsource_->getOriginalDepthBuffer(),pixels);

    lock_.unlock();

    QVideoFrame rgbFrame(lastRGB_);
    QVideoFrame depthFrame(lastDepth_);

    if (rgbVideo_->videoSurface()->surfaceFormat().pixelFormat()!=rgbFrame.pixelFormat()){
        QVideoSurfaceFormat rgbformat(rgbFrame.size(), rgbFrame.pixelFormat());
        rgbVideo_->videoSurface()->start(rgbformat);

        QVideoSurfaceFormat depthformat(depthFrame.size(), depthFrame.pixelFormat());
        depthVideo_->videoSurface()->start(depthformat);
    }

    rgbVideo_->videoSurface()->present(rgbFrame);
    depthVideo_->videoSurface()->present(depthFrame);

}
