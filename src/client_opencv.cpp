#include <opencv2/opencv.hpp>
#include "netcap_common.h"

#ifdef _LINUX
	#include <inotifytools/inotifytools.h>
	#include <inotifytools/inotify.h>
#endif

#define CLIENT_WINDOW "client_window"

using namespace cv;
using namespace std;

#include "client.h"


byte* MatToBytes(Mat &image, int &size)
{
   size  =  image.rows * image.cols;
   byte* tmp = (byte *)malloc(sizeof(byte)*size);
   memcpy(tmp,  image.data,  size * sizeof(byte));
   return tmp;
}

int BytestoMat(byte *buf, int width, int height, Mat &frame)
{
    frame  = Mat(height, width, CV_8UC3, buf);
    return 0;
}

int capture_client(const char* appname, unsigned short port, 
                   const char* dirname, int mode2,
				   const char* serverip, File &log )
{	
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

	namedWindow(CLIENT_WINDOW, CV_WINDOW_AUTOSIZE); // create a window called "WebCam"

	// encoding parameters 
	vector<int> comp_param;
	comp_param.push_back(CV_IMWRITE_JPEG_QUALITY);
	comp_param.push_back(98); // specify the compression quailty

	int idx = 0;
	char tag[1024];
	unsigned char* imageBuf;
	int ret = 0;

	while(1) {
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) { // if failed, break loop
			cout << "Can not read from capture device" << endl;
			break;
		}

		imshow(CLIENT_WINDOW, frame);


		// encode image into jpg
		vector<uchar> buf;
		int bufsize = 0;
		imencode(".jpg", frame, buf, std::vector<int>() );
		bufsize = (int)buf.size();
		// encoded image is now in buf (a vector)
		imageBuf = (unsigned char*)malloc(bufsize);
		memcpy(imageBuf, &buf[0], bufsize);

		sprintf(tag, "captured_frame_%06d", idx % 100000);
		ret = client_send_buffer(tag, serverip, port, (const char *)imageBuf, bufsize, log);
		if (mode2 == 1) { // save mode
			char filename[1024];
			sprintf(filename, "%s/%s.jpg", dirname, tag);
			File f;
			f.open(filename, File::WRITE);
			f.write((char*)imageBuf, bufsize);
			f.close();
		}
		
		free(imageBuf);

		if (ret == _ERR) {
			break;
		}
		idx++;

		if (waitKey(10) == 27) { // wait for 'esc' key press for 30 ms.
			cout << "esc key is pressed by user" << endl;
			break;
		} // waitKey

	 } // while 

	// close log
	destroyWindow(CLIENT_WINDOW);	
	return 0;
}

#ifdef _LINUX
int inotify_client(const char* appname, unsigned short port, 
                   const char* dirname, int mode2,
				   const char* serverip, File &log )
{
	
	// init and watch the entire directory tree from the current working
 	// directory 
	log.print(1, (char*)"inotify mode\n");
	
	if ( !inotifytools_initialize() 
	    || !inotifytools_watch_recursively( dirname, IN_CLOSE_WRITE )) {
		log.eprint(1, (char*)"%s %s\n", __func__, (char*)strerror(inotifytools_error() ));
		return _ERR;
	}
	int ret = 0;
	inotifytools_set_printf_timefmt((char*)"%T");
	struct inotify_event* event = inotifytools_next_event(-1);
	char filename[1024];
	while (event) {
		inotifytools_printf(event, (char*)"%T %w%f %e \n");
		inotifytools_snprintf(filename, 1024, event, (char*)"%w%f");
		if (strcmp(filename, "EXIT") == 0) break;
		ret = client_send_file(filename, serverip, port, log);		
		event = inotifytools_next_event(-1);
	} 

	inotifytools_cleanup();
	
	return ret;
}
#else // _WINDOWS
int inotify_client(const char* appname, unsigned short port,
	const char* dirname, int mode2,
	const char* serverip, File &log)
{
	printf("inotify_client() is not supported in Windows Platrom \n");
	return 0;
}
#endif

