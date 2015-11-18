#ifndef FRAMERECORDER_H
#define FRAMERECORDER_H

#include <QString>

/*TODO: make a separate thread to save the data*/

class ImageRecorder
{

public:
    static bool saveRGB(const std::string &filename,  unsigned char *data, int width, int height);
    static bool saveDepth(const std::string &filename,  unsigned short *data, int width, int height);
    static bool saveIr(const std::string &filename,  unsigned short *data, int width, int height);

};

#endif // FRAMERECORDER_H
