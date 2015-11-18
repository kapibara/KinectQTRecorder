#ifndef QMAINWINDOW_H
#define QMAINWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QImage>
#include <QAbstractButton>
#include <QShortcut>

#include "qvideowidget.h"
#include "qvideosource.h"
#include "qsettingswindow.h"
#include "badimagedetector.h"
#include "screenshotdisplay.h"
#include "resourcestorage.h"
#include "plotwidget.h"
#include "serializerthread.h"

class QMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QMainWindow(ResourceStorage &s,QWidget *parent = 0);

signals:
    //send Calibration Settings and ImageData to PlotWidget
    //void setCalibrationSettings(CameraCalib camera_parameter);
    void sendImageData(uchar* LastBufferRGB, ushort* LastBufferDepth, int image_width, int image_height);

public slots:
    void onRecord();
    //void onSliderMove(int value);
    void onRecordVideo();

    void onNewFrame();
    void onSettingsClick();
    //slot to show Plot Window and video source is paused
    void onShowPlot();
    //triggered when Plot Window is closed and video source resume
    void onClosePlot();

    void onSeriThreadFinished();

private:
    QVideoWidget *rgbVideo_;
    QVideoWidget *depthVideo_;

    QSlider *camPos_;
    QAbstractButton *record_;
    QAbstractButton *settings_;

    QAbstractButton *plotButton;
    QSettingsWindow *settingsWindow_;
    QShortcut *shortcut_;

    QSerializerThread *serithread_;
    QAbstractButton *recordVideo_;
    bool isrecording_;

    BadImageDetector *detector_;
    QScreenshotDisplay *screenshotDisplay_;

    QVideoSource *vsource_;

    //KinectMotor *motor_;

    PlotWidget *plot3Dwidget_;

    QImage lastRGB_;
    QImage lastDepth_;

    QMutex lock_;

    uchar *locBufferRGB_;
    uchar *locBufferGrey_;
    unsigned short *locBufferDepth_;
    unsigned short *locBufferIR_;

    QString currentPath_;
    QString basePath_;

    int totalCounter_;
    int framesCounter_;
    int videobufferSize_;

    Intrinsics intrinsics_;
    float nearPlane_;
    float farPlane_;
};

#endif // QMAINWINDOW_H
