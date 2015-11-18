#include "qsettingswindow.h"

#include <QFileDialog>
#include <QBoxLayout>

QSettingsWindow::QSettingsWindow(QWidget *parent,
                                 int defFrameCount,
                                 int defVBSize,
                                 QString defPath,
                                 float principle_point_x,
                                 float principle_point_y,
                                 float focal_length_x,
                                 float focal_length_y,
                                 float near_plane,
                                 float far_plane): QDialog(parent)
{
    path_ = defPath;
    frameCount_ = defFrameCount;
    vbSize_ =defVBSize;
    markupPath_ = "";
    isDirChanged_ = false;

    currentPath_ =  new QLabel("Current path: "+ path_);
    numOfImages_ = new QLabel("Select number of frames to record:");
    videobufferSize_ = new QLabel("Select video buffer size:");
    markup_ = new QLabel("Selected markup: "+ markupPath_);
    calibration_label = new QLabel("Calibration Settings");
    principle_point_x_label = new QLabel("Principle Point X");
    principle_point_y_label = new QLabel("Principle Point Y");
    focal_length_x_label = new QLabel("Focal Length X");
    focal_length_y_label = new QLabel("Focal Length Y");
    near_plane_label = new QLabel("Near Plane");
    far_plane_label = new QLabel("Far Plane");
    principle_point_x_line = new QLineEdit(QString::number(principle_point_x));
    principle_point_x_line->setEnabled(false);
    principle_point_y_line = new QLineEdit(QString::number(principle_point_y));
    principle_point_y_line->setEnabled(false);
    focal_length_x_line = new QLineEdit(QString::number(focal_length_x));
    focal_length_x_line->setEnabled(false);
    focal_length_y_line = new QLineEdit(QString::number(focal_length_y));
    focal_length_y_line->setEnabled(false);
    near_plane_line = new QLineEdit(QString::number(near_plane));
    far_plane_line = new QLineEdit(QString::number(far_plane));

    spinBox_ = new QSpinBox();
    spinBox_->setValue(frameCount_);
    spinBox_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinBox_->setMaximum(500);
    spinBox_->setMinimum(1);

    vbSizeBox_ = new QSpinBox();
    vbSizeBox_->setValue(vbSize_);
    vbSizeBox_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    vbSizeBox_->setMaximum(1000);
    vbSizeBox_->setMinimum(1);

    selectPath_ = new QPushButton("Change");
    selectPath_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    selectMarkup_ = new QPushButton("Select file");
    selectMarkup_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    accept_ = new QPushButton("Ok");
    accept_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    reject_ = new QPushButton("Cancel");
    reject_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    QBoxLayout *main = new QBoxLayout(QBoxLayout::TopToBottom);

    QGridLayout *settings = new QGridLayout();
    QGridLayout *calibration_layout = new QGridLayout();

    QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);
    int line = 0;

    settings->addWidget(numOfImages_,line,0);
    settings->addWidget(spinBox_,line,1,1,1,Qt::AlignRight);
    line++;

    settings->addWidget(videobufferSize_,line,0);
    settings->addWidget(vbSizeBox_,line,1,1,1,Qt::AlignRight);
    line++;

    settings->addWidget(currentPath_,line,0);
    settings->addWidget(selectPath_,line,1,1,1,Qt::AlignRight);
    line++;

    settings->addWidget(markup_,line,0);
    settings->addWidget(selectMarkup_,line,1,1,1,Qt::AlignRight);

    line = 0;
    calibration_layout->addWidget(calibration_label,line,0,1,2,Qt::AlignCenter);
    line++;

    calibration_layout->addWidget(principle_point_x_label,line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(principle_point_x_line,line,1,1,1,Qt::AlignRight);
    line++;

    calibration_layout->addWidget(principle_point_y_label,line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(principle_point_y_line,line,1,1,1,Qt::AlignRight);
    line++;

    calibration_layout->addWidget(focal_length_x_label,line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(focal_length_x_line,line,1,1,1,Qt::AlignRight);
    line++;

    calibration_layout->addWidget(focal_length_y_label,line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(focal_length_y_line,line,1,1,1,Qt::AlignRight);
    line++;

    calibration_layout->addWidget(near_plane_label, line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(near_plane_line, line,1,1,1,Qt::AlignRight);
    line++;

    calibration_layout->addWidget(far_plane_label,line,0,1,1,Qt::AlignLeft);
    calibration_layout->addWidget(far_plane_line,line,1,1,1,Qt::AlignRight);
    line++;

    buttons->addStretch();
    buttons->addWidget(accept_,0,Qt::AlignRight);
    buttons->addWidget(reject_,0,Qt::AlignRight);

    main->addLayout(settings);
    main->addLayout(calibration_layout);
    main->addLayout(buttons);

    setLayout(main);
    setModal(true);

    connect(accept_,SIGNAL(clicked()),this,SLOT(accept()));
    connect(reject_,SIGNAL(clicked()),this,SLOT(reject()));
    connect(selectPath_,SIGNAL(clicked()),this,SLOT(onSelectFile()));
    connect(spinBox_,SIGNAL(valueChanged(int)),this,SLOT(onValueChange(int)));
    connect(vbSizeBox_,SIGNAL(valueChanged(int)),this,SLOT(onVBSizeChange(int)));
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

void QSettingsWindow::onVBSizeChange(int newValue)
{
    vbSize_ = newValue;
}


