#include "qsettingswindow.h"

#include <QFileDialog>
#include <QBoxLayout>

QSettingsWindow::QSettingsWindow(QWidget *parent, int defFrameCount, QString defPath): QDialog(parent)
{
    path_ = defPath;
    frameCount_ = defFrameCount;
    markupPath_ = "";
    isDirChanged_ = false;

    currentPath_ =  new QLabel("Current path: "+ path_);
    numOfImages_ = new QLabel("Select the length of the sequence to save:");
    markup_ = new QLabel("Selected markup: "+ markupPath_);

    spinBox_ = new QSpinBox();
    spinBox_->setValue(frameCount_);
    spinBox_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinBox_->setMaximum(10);
    spinBox_->setMinimum(1);

    selectPath_ = new QPushButton("Change");
    selectPath_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    selectMarkup_ = new QPushButton("Select file");
    selectMarkup_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    accept_ = new QPushButton("Ok");
    accept_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    reject_ = new QPushButton("Cancel");
    reject_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QBoxLayout *main = new QBoxLayout(QBoxLayout::TopToBottom);

    QBoxLayout *path = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *count = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *markup = new QBoxLayout(QBoxLayout::LeftToRight);

    QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);

    count->addWidget(numOfImages_);
    count->addWidget(spinBox_,0,Qt::AlignRight);

    path->addWidget(currentPath_);
    path->addWidget(selectPath_,0,Qt::AlignRight);

    markup->addWidget(markup_);
    markup->addWidget(selectMarkup_,0,Qt::AlignRight);

    buttons->addStretch();
    buttons->addWidget(accept_,0,Qt::AlignRight);
    buttons->addWidget(reject_,0,Qt::AlignRight);

    main->addLayout(path);
    main->addLayout(count);
    main->addLayout(markup);
    main->addLayout(buttons);

    setLayout(main);
    setModal(true);

    connect(accept_,SIGNAL(clicked()),this,SLOT(accept()));
    connect(reject_,SIGNAL(clicked()),this,SLOT(reject()));
    connect(selectPath_,SIGNAL(clicked()),this,SLOT(onSelectFile()));
    connect(spinBox_,SIGNAL(valueChanged(int)),this,SLOT(onValueChange(int)));
    connect(selectMarkup_,SIGNAL(clicked()),this,SLOT(onSelectMarkup()));
}

void QSettingsWindow::onSelectFile()
{
    QString path;
    if (!(path = QFileDialog::getExistingDirectory(this,"Select a folder",path_)).isNull()){
        if (QDir(path)!=QDir(path_)){
            path_ = path;
            currentPath_->setText("Current path: "+ path_);
            isDirChanged_ = true;
        }else{
            isDirChanged_ = false;
        }
    }
}

void QSettingsWindow::onSelectMarkup()
{
    QString path;
    if (!(path = QFileDialog::getOpenFileName(this,"Select a markup image",markupPath_,tr("Images (*.png)"))).isNull()){
        markupPath_ = path;
        markup_->setText("Selected markup: " + markupPath_);
    }else{
        markupPath_ = QString();
        markup_->setText("Selected markup: ");
    }
}

void QSettingsWindow::onValueChange(int newValue)
{
    frameCount_ = newValue;
}


