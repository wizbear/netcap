#ifndef _CLIENT_H
#define _CLIENT_H

int client_send_file(const char* filename, const char* serverip, unsigned short port, File &log);

int client_send_buffer(const char* tag, const char* serverip, unsigned short port,
						const char* buf, int bufsize, File &log);

int client_recv_buffer(const char* serverip, unsigned short port, char* buf, int bufsize);

int dummy_client(const char* appname, unsigned short port, 
                 const char* dirname, int mode2,
				 const char* serverip, File &log );

int dummy_client2(const char* appname, unsigned short port, 
                  const char* dirname, int mode2,
				  const char* serverip, File &log );

int client_main(const char* appname, unsigned short port, 
	            const char* dirname, int mode2, 
		        const char* serverip);


class Client {
public :
	Client();
	~Client();
private :
};

#endif //_CLIENT_H