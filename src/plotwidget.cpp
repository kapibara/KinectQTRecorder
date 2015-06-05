#include "plotwidget.h"

PlotWidget::PlotWidget(QWidget *parent) :
    QGLWidget(parent)
{
    rotation_angle_x = 0.0f;
    rotation_angle_y = 0.0f;
    rotation_angle_z = 0.0f;
    translation_x = 0.0f;
    translation_y = 0.0f;
    translation_z = 0.0f;
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::initializeGL()
{
    glClearColor(0, 0, 0, 0);
    glClearDepth(1.0f);

    //Do depth test, so that only the points near the camera will be showed (the easiest method for Occlusion Culling)
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POINT_SMOOTH );
    glPointSize(1.0); // size of GL_Points

}

void PlotWidget::paintGL()
{
    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Move the object to spesific point so that it can be seen on the window
    glTranslatef(0,0,-1200.f);

    transformObject();
    drawObject();

    glMatrixMode( GL_PROJECTION );

    glPopMatrix();

    glFlush();
}

void PlotWidget::resizeGL(int w, int h)
{
    // Reshape callback function
    glViewport( 0, 0, static_cast<float>(w), static_cast<float>(h) );
    // Reload the projection matrix with an identity matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // gluPerspective — set up a perspective projection matrix
    gluPerspective(45.0, static_cast<float>(w) / static_cast<float>(h), near_plane, far_plane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,0,0,0,1,0,1,0);

    setFocus(); //this is to set focus so that we can use keyboard key
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = (event->y() - lastPos.y()) / 10.0f;
    float dy = (event->x() - lastPos.x()) / 10.0f;

    if (event->buttons() & Qt::LeftButton) {
        if(dx < 0 && abs(dx) > abs(dy))
            rotation_angle_x -= 1;
        else if(dx > 0 && abs(dx) > abs(dy))
            rotation_angle_x += 1;
        if(dy < 0 && abs(dy) > abs(dx))
            rotation_angle_y -= 1;
        else if(dy > 0 && abs(dy) > abs(dx))
            rotation_angle_y += 1;
    }

    lastPos = event->pos();

    updateGL();

}

void PlotWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {

    case Qt::Key_Up:
        translation_y += 10;
        break;

    case Qt::Key_Down:
        translation_y -= 10;
        break;

    case Qt::Key_Left:
        translation_x -= 10;
        break;

    case Qt::Key_Right:
        translation_x += 10;
        break;

    case Qt::Key_W:
        translation_z -= 10;
        break;

    case Qt::Key_S:
        translation_z += 10;
        break;

    case Qt::Key_8:
        rotation_angle_x += 1;
        break;

    case Qt::Key_5:
        rotation_angle_x -= 1;
        break;

    case Qt::Key_4:
        rotation_angle_y -= 1;
        break;

    case Qt::Key_6:
        rotation_angle_y += 1;
        break;

    case Qt::Key_A:
        rotation_angle_z -= 1;
        break;

    case Qt::Key_D:
        rotation_angle_z += 1;
        break;

    default:
        event->ignore();
        break;

    }

    updateGL();
}

void PlotWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0)
        translation_z += 5;
    else
        translation_z -= 5;

    updateGL();
}

void PlotWidget::hideEvent(QHideEvent *event)
{
    event->accept();
    rotation_angle_x = 0.0f;
    rotation_angle_y = 0.0f;
    rotation_angle_z = 0.0f;
    translation_x = 0.0f;
    translation_y = 0.0f;
    translation_z = 0.0f;
    emit closeplotwidget();
}

void PlotWidget::calculate3DPoints()
{  
    minpointZ = static_cast<float>(depth_mat.at<ushort>(0,0));
    maxpointZ = static_cast<float>(depth_mat.at<ushort>(0,0));

    for(int y = 0; y < rgb_pic.height(); y++)
        for(int x = 0; x < rgb_pic.width(); x++)
        {
            if(depth_mat.at<ushort>(y,x) != 0)
            {
                if(static_cast<float>(depth_mat.at<ushort>(y,x)) > maxpointZ)
                    maxpointZ = static_cast<float>(depth_mat.at<ushort>(y,x));
                else if(static_cast<float>(depth_mat.at<ushort>(y,x)) < minpointZ)
                    minpointZ = static_cast<float>(depth_mat.at<ushort>(y,x));
            }
        }

    midpointZ = (maxpointZ + minpointZ) / 2.0;

    for(int y = 0; y < rgb_pic.height(); y++)
        for(int x = 0; x < rgb_pic.width(); x++)
        {
            if(depth_mat.at<ushort>(y,x) != 0)
            {
                points_rgb_depth_list.push_back(qMakePair(qMakePair(static_cast<double>( ((static_cast<double>(x) - cx) * depth_mat.at<ushort>(y,x)) / fx),static_cast<double>(((static_cast<double>(y) - cy) * depth_mat.at<ushort>(y,x)) / fy)) , qMakePair(static_cast<double>(depth_mat.at<ushort>(y,x) - midpointZ), rgb_pic.pixel(x, y)) ) );
            }
        }

    updateGL();
}

void PlotWidget::transformObject()
{
    // The order of glRotate and glTranslate is important!
    // glScale -> glTranslate -> glRotate
    // Translation when user press keyboard
    glTranslatef(translation_x, translation_y, translation_z);
    // Rotate, when user press the mouse / keyboard.
    glRotatef( rotation_angle_x, 1.0, 0.0, 0.0 );
    glRotatef( rotation_angle_y, 0.0, 1.0, 0.0 );
    glRotatef( rotation_angle_z, 0.0, 0.0, 1.0 );
}

void PlotWidget::drawObject()
{
    glBegin(GL_POINTS);
    for(int i = 0; i < points_rgb_depth_list.size(); i++)
    {
        QColor point_color = points_rgb_depth_list.at(i).second.second;
        glColor3f( static_cast<float>(point_color.red()) / 255.0, static_cast<float>(point_color.green()) / 255.0, static_cast<float>(point_color.blue()) / 255.0 );
        glVertex3f( points_rgb_depth_list.at(i).first.first , -(points_rgb_depth_list.at(i).first.second), -points_rgb_depth_list.at(i).second.first );
    }

    glEnd();
}

void PlotWidget::getCalibrationSettings(CameraCalib camera_parameter)
{
    cx = camera_parameter.cx;
    cy = camera_parameter.cy;
    fx = camera_parameter.fx;
    fy = camera_parameter.fy;
    near_plane = camera_parameter.near_plane;
    far_plane = camera_parameter.far_plane;
}

void PlotWidget::receiveImageData(uchar *BufferRGB, ushort *BufferDepth, int image_width, int image_height)
{
    points_rgb_depth_list.clear();

    rgb_pic = QImage(BufferRGB,image_width,image_height,QImage::Format_RGB888);
    depth_mat = cv::Mat(image_height,image_width,CV_16U, BufferDepth, cv::Mat::AUTO_STEP);

    calculate3DPoints();
}
