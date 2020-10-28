#pragma once
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include "ExecDB.h"
#define execPtr ExecDB*

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class Client
{
private:
	struct addrinfo* result;
	SOCKET sock;
	execPtr database;
public:
	Client(char* ip, char* port);
	Client();
	DWORD connect_self();
	DWORD send_msg(char* buffer);
	DWORD send_msg(char* buffer, int len);
	char* recv_msg(DWORD len);
	char* recvall(DWORD* lng);
	execPtr getDB();
	~Client();
};

