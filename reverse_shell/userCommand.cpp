#include "userCommand.h"

using namespace std;

int userCommand::activate(Client* c, list<string> params) {
	LPWSTR name = new WCHAR[256];
	DWORD size;
	DWORD in = GetUserNameW(name, &size);
	c->send_msg((char*)name, size*2);
	return in;
}
