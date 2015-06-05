#ifndef QMAINWINDOW_H
#define QMAINWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QImage>
#include <QAbstractButton>
#include <QShortcut>

#include "qvideowidget.h"
#include "kinectmotor.h"
#include "qvideosource.h"
#include "qsettingswindow.h"
#include "badimagedetector.h"
#include "screenshotdisplay.h"
#include "resourcestorage.h"
#include "plotwidget.h"

class QMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QMainWindow(ResourceStorage &s,QWidget *parent = 0);

signals:
    //send Calibration Settings and ImageData to PlotWidget
    void setCalibrationSettings(CameraCalib camera_parameter);
    void sendImageData(uchar* LastBufferRGB, ushort* LastBufferDepth, int image_width, int image_height);

public slots:
    void onRecord();
    void onSliderMove(int value);
    void onNewFrame();
    void onSettingsClick();
    //slot to show Plot Window and video source is paused
    void showPlot();
    //triggered when Plot Window is closed and video source resume
    void triggerplotwidgetclosed();

private:
    QVideoWidget *rgbVideo_;
    QVideoWidget *depthVideo_;

    QSlider *camPos_;
    QAbstractButton *record_;
    QAbstractButton *settings_;
    QAbstractButton *plotButton;
    QSettingsWindow *settingsWindow_;
    QShortcut *shortcut_;

    BadImageDetector *detector_;
    QScreenshotDisplay *screenshotDisplay_;

    QVideoSource *vsource_;

    KinectMotor *motor_;

    PlotWidget *newplotwidget;

    QImage lastRGB_;
    QImage lastDepth_;

    QMutex lock_;

    uchar *locBufferRGB_;
    uchar *locBufferGrey_;
    unsigned short *locBufferDepth_;

    QString currentPath_;
    QString basePath_;

    int currentCounter_;
    int totalCounter_;
    int framesCounter_;

    CameraCalib camera_calib_para;

    bool pause_;
};

#endif // QMAINWINDOW_H
