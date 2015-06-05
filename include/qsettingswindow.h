#ifndef QSETTINGSWINDOW_H
#define QSETTINGSWINDOW_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class QSettingsWindow: public QDialog
{
    Q_OBJECT
public:
    QSettingsWindow(QWidget *parent, int defFrameCount, QString defPath, float principle_point_x, float principle_point_y, float focal_length_x, float focal_length_y, float near_plane, float far_plane);

    int getFrameCount() {return frameCount_;}
    QString getPath() {return path_;}
    QString getMarkupPath() {return markupPath_;}
    bool isDirChanged() {return isDirChanged_;}
    QPair<float,float> getPrinciplePoint() {return qMakePair(principle_point_x_line->text().toFloat(),principle_point_y_line->text().toFloat());} ;
    QPair<float,float> getFocalLength() {return qMakePair(focal_length_x_line->text().toFloat(),focal_length_y_line->text().toFloat());} ;
    QPair<float,float> getNearFarPlane() {return qMakePair(near_plane_line->text().toFloat(),far_plane_line->text().toFloat());};

public slots:
    void onSelectFile();
    void onValueChange(int newValue);
    void onSelectMarkup();

public:
    QString path_;
    int frameCount_;
    QString markupPath_;

    bool isDirChanged_;

    QLabel *currentPath_;
    QLabel *numOfImages_;
    QLabel *markup_;
    QLabel *calibration_label;
    QLabel *principle_point_x_label;
    QLineEdit *principle_point_x_line;
    QLabel *principle_point_y_label;
    QLineEdit *principle_point_y_line;
    QLabel *focal_length_x_label;
    QLineEdit *focal_length_x_line;
    QLabel *focal_length_y_label;
    QLineEdit *focal_length_y_line;
    QLabel *far_plane_label;
    QLineEdit *far_plane_line;
    QLabel *near_plane_label;
    QLineEdit *near_plane_line;
    QSpinBox *spinBox_;
    QPushButton *selectPath_;
    QPushButton *selectMarkup_;

    QPushButton *accept_;
    QPushButton *reject_;
};

#endif // QSETTINGSWINDOW_H
