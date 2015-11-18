#ifndef QVIDEOSURFACE_H
#define QVIDEOSURFACE_H

#include <QImage>
#include <QPainter>
#include <QList>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QWidget>
#include <QAbstractVideoSurface>

#include "kinect2videosource.h"

class QVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QVideoSurface(QWidget *widget, QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> virtual supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool isFormatSupported(const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const;

    bool virtual start(const QVideoSurfaceFormat &format);
    void virtual stop();

    bool virtual present(const QVideoFrame &frame);

    QRect videoRect() const { return targetRect_; }
    void updateVideoRect();

    void paint(QPainter *painter);

    void setMarkup(const QImage *markup);

private:
    QWidget *widget_;
    QImage::Format imageFormat_;
    QVideoFrame currentFrame_;
    QRect targetRect_;
    QSize imageSize_;
    QRect sourceRect_;

    QImage *markup_;

};

#endif // QVIDEOSURFACE_H
