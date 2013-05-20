#include "qvideowidget.h"

QVideoWidget::QVideoWidget(QWidget *parent)
     : QWidget(parent)
     , surface_(0)
 {
     setAutoFillBackground(false);
     setAttribute(Qt::WA_NoSystemBackground, true);
     setAttribute(Qt::WA_PaintOnScreen, true);

     QPalette palette = this->palette();
     palette.setColor(QPalette::Background, Qt::black);
     setPalette(palette);

     setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

     surface_ = new QVideoSurface(this,parent);
 }

 QVideoWidget::~QVideoWidget()
 {
     delete surface_;
 }

 QSize QVideoWidget::sizeHint() const
 {
     return surface_->surfaceFormat().sizeHint();
 }

 void QVideoWidget::paintEvent(QPaintEvent *event)
 {
     QPainter painter(this);

     if (surface_->isActive()) {
         const QRect videoRect = surface_->videoRect();

         if (!videoRect.contains(event->rect())) {
             QRegion region = event->region();
             region-=videoRect;

             QBrush brush = palette().background();

             foreach (const QRect &rect, region.rects())
                 painter.fillRect(rect, brush);
         }

         surface_->paint(&painter);
     } else {
         painter.fillRect(event->rect(), palette().background());
     }
 }

 void QVideoWidget::resizeEvent(QResizeEvent *event)
 {
     QWidget::resizeEvent(event);

     surface_->updateVideoRect();
 }
