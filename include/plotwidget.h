//A class that contatin Camera Calibration Parameter
#ifndef CAMERACALIB_H
#define CAMERACALIB_H

#include <vector>
#include <array>


#endif // CAMERACALIBH

//A class to plot the 3D Points gathered from Kinect
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QKeyEvent>
#include <QVector3D>
#include <QDebug>

#include <GL/glu.h>

#include <time.h>
#include <math.h>
#include <tuple>

#include "bufferwrapper.h"

class PlotWidget : public QGLWidget
{
    Q_OBJECT
public:
    PlotWidget(QWidget *parent = 0);

    void setModal(bool modal){
        if(modal){
            setWindowModality(Qt::WindowModal);
        }else{
            setWindowModality(Qt::NonModal);
        }
    }

    void setNearFarPlanes(float np, float fp){
        near_plane_ = np;
        far_plane_ = fp;
    }

    /*this function is used to populate point cloud*/
    pc &getPCReference()
    {
        return points_rgb_depth_list_;
    }

protected:
    void initializeGL(); //sets up resources needed by the OpenGL implementation to render the scene. called once if QGLWidget was created
    void paintGL(); //performs painting operations using OpenGL calls. called if updateGL() was called
    void resizeGL(int w, int h); // resizes the viewport so that the rendered scene fits onto the widget, and sets up a projection matrix to map 3D coordinates to 2D viewport coordinates. called at beginning and when the window resized
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void hideEvent(QHideEvent *event); // Reset all the object transformation and hide the PlotWidget
   // void calculate3DPoints(); //Calculate MidDepth and 3D Points

    //3D Points container (point : color)
    pc points_rgb_depth_list_;

    //Container for the Buffers that are received from qmainwindow
    BufferWrapper<ushort,1> depth_;
    BufferWrapper<uchar,3>  rgb_;

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
    float far_plane_, near_plane_;
    //float cx, cy; //principle point
    //float fx, fy; //focal length

    //Used to determine the MidPoint of the Z-Axis
    float minpointZ;
    float maxpointZ;
    float midpointZ;

    //Object Transformation and Draw Function
    void transformObject();
    void drawObject();

//public slots:
    //slot used to get the Settings and Image Data from qmainwindow
    //void setCalibrationSettings(CameraCalib camera_parameter);
    //void setImageData(uchar* BufferRGB, ushort *BufferDepth, int image_width, int image_height);

signals:
    //send a signal that the plot window is closed to qmainwindow
    void closeplotwidget();

};

#endif // GLWIDGET_H
