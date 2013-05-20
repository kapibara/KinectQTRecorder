#ifndef FRAMERECORDER_H
#define FRAMERECORDER_H

#include <opencv2/opencv.hpp>
#include <string>

/*TODO: make a separate thread to save the data*/

class ImageRecorder
{

public:
    static void saveRGB(const std::string &filename, unsigned char *data, int width, int height);
    static void saveDepth(const std::string &filename,  unsigned short *data, int width, int height);

};

#endif // FRAMERECORDER_H
