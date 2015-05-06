#include "screenshotdisplay.h"

#include <QBoxLayout>

#include <iostream>

QScreenshotDisplay::QScreenshotDisplay(const QString &resLocation, QWidget *parent):QDialog(parent)
{
    display_ = new QLabel("");
    hint_ = new QLabel("");
    ok_ = new QPushButton("Ok");

    error_ = new QSound(resLocation + "boing.wav");

    okImage_ = new QImage();
    okImage_->load(resLocation + "Ok.png");
    badImage_ = new QImage();
    badImage_->load(resLocation + "Bad.png");

    QBoxLayout *main = new QBoxLayout(QBoxLayout::TopToBottom);
    QBoxLayout *bottom = new QBoxLayout(QBoxLayout::LeftToRight);

    bottom->addWidget(hint_);
    bottom->addWidget(ok_,0,Qt::AlignRight);

    main->addWidget(display_);
    main->addLayout(bottom);

    setLayout(main);
    setModal(true);

    connect(ok_,SIGNAL(clicked()),this,SLOT(accept()));

}
