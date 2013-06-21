#include "imagerecorder.h"

#include <iostream>
using namespace std;

using namespace cv;

void ImageRecorder::saveRGB(const string &filename, unsigned char *data, int width, int height)
{
    Mat container = Mat(height,width,CV_8UC3,data);

    imwrite(filename,container);
}

void ImageRecorder::saveDepth(const string &filename, unsigned short *data, int width, int height)
{

    Mat container = Mat(height,width,CV_16UC1,data);

    imwrite(filename,container);
}
