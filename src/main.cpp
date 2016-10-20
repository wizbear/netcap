#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "netcap_common.h"
#include "server.h"
#include "client.h"

void usage(const char* appname) {
	printf("%s.conf file setting\n", appname);
	printf("in server case : (mode2 0: normal server, 1:lim server, 5:keylogger server\n");
	printf("  server\n  portno\n dirname\n mode2\n winname\n");
	printf("server example :\n");
	printf("  server\n  5555\n  ./tmp\n  0\n");	
	printf("in client case : (mode2  0:capture but not save, 1:capture and save, 2:inotify dir, 3: dummy_client\n");
	printf("  client\n  portno\n dirname\n mode2\n serverip\n");
	printf("client example :\n");
	printf("  client\n  5555\n  ./tmp\n  1\n  192.0.0.10\n");		
}


int main(int argc, char** argv)
{
#ifdef _WINDOWS
	WSADATA wsadata;
	WSAStartup(0x0202, &wsadata);
#endif

	char appname[100];
#ifdef _WINDOWS
	strcpy(appname, getExeName());
#else // _LINUX
	strcpy(appname, argv[0]);
#endif

	char confname[100];
	memset(confname, 0x00, 100);
	sprintf(confname, "%s.conf", appname);

	FILE *fp = fopen(confname, "r");
	if (fp == NULL) {
		printf("%s file can not be found.\n",confname);
		return -1;
	}
	
	char mode[100];
	memset(mode, 0x00, 100);
	if (fscanf(fp, "%s\n", mode)){};
	
	char portno[100] = {0, };	
	char dirname[100] = {0, };
	char mode2[100] = {0, };
	char serverip[100]  = {0, };
	unsigned short port = 0;	
	if (fscanf(fp, "%s", portno)){};
	if (fscanf(fp, "%s", dirname)){};
	if (fscanf(fp, "%s", mode2)){};	
	if (fscanf(fp, "%s", serverip)){};	
	fclose(fp);

	
	if ((strlen(portno) == 0) || (strlen(dirname) == 0) || 
		(strlen(mode2) == 0) || (strlen(serverip) == 0)) goto PARAM_ERR;


	port = (unsigned short)atoi(portno);
	if (strcmp(mode, "client") == 0) {
		return client_main(appname, port, dirname, atoi(mode2), serverip);
	} else if (strcmp(mode, "server") == 0) { 
		return server_main(appname, port, dirname, atoi(mode2), serverip);  // server ip == window name
	} 

PARAM_ERR:	
	fclose(fp);
	printf("please, check %s.conf file. \n", argv[0]);
	usage(argv[0]);
	return -1;
}

