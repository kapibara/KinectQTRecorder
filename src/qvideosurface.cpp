#include "qvideosurface.h"

QVideoSurface::QVideoSurface(QWidget *widget, QObject *parent) : QAbstractVideoSurface(parent)
    , widget_(widget)
    , imageFormat_(QImage::Format_Invalid)
{
    markup_ = 0;
}

QList<QVideoFrame::PixelFormat> QVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool QVideoSurface::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const
{
    Q_UNUSED(similar);

    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}

bool QVideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        imageFormat_ = imageFormat;
        imageSize_ = size;
        sourceRect_ = format.viewport();

        QAbstractVideoSurface::start(format);

        widget_->updateGeometry();
        updateVideoRect();

        return true;
    } else {
        return false;
    }
}

void QVideoSurface::stop()
{
    currentFrame_ = QVideoFrame();
    targetRect_ = QRect();

    QAbstractVideoSurface::stop();

    widget_->update();
}

void QVideoSurface::setMarkup(const QImage *markup)
{
    if (markup_ != 0){
        delete markup_;
        markup_ = 0;
    }

    if (markup != 0){
        markup_ = new QImage(*markup);
    }
}

bool QVideoSurface::present(const QVideoFrame &frame)
{
    if (surfaceFormat().pixelFormat() != frame.pixelFormat()
            || surfaceFormat().frameSize() != frame.size()) {
        setError(IncorrectFormatError);
        stop();

        return false;
    } else {
        currentFrame_ = frame;

        widget_->repaint(targetRect_);

        return true;
    }
}

void QVideoSurface::updateVideoRect()
{
    QSize size = surfaceFormat().sizeHint();
    size.scale(widget_->size().boundedTo(size), Qt::KeepAspectRatio);

    targetRect_ = QRect(QPoint(0, 0), size);
    targetRect_.moveCenter(widget_->rect().center());
}

void QVideoSurface::paint(QPainter *painter)
{
    if (currentFrame_.map(QAbstractVideoBuffer::ReadOnly)) {
        const QTransform oldTransform = painter->transform();

        if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop) {
           painter->scale(1, -1);
           painter->translate(0, -widget_->height());
        }

        QImage image(currentFrame_.bits(),
                currentFrame_.width(),
                currentFrame_.height(),
                currentFrame_.bytesPerLine(),
                imageFormat_);

        if (markup_ !=0){
            QPainter p(&image);
            p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.drawImage(0, 0, *markup_);
        }

        painter->drawImage(targetRect_, image, sourceRect_);

        painter->setTransform(oldTransform);

        currentFrame_.unmap();
    }
}
