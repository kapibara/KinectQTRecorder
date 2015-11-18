#include "plotwidget.h"

#include <iostream>

PlotWidget::PlotWidget(QWidget *parent) :
    QGLWidget(parent,0,Qt::Window)
{
    rotation_angle_x = 0.0f;
    rotation_angle_y = 0.0f;
    rotation_angle_z = 0.0f;
    translation_x = 0.0f;
    translation_y = 0.0f;
    translation_z = 0.0f;
    maxpointZ = 0;
    midpointZ = 0;
    minpointZ = 0;
    far_plane_ = 6000.0;
    near_plane_ = 0.1;
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
    glTranslatef(0,0,-500.f);

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
    gluPerspective(45.0, static_cast<float>(w) / static_cast<float>(h), near_plane_, far_plane_);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,0,0,0,1,0,1,0);

    setFocus(); //this is to set focus so that we can use keyboard key
}

void PlotWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event)
{
    float dx = (event->y() - lastPos.y()) / 10.0f;
    float dy = (event->x() - lastPos.x()) / 10.0f;

    if (event->buttons()) {
        std::cerr << "event->buttons()" << event->buttons() <<std::endl;
        if(event->buttons()&Qt::LeftButton){
            if(dx < 0 && abs(dx) > abs(dy))
                rotation_angle_x -= 4;
            else if(dx > 0 && abs(dx) > abs(dy))
                rotation_angle_x += 4;
            if(dy < 0 && abs(dy) > abs(dx))
                rotation_angle_y -= 4;
            else if(dy > 0 && abs(dy) > abs(dx))
                rotation_angle_y += 4;
        }
        else if(event->buttons()&Qt::RightButton){
            translation_x -= dy*20;
            translation_y += dx*20;
        }
    }



    lastPos = event->pos();

    updateGL();

}

void PlotWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {

    case Qt::Key_Up:
        translation_y += 30;
        break;

    case Qt::Key_Down:
        translation_y -= 30;
        break;

    case Qt::Key_Left:
        translation_x -= 30;
        break;

    case Qt::Key_Right:
        translation_x += 30;
        break;

    case Qt::Key_W:
        translation_z -= 30;
        break;

    case Qt::Key_S:
        translation_z += 30;
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
        translation_z += 100;
    else
        translation_z -= 100;

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
//    std::cerr << points_rgb_depth_list_.size() << std::endl;
    for(int i = 0; i < points_rgb_depth_list_.size(); i++)
    {
        glColor3f( points_rgb_depth_list_.at(i).second[0]/ 255.0,
                   points_rgb_depth_list_.at(i).second[1]/ 255.0,
                   points_rgb_depth_list_.at(i).second[2]/ 255.0);
        glVertex3f( points_rgb_depth_list_.at(i).first[0],
                    -points_rgb_depth_list_.at(i).first[1],
                    -points_rgb_depth_list_.at(i).first[2]);
    }

    glEnd();
}
