#include "Client.h"

Client::Client(char* ip, char* port)
{
	this->database = new ExecDB();
	this->result = NULL;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	//create addrinfo in kernel space
	DWORD iResult = getaddrinfo(ip, port, &hints, &this->result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		throw iResult;
	}

	//create socket
	this->sock = socket(this->result->ai_family, this->result->ai_socktype, this->result->ai_protocol);
	if (this->sock == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		throw iResult;
	}

}

Client::Client()
{
	const char* ip = "127.0.0.1";
	const char* port = "8008";
	Client((char*)ip, (char*)port);
}

DWORD Client::connect_self()
{
	//connect to server
	DWORD ret = connect(this->sock, this->result->ai_addr, this->result->ai_addrlen);
	return ret;
}

DWORD Client::send_msg(char* buffer)
{
	DWORD iResult = send(this->sock, buffer, (int)strlen(buffer), 0);
	if (iResult == SOCKET_ERROR) {
		closesocket(this->sock);
		printf("closing socket...\n");
	}
	return iResult;
}

DWORD Client::send_msg(char* buffer, int len)
{
	int iResult = send(this->sock, buffer,len, 0);
	if (iResult == SOCKET_ERROR) {
		printf("closing socket...\n");
		closesocket(this->sock);
	}
	return iResult;
}

char* Client::recv_msg(DWORD len)
{
	char* recvbfr = new char[len];
	int iResult = recv(this->sock, recvbfr, len, 0);
	if (iResult > 0) {
		//add string terminator
		recvbfr[min(iResult, len-1)] = 0;
		return recvbfr;
	}
	else
	{
		delete[] recvbfr;
		printf("%ld", WSAGetLastError());
		return NULL;
	}
}

char* Client::recvall(DWORD* lng)
{
	//recv msg length, assuming that both machines have the same indianess (<L)
	DWORD len;
	int iResult = recv(this->sock, (char*)&len, sizeof(DWORD), 0);
	if (iResult == SOCKET_ERROR)
		return NULL;
	char* recvbfr = new char[len+1];
	lng[0] = len + 1;
	DWORD left = len;
	while (left > 0) {
		//recv as much as possible
		iResult = recv(this->sock, recvbfr, len, 0);
		//update the buffer pointer and decrement already read bytes
		recvbfr += iResult;
		left -= iResult;
	}
	// end string with zero...
	recvbfr -= len;
	recvbfr[len] = 0;
	return recvbfr;
}

ExecDB* Client::getDB()
{
	return (ExecDB*)this->database;
}

Client::~Client() {
	closesocket(this->sock);
	printf("closing socket...\n");
	delete this->database;
}