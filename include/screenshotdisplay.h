#ifndef SCREENSHOTDISPLAY_H
#define SCREENSHOTDISPLAY_H

#include <QDialog>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QPainter>
#include <QSound>

#include "badimagedetector.h"

class QScreenshotDisplay: public QDialog
{
    Q_OBJECT
public:
    QScreenshotDisplay(const QString &resLocation, QWidget *parent);

    void setLocation(const QString &location){
        location_ = location;
        this->setWindowTitle("Saved to: " + location);
    }

    void setImageSource(QImage *source, BadImageDetector::State state){

        image_ = source;


        QPainter painter(image_);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        switch (state)
        {
        case BadImageDetector::State::GOOD:
            painter.drawImage(0,0, *okImage_);
            hint_->setText("");
            break;
        case BadImageDetector::State::TOO_NEAR:
            error_->play();
            painter.drawImage(0,0,*badImage_);
            hint_->setText("The subject is too near; move several steps away from the camera");
            break;
        case BadImageDetector::State::TOO_FAR:
            error_->play();
            painter.drawImage(0,0,*badImage_);
            hint_->setText("The subject is too far; move several steps closer to the camera");
            break;
        };

        painter.end();

        display_->setPixmap(QPixmap::fromImage(*image_));
    }

private:

    QLabel *display_;
    QLabel *hint_;
    QString location_;
    QImage *image_;
    QPushButton *ok_;
    QImage *okImage_;
    QImage *badImage_;
    QSound *error_;

};

#endif // SCREENSHOTDISPLAY_H
