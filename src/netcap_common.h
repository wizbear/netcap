#ifndef NETCAP_COMMON_H
#define NETCAP_COMMON_H

#ifdef _WINDOWS
	#define _CRT_SECURE_NO_WARNINGS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
    #define INVALID_SOCKET    (~0)
    #define SOCKET_ERROR      (-1)
    //#define INADDR_NONE       (-1)
	#include <netinet/in.h>
	typedef int SOCKET;
#endif

typedef unsigned char byte;

#define _ERR   (-1)
#define _NOERR (0)

const int FSBUF = 1024*100; // buffer size between file and sock

//
// Utilities
//
/* test that dir exists (1 success, -1 does not exist, -2 not dir) */
int xis_dir (char *d);
char* getExeName();

class Socket;

class File {
public:
	enum mode { READ, WRITE };
	File();
	~File();
	
	int open(const char* filename, mode m);
	int close();
	int read(char* buf, int bufsize);
	int write(char* buf, int bufsize);
	int sendTo(Socket &s);
	int send(Socket &s);
	int recv(Socket &s);
	
	int size();
	const char* name();
	const char* path();

	void lprint(char con, char *fmt, ...);   // [L]
	void eprint(char con, char *fmt, ...);  // [E]
	void pprint(char con, char *fmt, ...);  // [P]
	void print(char con, char *fmt, ...);   // nothing for prefix
private:
	int _size;
	char _name[1024];
	char _path[1024];
	int _fd;
};


class Socket {
public:
	Socket();
	~Socket();

	int open();
	int close();
	void clone(SOCKET sock);

	int sendInt(int num);
	int recvInt(int &num);
	int sendStr(const char *str); // send null-terminated 
	int recvStr(char *str); // get null-terminated
	int recv(void *buf, int len);
	int send(void *buf, int len);
	int sendOkay(); // _NOERR, _ERR
	int recvOkay(); // _NOERR, _ERR

	void setReuse(int ov);     // ov = 1, set Reuse
	void setKeepAlive(char ov); // ov = 1, set KeepAlive
	void setLinger(char onff, char linger);    // ov = 1, set Linger
	void setTcpCork(char ov);   // if ov = 1, set TcpCork. if ov = 0, reset TcpCork.
	void setTcpNoDelay(char ov); // if ov = 1, set TcpNoDelay. if ov = 0, reset TcpNoDelay.
	const char* getErrMsg();

	struct sockaddr_in &getSockAddr();

protected: 
	void setSockAddr(const char *ip, unsigned short port); // MUST FREE
	SOCKET _sock;
	struct sockaddr_in _addr;
	int _portno;
	char _errmsg[1024];

	friend class File;
};

//
//
//
class ClientSocket : public Socket {
public:
	ClientSocket();
	~ClientSocket();
	int connect(const char* ip, unsigned short portno);

	friend class ServerSocket;
};

//
//
//
class ServerSocket : public Socket {
public:
	ServerSocket();
	~ServerSocket();
	int bind(unsigned short port);
	int listen();
	//SOCKET accept();
	int accept(Socket &cs);
};

#define PERF_START()  		     PerfTimer _pt; _pt.start();
#define PERF_STOP3(fname, fsize, log) _pt.print(fname, fsize, log);        // stop and print 

class PerfTimer 
{
public:
	PerfTimer();
	~PerfTimer();
	void start();
	void stop();
	double elapsed();
	void reset();
	void print(const char *funcname);
	void print(const char *filename, int fsize);
	void print(const char *filename, int fsize, File &log);
private:
	char _buf[200];
	double _elapsed;
	long _start;
	long _stop;
};

// utilities
void get_time_stamp(char *buf);
const char* get_time_stamp2(char *buf); // build short format timestamp and return const char* type

#endif // NETCAP_COMMON_H

