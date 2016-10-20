#ifndef _CLIENT_KEYLOGGER_H
#define _CLIENT_KEYLOGGER_H

int client_keylogger(const char* appname, unsigned short port,
                     const char* dirname, int mode2,
				     const char* serverip, File &log );

#endif //_CLIENT_KEYLOGGER_H