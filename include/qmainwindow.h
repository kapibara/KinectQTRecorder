#ifndef QMAINWINDOW_H
#define QMAINWINDOW_H

#include <QWidget>
#include <QSlider>
#include <QImage>
#include <QAbstractButton>

#include "qvideowidget.h"
#include "kinectmotor.h"
#include "qvideosource.h"
#include "qsettingswindow.h"

class QMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit QMainWindow(QWidget *parent = 0);

signals:

public slots:
    void onRecord();
    void onSliderMove(int value);
    void onNewFrame();
    void onSettingsClick();

private:
    QVideoWidget *rgbVideo_;
    QVideoWidget *depthVideo_;

    QSlider *camPos_;
    QAbstractButton *record_;
    QAbstractButton *settings_;
    QSettingsWindow *settingsWindow_;

    QVideoSource *vsource_;

    KinectMotor *motor_;

    QImage lastRGB_;
    QImage lastDepth_;

    QMutex lock_;

    uchar *locBufferRGB_;
    uchar *locBufferDepth_;

    QString currentPath_;
    QString basePath_;

    int currentCounter_;
    int totalCounter_;
    int framesCounter_;

    bool pause_;
};

#endif // QMAINWINDOW_H
