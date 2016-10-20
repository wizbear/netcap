#ifdef _WINDOWS
	#define _CRT_SECURE_NO_WARNINGS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>        // perror()
#include <sys/types.h>    // socket
#include <stdlib.h>       // exit()
#include <string.h>       // strlen()
#include <sys/stat.h>     // open() 
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

#include "netcap_common.h"

#ifdef _WINDOWS
	#include <windows.h>
	#include <io.h>
	#pragma comment(lib, "ws2_32.lib")
	typedef int socklen_t;
	#define IS_INVALID_SOCKET(sock)  (sock == INVALID_SOCKET)
#else
	#include <stdarg.h>	  // va_start, va_end		
	#include <sys/socket.h>
	#include <netinet/in.h>   // sockaddr_in
	#include <netinet/ip.h> 
	#include <arpa/inet.h>    // inet_addr
	#include <unistd.h>       // read() 
	#include <sys/sendfile.h> // sendfile()
	#include <netinet/tcp.h>  // SOL_TCP, IPPROTO_TCP
	#include <dirent.h>       // opendir()
	#include <sys/ioctl.h>    // ioctl()
	#define IS_INVALID_SOCKET(sock)  ( sock < 0 )
#endif




#ifdef _WINDOWS 
	#define OPEN_READ(a)     _open((a),O_RDONLY)
	#define OPEN_WRITE(a)    _open((a),O_WRONLY|O_CREAT/*|O_EXCL*/, 0644);
	#define READ(a,b,c)      _read((a),(b),(c))
	#define WRITE(a,b,c)     _write((a),(b),(c))
	#define CLOSE_SOCKET(a)  closesocket(a)
	#define CLOSE_FILE(a)	 _close(a)
#else // _LINUX
	#define OPEN_READ(a)     ::open((a),O_RDONLY)
	#define OPEN_WRITE(a)    ::open((a),O_WRONLY | O_CREAT | O_EXCL, 0644);
	#define READ(a,b,c)      ::read((a),(b),(c))
	#define WRITE(a,b,c)     ::write((a),(b),(c))
	#define CLOSE_SOCKET(a)  ::close(a)	
	#define CLOSE_FILE(a)    ::close(a)
#endif

#include <errno.h>        // errno 

const char* _PKT_OK = "YAKO";
const int _LEN_OK = 4; 
const int _LEN_STR = 1024;
const int _LEN_INT = 22;  // 0 ~ 18446744073709551615

#ifdef _WINDOWS
char fname[255];
#include <windows.h>
char* getExeName()
{
	char fullpath[255];
	char drive[255];
	char dir[255];
	char ext[255];
	GetModuleFileName(GetModuleHandle(NULL), fullpath, 255);
	_splitpath(fullpath, drive, dir, fname, ext);
	return fname;
}

void socket_err(const char *msg, int err)
{
	void* buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&buf, 0, NULL );
	printf("%s %s\n", msg, (char*)buf);
	LocalFree(buf);
}
#else // _LINUX
char *getExeName()
{
	return NULL;
	//return argv[0]; // i don't know the type such as '/home/zooba/netcap'
}

void socket_err(const char* msg)
{
	perror(msg);
}
#endif

//
// File Class
// 
File::File()
{
	_fd = 0;
	_size = 0;
	memset(_name, 0x00, 1024);
	memset(_path, 0x00, 1024);
}

File::~File()
{
}

// internal 
char* getName(char* path)
{
	size_t len = strlen(path);
	while (len > 0) {
		if (path[len] == '/') return (path + len + 1);
		len--;
	}
	return path;
}

int File::open(const char* filename, mode m)
{
	if ( READ == m ) {
		_fd = OPEN_READ(filename);
	} else if ( WRITE == m ) {
		_fd = OPEN_WRITE(filename);
	}
	
	if ( _fd < 0 ) {
		printf("%s(%s) : %d, %s\n", __func__, filename, errno , strerror(errno));
	} else { // get file size 
		struct stat tmp;
		stat(filename, &tmp);
		_size = tmp.st_size;
		strcpy(_path, filename);
		strcpy(_name, getName(_path));
	}
	return 0;
}

int File::close()
{
	if ( 0 < _fd ) CLOSE_FILE(_fd);
	_fd = 0;
	_size = 0;
	memset(_path, 0x00, 1024);
	memset(_name, 0x00, 1024);
	return 0;
}

int File::read(char* buf, int size)
{
	int bytes;
	bytes = READ(_fd, buf, size);
	return bytes;
}

int File::write(char* buf, int size)
{
	int bytes;
	bytes = WRITE(_fd, buf, size);
	return bytes;
}

#ifdef _WINDOWS
int File::sendTo(Socket &s)
{
	return 0;
}
#else // _LINUX
int File::sendTo(Socket &s)
{
	if (0 > _fd) return 1;
	int ret = ::sendfile(s._sock, _fd, 0, _size);
	if (ret == -1) {
		printf("%s : %d, %s\n", __func__, errno, strerror(errno));
	}

	return ret;
}
#endif

int File::send(Socket &s)
{
	char buf[FSBUF];
	int total = 0;
	int ret;
	
	for (;;)
	{
		memset(buf, 0x00, FSBUF);
		int bytes;
		bytes = READ(_fd, buf, FSBUF);
		
        // If read was success, send data. 
        if( 0 < bytes ) {
			ret = s.send(buf, bytes);
		}

		total = total + bytes;
        // There is something tricky going on with read ..
        // Either there was error, or we reached end of file.
        if (bytes < FSBUF)
		{
			if (bytes == 0) printf("EOF\n");
			break;
		}

	}
    ret++; // dummy code
	printf("\n");
	return total;
}

int File::recv(Socket &s)
{
    int bytes = 0;
	int bytesTotal = 0;
	
    char buf[FSBUF];
    memset(buf, 0x00, FSBUF);
	
    while((bytes = s.recv(buf, FSBUF)) > 0)
    {
        printf("#");
		size_t ret = WRITE(_fd, buf, bytes);
		bytesTotal = bytesTotal + bytes;
    }

	printf("\n");
    if(bytes < 0)  {
        printf("%s : %d, %s\n", __func__, errno , strerror(errno));	
    }	
	return bytesTotal;
}

void File::lprint(char con, char *fmt, ...)
{
	char buf[512] = { 0, };
	va_list ap;
	strcpy(buf, "[L] ");
	va_start(ap, fmt);
	vsprintf(buf + strlen(buf), fmt, ap);
	va_end(ap);

	write(buf, (int)strlen(buf));
	if (1 == con) printf("%s", buf);
}

void File::eprint(char con, char *fmt, ...)
{
	char buf[512] = { 0, };
	va_list ap;
	strcpy(buf, "[L] ");
	va_start(ap, fmt);
	vsprintf(buf + strlen(buf), fmt, ap);
	va_end(ap);

	write(buf, (int)strlen(buf));
	if (1 == con) printf("%s", buf);
}

void File::pprint(char con, char *fmt, ...)
{
	char buf[512] = { 0, };
	va_list ap;
	strcpy(buf, "[P] ");
	va_start(ap, fmt);
	vsprintf(buf + strlen(buf), fmt, ap);
	va_end(ap);

	write(buf, (int)strlen(buf));
	if (1 == con) printf("%s", buf);
}

void File::print(char con, char *fmt, ...)
{
	char buf[512] = {0,};
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buf + strlen(buf), fmt, ap);
	va_end(ap);

	write(buf, (int)strlen(buf));
	if (1 == con) printf("%s", buf);	
}

int File::size() { return _size; };
const char* File::name() { return (const char*)_name; }
const char* File::path() { return (const char*)_path; }

//
//
//
//

Socket::Socket()
{
	_portno = 0;
	_sock = INVALID_SOCKET;
	if (_sock < 0) perror(__func__);
}

Socket::~Socket()
{
	if (_sock != INVALID_SOCKET) CLOSE_SOCKET(_sock);
}


int Socket::open()
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0) { perror(__func__); return _ERR; }
	
	return _NOERR;
}


int Socket::close()
{
	int ret = 0;
	ret = CLOSE_SOCKET(_sock);
	_sock = INVALID_SOCKET;
	if (-1 == ret) { perror(__func__); return _ERR; }

	return _NOERR;
}

int Socket::sendInt(int num)
{
	char buf[_LEN_INT]; 
	memset(buf, 0x00, _LEN_INT);
	sprintf(buf, "%d", num);

	int ret = send((void*)buf, _LEN_INT);
	return (_LEN_INT == ret) ? _NOERR : _ERR;
}

int Socket::recvInt(int &num)
{
	char buf[_LEN_INT];
	memset(buf, 0x00, _LEN_INT);
	
	int ret = recv(buf, _LEN_INT);
	if (ret == _LEN_INT) {
		num = atol(buf);
		return _NOERR;
	} else {
		num = 0;
		return _ERR;
	}
}

int Socket::sendStr(const char *str)
{
	int ret = send((void *)str, _LEN_STR);
	return (_LEN_STR == ret) ? _NOERR : _ERR;
}

int Socket::recvStr(char *str)
{
	int ret = recv(str, _LEN_STR);	
	return ( ret == _LEN_STR ) ? _NOERR : _ERR;
}

int Socket::sendOkay()
{
	int ret = send((void *)_PKT_OK, _LEN_OK);
	return ret;
}

int Socket::recvOkay()
{
	char buf[_LEN_OK+1];
	memset(buf, 0x00, _LEN_OK+1);
	
	int ret = recv(buf, _LEN_OK);
	if (ret == _ERR) return _ERR;
	if ( strcmp(buf, _PKT_OK) == 0 ) return _NOERR;
	return _ERR;
}

int Socket::recv(void *buf, int len)
{
	int left = len;
	int bytes = 0;
	unsigned int total = 0;
    
    unsigned long datasize = 0;
    do {
        bytes = ::recv(_sock, (char*)buf+total, left, 0);
		if (bytes == 0) {
            break;  // eof or connection closed
        } else if (bytes == -1) {
			printf("E: %s, %s(%d)\n", strerror(errno), __func__, __LINE__);
			return _ERR; // return -1;
		}
		left = left - bytes;
		total = total + bytes;
        
        if (datasize == total) break; // new added
    } while (left > 0);
	return total;
}

int Socket::send(void *buf, int len)
{
	int total = 0;
	int bytes = 0;	
	while ( total != len ) {
		bytes = ::send(_sock, (const char*)buf+total, (int)(len-total), 0); 
		if (bytes == -1) { perror(__func__); break; }
		total = total + bytes;
	}

	return (int)((bytes == -1) ? bytes : total);
}

void Socket::setReuse(int ov)
{
	if(setsockopt(_sock, SOL_SOCKET,SO_REUSEADDR, (const char*)&ov, sizeof(int)) < 0) {
		perror(__func__);
	}	
}

void Socket::setKeepAlive(char ov)
{
	if(setsockopt(_sock, SOL_SOCKET, SO_KEEPALIVE, &ov, sizeof(char)) < 0) {
		perror(__func__);
	}
}

void Socket::setLinger(char onff, char linger)
{
	struct linger tmp;
	tmp.l_onoff = onff;
	tmp.l_linger = linger; // 10
	
	if(setsockopt(_sock, SOL_SOCKET, SO_LINGER, (char*)&tmp, sizeof(tmp)) < 0) {
		perror("setsocketkeepalive()");
	}
}

#ifdef _WINDOWS
void Socket::setTcpCork(char ov)
{
	// do noting 
}
#else // _LINUX 
void Socket::setTcpCork(char ov)
{
	// TCP_CORK ON means don't send any data (partial frames) smaller than the MSS 
	// until the application says so or until 200ms later.
	// TCP_CORK OFF means send all the data (partial frames) now.
	if (setsockopt(_sock, SOL_TCP, TCP_CORK, &ov, sizeof(char)) < 0) {
		perror(__func__);
	}
}
#endif

void Socket::setTcpNoDelay(char ov)
{
	// TCP_NODELAY ON means send the data (partial frames) the moment you get, 
	// regardless if you have enough frames for a full network packet.
    // TCP_NODELAY OFF means Nagles Algoritm which means send the data 
	// when it is bigger than the MSS or waiting for the receiving acknowledgement 
	// before sending data which is smaller.
	if(setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, &ov, sizeof(char)) < 0) {
		perror(__func__);
	}
}


struct sockaddr_in &Socket::getSockAddr()
{
	return _addr;
}
//
//  Client Socket
//
//

void Socket::clone(SOCKET sock)
{
	_sock = sock;
}

void Socket::setSockAddr(const char *ip, unsigned short port)
{
	//_addr = new struct sockaddr_in;
	size_t len = sizeof(struct sockaddr_in);
	memset((char*)&_addr, 0x00, len);

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = (ip != NULL) ? inet_addr(ip) : INADDR_ANY;
	_addr.sin_port = htons(port);
}

const char* Socket::getErrMsg()
{
#ifdef _WINDOWS
	char *s = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPSTR)&s, 0, NULL);
	strcpy_s(_errmsg, 1024, s);
	//fprintf(stderr, "[ERR] %s %s", __func__, s);
	LocalFree(s);
#else
	strcpy(_errmsg, strerror(errno))
#endif
	return _errmsg;
}

ClientSocket::ClientSocket()
{
	
}

ClientSocket::~ClientSocket()
{
	
}

int ClientSocket::connect(const char* ip, unsigned short portno)
{
	setSockAddr(ip, portno);
	int len = sizeof(struct sockaddr_in);
	int ret = ::connect(_sock, (struct sockaddr*)&_addr, len);
	if ( ret != 0 ) {
		char msg[1024];
		strcpy(msg, getErrMsg());
		fprintf(stderr, "[ERR] %s %s", __func__, msg);
		return _ERR; 
	}
	return _NOERR;
}

//
//  Server Socket
//

ServerSocket::ServerSocket()
{
	
}

ServerSocket::~ServerSocket()
{

}

int ServerSocket::bind(unsigned short port)
{
	int len = sizeof(struct sockaddr_in);
	setSockAddr(NULL, port);
	
	if ( 0 > ::bind(_sock, (const struct sockaddr *)&_addr, len) ) {
		char msg[1024];
		strcpy(msg, getErrMsg());
		fprintf(stderr, "[ERR] %s %s", __func__, msg);
		return _ERR;
	}
	return _NOERR;
}

int ServerSocket::listen()
{
	if (0 > ::listen(_sock, 5)) {
		char msg[1024];
		strcpy(msg, getErrMsg());
		fprintf(stderr, "[ERR] %s %s", __func__, msg);
		return _ERR;
	}
	return _NOERR;	
}

int ServerSocket::accept(Socket &cs)
{
	socklen_t clen = sizeof(_addr);
	struct sockaddr_in &tmpaddr = cs.getSockAddr();

	SOCKET sock = ::accept(_sock, (struct sockaddr *)&tmpaddr, &clen);
	if (IS_INVALID_SOCKET(sock)) {
		char msg[1024];
		strcpy(msg, getErrMsg());
		fprintf(stderr, "[ERR] %s %s", __func__, msg);
		return _ERR;
	}
	cs.clone(sock);
	return _NOERR;
}

PerfTimer::PerfTimer()
{ 
	_elapsed = 0.0; 
}

PerfTimer::~PerfTimer()
{
}

void PerfTimer::start() 
{ 
	_start = clock(); 
}

void PerfTimer::stop() 
{ 
	_stop = clock(); 
	_elapsed = ((double)(_stop - _start) * 1000.0 / CLOCKS_PER_SEC); 
}

double PerfTimer::elapsed() 
{ 
	return _elapsed; 
}

void PerfTimer::reset() 
{ 
	_elapsed = 0.0; 
}

void PerfTimer::print(const char *funcname) 
{ 
	printf("[PERF] %s Time elapsed : %f ms\n", funcname, _elapsed); 
}

void PerfTimer::print(const char *filename, int fsize) 
{ 
	printf("[P] %s(%d Kbytes) Time elapsed : %f ms\n", filename, fsize/1024, _elapsed); 
}

void PerfTimer::print(const char *filename, int fsize, File &log) 
{ 
	_stop = clock();
	_elapsed = ((double)(_stop - _start) * 1000.0 / CLOCKS_PER_SEC); 
	log.pprint(1, (char*)" %s(%d Kbytes) Time elapsed : %f ms\n", filename, fsize / 1024, _elapsed);
}	

/* test that dir exists (1 success, -1 does not exist, -2 not dir) */
#ifdef _LINUX
int xis_dir (char *pathname)
{
  DIR *dirptr;
  if (access ( pathname, F_OK ) != -1 ) {
  // file exists
    if ((dirptr = opendir (pathname)) != NULL) {
      closedir (dirptr);
    } else {
      return -2; /* d exists, but not dir */
    }
  } else {
    return -1;     /* d does not exist */
  }
  return 1;
}
#else
int xis_dir(char *pathname)
{
	struct stat info;

	if (stat(pathname, &info) != 0) {
		printf("cannot access %s\n", pathname);
		return -1;
	}
	else if (info.st_mode & S_IFDIR) { // S_ISDIR() doesn't exist on my windows 
		printf("%s is a directory\n", pathname);
		return 1;
	}
	else {
		printf("%s is no directory\n", pathname);
		return -2;
	}
}
#endif

void get_time_stamp(char *buf)
{
	memset(buf, 0x00, 1024);
	time_t tm_time;
	struct tm *st_time;
	time(&tm_time);
	st_time = localtime(&tm_time);
	strftime(buf, 1024, "%a, %d %b %Y %H:%M:%S GMT", st_time);
}

const char* get_time_stamp2(char *buf)
{
	memset(buf, 0x00, 1024);
	time_t tm_time;
	struct tm *st_time;
	time(&tm_time);
	st_time = localtime(&tm_time);
	strftime(buf, 1024, "%H:%M:%S", st_time);
	return buf;
}