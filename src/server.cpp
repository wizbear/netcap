#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "netcap_common.h"
#include "server.h"
#include "server_opencv.h"
#include "server_keylogger.h"

#ifdef _WINDOWS
	#define SLEEP(A)	Sleep(A)
#else // _LINUX
	#include <inotifytools/inotifytools.h>
	#include <inotifytools/inotify.h>
	#include <unistd.h>
	#define SLEEP(A)	sleep(A)
#endif



using namespace cv;
using namespace std;

typedef unsigned char byte;



int server_main(const char* appname, unsigned short port, const char* dirname, int mode2, const char* serverip)
{
	const char* winname = serverip;
    
	char logfile[100];
	sprintf(logfile, "%s.server.log", appname);
	File log;
	log.open(logfile, File::WRITE);
	log.print(1, (char*)"===== %s server to (port:#%d) ", appname, port);
	
	int ret = 0;
	if ( (mode2 == 0) ) {
        log.print(1, (char*)"normal mode ====\n");
		ret = server(appname, port, dirname, mode2, winname, log);
	} else if (mode2 == 3) {
        log.print(1, (char*)"lim's server ====\n");
		ret = server2(appname, port, dirname, mode2, winname, log);		
	} else if (mode2 == 4) {
        log.print(1, (char*)"lim's server without client close ====\n");
		ret = server3(appname, port, dirname, mode2, winname, log);		
	} else if (mode2 == 5) {
        log.print(1, (char*)"key logger mode ====\n");
		ret = server5(appname, port, dirname, mode2, winname, log); 
    }
	log.close();
	return ret;
}

