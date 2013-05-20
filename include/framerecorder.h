#ifndef FRAMERECORDER_H
#define FRAMERECORDER_H

#include <QObject>

/*TODO: make a separate thread to save the data*/

class FrameRecorder : public QObject
{
    Q_OBJECT
public:
    explicit FrameRecorder(QObject *parent = 0);

signals:

public slots:

};

#endif // FRAMERECORDER_H
