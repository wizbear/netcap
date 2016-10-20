#ifndef _SERVER_OPENCV_H
#define _SERVER_OPENCV_H

int server(const char* appname, unsigned short port, 
           const char* dirname, int mode2, const char* winname, File &log);

int server2(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log);

int server3(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log);

int server4(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log);
          

#endif // _SERVER_OPENCV_H