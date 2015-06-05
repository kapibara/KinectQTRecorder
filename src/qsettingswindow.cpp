#include "qsettingswindow.h"

#include <QFileDialog>
#include <QBoxLayout>

QSettingsWindow::QSettingsWindow(QWidget *parent, int defFrameCount, QString defPath, float principle_point_x, float principle_point_y, float focal_length_x, float focal_length_y, float near_plane, float far_plane): QDialog(parent)
{
    path_ = defPath;
    frameCount_ = defFrameCount;
    markupPath_ = "";
    isDirChanged_ = false;

    currentPath_ =  new QLabel("Current path: "+ path_);
    numOfImages_ = new QLabel("Select the length of the sequence to save:");
    markup_ = new QLabel("Selected markup: "+ markupPath_);
    calibration_label = new QLabel("Calibration Settings:");
    principle_point_x_label = new QLabel("Principle Point X");
    principle_point_y_label = new QLabel("Principle Point Y");
    focal_length_x_label = new QLabel("Focal Length X");
    focal_length_y_label = new QLabel("Focal Length Y");
    near_plane_label = new QLabel("Near Plane");
    far_plane_label = new QLabel("Far Plane");
    principle_point_x_line = new QLineEdit(QString::number(principle_point_x));
    principle_point_y_line = new QLineEdit(QString::number(principle_point_y));
    focal_length_x_line = new QLineEdit(QString::number(focal_length_x));
    focal_length_y_line = new QLineEdit(QString::number(focal_length_y));
    near_plane_line = new QLineEdit(QString::number(near_plane));
    far_plane_line = new QLineEdit(QString::number(far_plane));

    spinBox_ = new QSpinBox();
    spinBox_->setValue(frameCount_);
    spinBox_->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    spinBox_->setMaximum(500);
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
    QBoxLayout *calibration_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QBoxLayout *principle_point_x_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *principle_point_y_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *focal_length_x_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *focal_length_y_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *far_plane_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    QBoxLayout *near_plane_layout = new QBoxLayout(QBoxLayout::LeftToRight);

    QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);

    count->addWidget(numOfImages_);
    count->addWidget(spinBox_,0,Qt::AlignRight);

    path->addWidget(currentPath_);
    path->addWidget(selectPath_,0,Qt::AlignRight);

    markup->addWidget(markup_);
    markup->addWidget(selectMarkup_,0,Qt::AlignRight);

    calibration_layout->addWidget(calibration_label);

    principle_point_x_layout->addWidget(principle_point_x_label, Qt::AlignLeft);
    principle_point_x_layout->addWidget(principle_point_x_line, Qt::AlignRight);

    principle_point_y_layout->addWidget(principle_point_y_label, Qt::AlignLeft);
    principle_point_y_layout->addWidget(principle_point_y_line, Qt::AlignRight);

    focal_length_x_layout->addWidget(focal_length_x_label, Qt::AlignLeft);
    focal_length_x_layout->addWidget(focal_length_x_line, Qt::AlignRight);

    focal_length_y_layout->addWidget(focal_length_y_label, Qt::AlignLeft);
    focal_length_y_layout->addWidget(focal_length_y_line, Qt::AlignRight);

    near_plane_layout->addWidget(near_plane_label, Qt::AlignLeft);
    near_plane_layout->addWidget(near_plane_line, Qt::AlignRight);

    far_plane_layout->addWidget(far_plane_label, Qt::AlignLeft);
    far_plane_layout->addWidget(far_plane_line, Qt::AlignRight);

    calibration_layout->addLayout(principle_point_x_layout);
    calibration_layout->addLayout(principle_point_y_layout);
    calibration_layout->addLayout(focal_length_x_layout);
    calibration_layout->addLayout(focal_length_y_layout);
    calibration_layout->addLayout(near_plane_layout);
    calibration_layout->addLayout(far_plane_layout);

    buttons->addStretch();
    buttons->addWidget(accept_,0,Qt::AlignRight);
    buttons->addWidget(reject_,0,Qt::AlignRight);

    main->addLayout(path);
    main->addLayout(count);
    main->addLayout(markup);
    main->addLayout(calibration_layout);
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


