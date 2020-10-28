#include "ExecDB.h"

using namespace std;

ExecDB::ExecDB()
{
    lstrcpy(this->pathBuffer, L"C:\\TMP\\");
    CreateDirectoryW(this->pathBuffer, NULL);
    this->database = new map<string, LPWSTR>();
}

ExecDB::~ExecDB()
{
    delete this->database;
}

LPWSTR ExecDB::GetExec(LPSTR name)
{
    if (this->database->find(string(name)) != this->database->end()) {
        return this->database->find(string(name))->second;
    }
    return (LPWSTR)L"INVALID_FILE_VALUE";
}

int ExecDB::CreateFile(LPSTR name)
{
    LPWSTR tmpName = new TCHAR[1024];
    if (this->pathBuffer[0] != NULL) {
        if (GetTempFileNameW(this->pathBuffer, L"DB", 0, tmpName) != 0) {
            HANDLE hFile = CreateFileA((LPCSTR)(name), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            CloseHandle(hFile);
            this->database->insert({ name,  tmpName });
            return 1;
        }
        return 0;
    }
    return 0;
}

int ExecDB::WriteData(LPSTR name, char* buffer, long len)
{
    DWORD in;
    LPWSTR hName = this->GetExec(name);
    // test();
    HANDLE hFile = CreateFileA((LPCSTR)name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        if (WriteFile(hFile, buffer, len, &in, NULL)) { 
            printf("%d\n", GetLastError());
            in = FlushFileBuffers(hFile);
            in = CloseHandle(hFile); 
            return in;
        }
            
    }
    CloseHandle(hFile);
    return 0;
}

int ExecDB::ExecData(LPSTR name)
{
    LPWSTR hName = this->GetExec(name);
    DWORD in = 0;
    if (hName != (LPWSTR)L"INVALID_FILE_VALUE") {
        in = WinExec((LPCSTR)name, HIDE_WINDOW);
    }
    return in;
}

string ExecDB::ListData()
{
    string ans = "listing: ";
    for (auto itr = this->database->begin(); itr != this->database->end(); itr++) {
        ans += itr->first + string("\n");
    }
    return ans;
}