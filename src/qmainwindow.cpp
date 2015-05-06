#include "qmainwindow.h"
#include "imagerecorder.h"

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
    basePath_ = QDir::currentPath();

    totalCounter_ = 0;
    currentCounter_ = 0;

    rgbVideo_ = new QVideoWidget(this);
    depthVideo_ = new QVideoWidget(this);

    record_ = new QPushButton();
    record_->setText(tr("Record"));

    settings_ = new QPushButton();
    settings_->setText(tr("Settings"));

    motor_ = new KinectMotor();

    settingsWindow_ = new QSettingsWindow(this,framesCounter_,basePath_);
    settingsWindow_->setModal(true);

    screenshotDisplay_ = new QScreenshotDisplay(s.getResourceLocation(),this);
    screenshotDisplay_->setModal(true);

    camPos_ = new QSlider(Qt::Horizontal,this);
    camPos_->setRange(-31,31);
    camPos_->setSingleStep(1);

    shortcut_ = new QShortcut(this);
    shortcut_->setKey(Qt::Key_Tab);

    camPos_->setValue(0);

    detector_ = new BadImageDetector();

/*
    while(!motor_->open())
    {
        if(motor_->isNoDeviceError()){
            QMessageBox::information(0,"No device connected","No device found.\n The application will be closed.");
            exit(-1);
        }
        motor_->move(0);
//        sleep(1);
    }*/

    pause_ = false;
    vsource_ = new QVideoSource(lock_,pause_);

    locBufferRGB_ = new uchar[vsource_->getFrameWidth()*vsource_->getFrameHeight()*3];
    locBufferGrey_ = new uchar[vsource_->getFrameWidth()*vsource_->getFrameHeight()*3];
    locBufferDepth_ = new unsigned short[vsource_->getFrameWidth()*vsource_->getFrameHeight()];

    QBoxLayout *vlayout = new QBoxLayout(QBoxLayout::LeftToRight); //videos

    vlayout->addWidget(rgbVideo_);
    vlayout->addWidget(depthVideo_);

    QBoxLayout *clayout = new QBoxLayout(QBoxLayout::LeftToRight); //control

    clayout->addWidget(camPos_);
    clayout->addWidget(record_);
    clayout->addWidget(settings_);

    QBoxLayout *mlayout = new QBoxLayout(QBoxLayout::TopToBottom); //main

    mlayout->addLayout(vlayout);
    mlayout->addLayout(clayout);

    setLayout(mlayout);
    setWindowTitle(basePath_);
    resize(QSize(1280,600));

    connect(record_,SIGNAL(clicked()),this,SLOT(onRecord()));
    connect(camPos_,SIGNAL(valueChanged(int)),this,SLOT(onSliderMove(int)));
    connect(vsource_,SIGNAL(framesReady()),this,SLOT(onNewFrame()));
    connect(settings_,SIGNAL(clicked()),this,SLOT(onSettingsClick()));
    connect(shortcut_,SIGNAL(activated()),record_,SIGNAL(clicked()));

    vsource_->start();
}

void QMainWindow::onRecord(){
    if(currentCounter_ == 0){
        totalCounter_++;
        currentCounter_ = framesCounter_;
        currentPath_ = basePath_ + "/r" + QString::number(totalCounter_) + "/";
        QDir().mkdir(currentPath_);
    }
}

void QMainWindow::onSettingsClick(){
    vsource_->pause();

    int result = settingsWindow_->exec();

    if(result == QDialog::Accepted){

        framesCounter_ = settingsWindow_->getFrameCount();

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
    }

    vsource_->resume();
}

void QMainWindow::onSliderMove(int newValue){
    if (motor_->isOpened())
    {
        motor_->move(newValue);
    }
    else{
        if (motor_->open()){
            motor_->move(newValue);
        }else{
            cerr << "Unable to connect to the motor" << endl;

            camPos_->setValue(0);
        }
    }
}

void QMainWindow::onNewFrame()
{

    lock_.lock();

       memcpy(locBufferRGB_,vsource_->getRGBBuffer(),vsource_->getFrameWidth()*vsource_->getFrameHeight()*3);
       memcpy(locBufferDepth_,vsource_->getOriginalDepthBuffer(),vsource_->getFrameWidth()*vsource_->getFrameHeight()*sizeof(short));
       memcpy(locBufferGrey_,vsource_->getScaledDepthBuffer(),vsource_->getFrameWidth()*vsource_->getFrameHeight()*3);

    lock_.unlock();

       lastRGB_ = QImage(locBufferRGB_,vsource_->getFrameWidth(),vsource_->getFrameHeight(),QImage::Format_RGB888);
       lastDepth_ = QImage(locBufferGrey_,vsource_->getFrameWidth(),vsource_->getFrameHeight(),QImage::Format_RGB888);


    if (currentCounter_ > 0){

        BadImageDetector::State result = detector_->checkImage(locBufferDepth_);

        if(currentCounter_ == framesCounter_){

            vsource_->pause();

            screenshotDisplay_->setLocation(currentPath_);
            screenshotDisplay_->setImageSource(&lastDepth_,result);

            screenshotDisplay_->exec();

            vsource_->resume();
        }

        QString suffix = "";

        switch(result){
        case BadImageDetector::GOOD:
            suffix ="";
            break;
        case BadImageDetector::TOO_FAR:
            suffix ="TF";
            break;
        case BadImageDetector::TOO_NEAR:
            suffix = "TN";
            break;
        }

        lastDepth_.save(currentPath_ + "/imDepth" + suffix + QString::number(currentCounter_) + ".png");

        QString path = currentPath_ +  "/ImDepthOrig" + suffix + QString::number(currentCounter_) + ".png";
        ImageRecorder::saveDepth(path.toLatin1().data(),locBufferDepth_,vsource_->getFrameWidth(),vsource_->getFrameHeight());

        path = currentPath_ + "/ImRGB" + suffix + QString::number(currentCounter_) + ".png";

        ImageRecorder::saveRGB(path.toLatin1().data(),locBufferRGB_,vsource_->getFrameWidth(),vsource_->getFrameHeight());

        currentCounter_--;
    }

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
