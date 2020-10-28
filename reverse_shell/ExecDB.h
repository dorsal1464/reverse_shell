#pragma once
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include<map>
#include<string>

using namespace std;

class ExecDB
{
public:
	ExecDB();
	~ExecDB();
	LPWSTR GetExec(LPSTR name);
	int CreateFile(LPSTR name);
	int WriteData(LPSTR name, char* buffer, long len);
	int ExecData(LPSTR name);
	string ListData();
private:
	TCHAR pathBuffer[MAX_PATH];
	map<string, LPWSTR>* database;
};

