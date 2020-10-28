#pragma once
#include "AbstractCommand.h"
class DatabaseCommand :
    public AbstractCommand
{
public:
    int activate(Client* c, list<string> params);
};

