//A class that contatin Camera Calibration Parameter
#ifndef CAMERACALIB_H
#define CAMERACALIB_H

class CameraCalib
{

public:
    //Calibration
    float far_plane;
    float near_plane;
    float cx;
    float cy;
    float fx;
    float fy;
};

#endif // CAMERACALIBH

//A class to plot the 3D Points gathered from Kinect
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QKeyEvent>
#include <QDebug>
#include <GL/glu.h>
#include <time.h>
#include <QVector3D>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class PlotWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

protected:
    void initializeGL(); //sets up resources needed by the OpenGL implementation to render the scene. called once if QGLWidget was created
    void paintGL(); //performs painting operations using OpenGL calls. called if updateGL() was called
    void resizeGL(int w, int h); // resizes the viewport so that the rendered scene fits onto the widget, and sets up a projection matrix to map 3D coordinates to 2D viewport coordinates. called at beginning and when the window resized
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void hideEvent(QHideEvent *event); // Reset all the object transformation and hide the PlotWidget
    void calculate3DPoints(); //Calculate MidDepth and 3D Points

    //3D Points container
    QList<QPair<QPair<float,float>,QPair<float, QRgb> > > points_rgb_depth_list;

    //Container for the Buffers that are received from qmainwindow
    cv::Mat depth_mat;
    QImage rgb_pic;

private:
    //last Pos is used for Rotating the object with mouse
    QPoint lastPos;

    //Parameter for Object Transformation
    float rotation_angle_x;
    float rotation_angle_y;
    float rotation_angle_z;
    float translation_x;
    float translation_y;
    float translation_z;

    //Camera Calibration Parameter
    float far_plane, near_plane;
    float cx, cy; //principle point
    float fx, fy; //focal length

    //Used to determine the MidPoint of the Z-Axis
    float minpointZ;
    float maxpointZ;
    float midpointZ;

    //Object Transformation and Draw Function
    void transformObject();
    void drawObject();

public slots:
    //slot used to get the Settings and Image Data from qmainwindow
    void getCalibrationSettings(CameraCalib camera_parameter);
    void receiveImageData(uchar* BufferRGB, ushort *BufferDepth, int image_width, int image_height);

signals:
    //send a signal that the plot window is closed to qmainwindow
    void closeplotwidget();

};

#endif // GLWIDGET_H
