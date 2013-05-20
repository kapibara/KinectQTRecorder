#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>
#include "qvideosurface.h"

class QVideoWidget : public QWidget
{
    Q_OBJECT
public:
    QVideoWidget(QWidget *parent = 0);
    ~QVideoWidget();

    QAbstractVideoSurface *videoSurface() const { return surface_; }
    QSize sizeHint() const;

    void setMarkup(const QImage *markup) {surface_->setMarkup(markup);}

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QVideoSurface *surface_;
};

#endif // QVIDEOWIDGET_H
