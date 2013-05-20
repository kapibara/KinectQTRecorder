#ifndef QSETTINGSWINDOW_H
#define QSETTINGSWINDOW_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

class QSettingsWindow: public QDialog
{
    Q_OBJECT
public:
    QSettingsWindow(QWidget *parent, int defFrameCount, QString defPath);

    int getFrameCount() {return frameCount_;}
    QString getPath() {return path_;}
    QString getMarkupPath() {return markupPath_;}
    bool isDirChanged() {return isDirChanged_;}

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
    QSpinBox *spinBox_;
    QPushButton *selectPath_;
    QPushButton *selectMarkup_;

    QPushButton *accept_;
    QPushButton *reject_;
};

#endif // QSETTINGSWINDOW_H
