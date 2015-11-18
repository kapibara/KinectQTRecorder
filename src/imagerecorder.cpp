#include "imagerecorder.h"

#include "QImage"
//#include <png.h>
#include <iostream>
#include <memory>

#include <fstream>

bool ImageRecorder::saveRGB(const std::string &filename,unsigned char *data, int width, int height)
{
     std::cerr << "saveRGB():start: " << width << ":" << height << std::endl;
    const QImage image(data,width,height,QImage::Format_RGB888);
    image.save(QString(filename.c_str()),"PNG");
     std::cerr << "saveRGB():done" << std::endl;
    return true;
}

bool ImageRecorder::saveIr(const std::string &filename,  unsigned short *data, int width, int height)
{
    std::ofstream binout(filename, std::ios::binary);

    binout.write((const char *)&width,sizeof(int));
    binout.write((const char *)&height,sizeof(int));
    binout.write((const char *)data,sizeof(short)*width*height);

    binout.close();

    return true;
}

bool ImageRecorder::saveDepth(const std::string &filename, unsigned short *data, int width, int height)
{
    /*uncompressed version*/
    std::ofstream binout(filename, std::ios::binary);

    binout.write((const char *)&width,sizeof(int));
    binout.write((const char *)&height,sizeof(int));
    binout.write((const char *)data,sizeof(short)*width*height);

    binout.close();

    //const QImage image(( unsigned char *)data,width,height,QImage::Format_RGB444);
    //image.save(QString(filename.c_str()),"PNG");
    return true;

    /*
    std::cerr << "trying to save: " << filename << std::endl;

    FILE *fp = fopen(filename.c_str(), "w");
    if(fp == NULL){
        std::cerr << "could not open a file" << std::endl;
        return false;
    }

    std::cerr << "file opened " << std::endl;

    // Initialize write structure
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        std::cerr << "could not create png_structp" <<std::endl;
        fclose(fp);
        return false;
    }

    // Initialize info structure
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        std::cerr << "could not create png_info" << std::endl;
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
        return false;
    }

    std::cerr << "png_init_io " << std::endl;

    png_init_io(png_ptr, fp);

    // Write header (16 bit depth)
    png_set_IHDR(png_ptr, info_ptr, width, height,
               8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    std::cerr << "png_write_info " << std::endl;

    png_write_info(png_ptr, info_ptr);


    std::cerr << "writing data: " << std::endl;

     // Write image data
     for (int y=0 ; y< height ; y++) {
        png_write_row(png_ptr, (png_bytep)(data+y*width));
     }

     // End write
     png_write_end(png_ptr, NULL);

     fclose(fp);
     png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
     png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

     return true;*/
}
