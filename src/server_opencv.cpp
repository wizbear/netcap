#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "netcap_common.h"
#include "server_opencv.h"

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



int server(const char* appname, unsigned short port, 
           const char* dirname, int mode2, const char* winname, File &log)
{
	ServerSocket ssock;
	int ret = 0; 
	ret = ssock.open();
	ssock.setReuse(1);
	
	ret = ssock.bind(port);
	ret = ssock.listen();

    namedWindow(winname, CV_WINDOW_AUTOSIZE); // create a window called "WebCam"	
	while(1) {
		PERF_START();		
		
		ClientSocket cs;
		ret = ssock.accept(cs); // STOP
		//ret = cs.open(ssock.accept());	
		
		// 0. Check Okay
		ret = cs.recvOkay();				
		ret = cs.sendOkay();

		// 1. get filename or tagname
		char fname[1024];
		memset(fname, 0x00, 1024);
		ret = cs.recvStr(fname);

		File f;
		char buf[200];
		memset(buf, 0x00, 200);
		sprintf(buf, "%s%s",dirname,fname);
		// ret = f.open(buf, File::WRITE);
		
		// 2. get file size
		int fsize = 0;
		ret = cs.recvInt(fsize);

		char* netbuf;
		netbuf = (char *)malloc(fsize);
		
		// 3. get file contents
		// int bytes = f.recv(cs);
		cs.recv(netbuf, fsize);
		Mat imgbuf = Mat(480, 640, CV_8U, netbuf);
		Mat frame = imdecode(imgbuf, CV_LOAD_IMAGE_COLOR);
		free(netbuf);		

		PERF_STOP3(fname, fsize, log);

		imshow(winname, frame);
		if (waitKey(30) == 27) { // wait for 'esc' key press for 30 ms.
			cout << "esc key is pressed by user" << endl;
		break;
		}
		cs.close();
	} // end of while

  destroyWindow(winname);

  return ret;
}

int server2(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log)
{

	ServerSocket ssock;

	int ret = ssock.open();
	ssock.setReuse(1);
	
	ret = ssock.bind(port);
	ret = ssock.listen();

	Mat image;
	//char buffer[200];
	int  t = 1;
	//// Day
	//VideoCapture vcDay(".//Data//01_day_test.avi");
	//if( !vcDay.isOpened() ){ return -1; }	
	
	while(1){
		//vcDay >> image;
		//if( image.empty() ){ break; }

		ClientSocket cs;
		ret = ssock.accept(cs); // STOP
		//ret = cs.open(ssock.accept());			
		
        char buf[1024];
        //int bufsize = 0;
		
		
		//_detect_img2msg(t, buf, bufsize);
        
		
		printf("S -> %s\n", buf);
        
        cs.send(buf, 1024);
        
        cs.close();

		//mISLDetDay.detect_img2msg((void*)&image, 1, 1, 0.5 );
		t++;
	}



    log.close();
	return ret;
}

int server3(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log)
{
    printf("HERE\n");
    int ret = 0;

	ServerSocket ssock;

	ret = ssock.open();
	ssock.setReuse(1);
	
	ret = ssock.bind(port);
	ret = ssock.listen();

	Mat image;
	//char buffer[200];
	int  t = 1;
	//// Day
	//VideoCapture vcDay(".//Data//01_day_test.avi");
	//if( !vcDay.isOpened() ){ return -1; }	
	
	while(1){
		//vcDay >> image;
		//if( image.empty() ){ break; }

		ClientSocket cs;
		ret = ssock.accept(cs); // STOP
		//ret = cs.open(ssock.accept());			
        char buf[1024];
        //int bufsize;
        t = 1;
        printf("***************** new client : index %d\n", t); 
        bool stop = false;
        
        while (stop == false) {    
           // _detect_img2msg(t, buf, bufsize);
        
            ret = cs.send(buf, 1024);
            printf("S(ret:%d) -> %s\n", ret, buf);
            if (ret == -1) {
                printf("send() ret == -1\n");
                stop = true;
            } else if (ret != 1024) {
                printf("******************* send() ret == %d\n", ret);
                stop = true;                
            } else {
                
            }
            //mISLDetDay.detect_img2msg((void*)&image, 1, 1, 0.5 );
            SLEEP(1);
            t++;
        }
        cs.close();
        printf("cs.close()\n");
	}

    log.close();
	return ret;
}

////////////////////
//
//
//
const char *hdr_tmpl = "HTTP/1.1 200 OK\r\n"
                        "Server: MiniSvr\r\n"
                        "Date: %s\r\nContent-Type: image/jpg\r\n"
                        "Content-Length: %d\r\n"
                        "Connection: keep-alive\r\n";
  // Date :                      
  //Date: Thu, 22 Sep 2016 17:18:49 GMT
  //Content-Type: image/png
  //Content-Length: 3744
  //Connection: keep-alive

int server4(const char* appname, unsigned short port, 
            const char* dirname, int mode2, const char* winname, File &log)
{
	// Camcoder Setup
    
    // init video capture device
	VideoCapture cap(0); // if you want to capture frame by movie file
						 // then assign filename instead of 0

	if (!cap.isOpened()) {
		cout << "Cannot open the capture device" << endl;
		return -1;
	}

	double width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Frame size : " << width << " x " << height << endl;

	//namedWindow(CLIENT_WINDOW, CV_WINDOW_AUTOSIZE); // create a window called "WebCam"

	// encoding parameters 
	vector<int> comp_param;
	comp_param.push_back(CV_IMWRITE_JPEG_QUALITY);
	comp_param.push_back(98); // specify the compression quailty
    
    //
    
	ServerSocket ssock;
	int ret = 0; 
	ret = ssock.open();
	ssock.setReuse(1);
	
	ret = ssock.bind(port);
	ret = ssock.listen();
    
	unsigned char* imageBuf;
    namedWindow(winname, CV_WINDOW_AUTOSIZE); // create a window called "WebCam"	
	while(1) {
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) { // if failed, break loop
			cout << "Can not read from capture device" << endl;
			break;
		}

		// encode image into jpg
		vector<uchar> imgbuf;
		int imgbufsize = 0;
		imencode(".jpg", frame, imgbuf, std::vector<int>() );
		imgbufsize = (int)imgbuf.size();
		// encoded image is now in buf (a vector)
		imageBuf = (unsigned char*)malloc(imgbufsize);
		memcpy(imageBuf, &imgbuf[0], imgbufsize);
		
		ClientSocket cs;
		ret = ssock.accept(cs); // STOP
		
		// 1. get filename or tagname
		char request[10240];
		memset(request, 0x00, 10240);
		ret = cs.recv(request, 10240);

        printf("recved : %d\n%s\n", ret, request);
        
        char timebuf[1024];
        get_time_stamp(timebuf);
        //printf("Date : %s\n", timebuf);
        
        char resp_hdr[2048];
        sprintf(resp_hdr, hdr_tmpl, timebuf, imgbufsize);

        int resp_len = strlen(resp_hdr);

        printf("resp_hdr : %s\n", resp_hdr);
        cs.send(resp_hdr, resp_len);
    
        cs.send(imageBuf, imgbufsize);
    
        free(imageBuf);

		//imshow(winname, frame);
		//if (waitKey(30) == 27) { // wait for 'esc' key press for 30 ms.
		//	cout << "esc key is pressed by user" << endl;
		//break;
		//}
        
		cs.close();
	} // end of while

  destroyWindow(winname);

  return ret;
}