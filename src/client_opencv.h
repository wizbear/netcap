#ifndef _CLIENT_OPENCV_H
#define _CLIENT_OPENCV_H

int capture_client(const char* appname, unsigned short port, 
                   const char* dirname, int mode2,
				   const char* serverip, File &log );

int inotify_client(const char* appname, unsigned short port, 
                   const char* dirname, int mode2,
				   const char* serverip, File &log );

#endif //_CLIENT_OPENCV_H