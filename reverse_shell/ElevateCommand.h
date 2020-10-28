#pragma once
#include "AbstractCommand.h"
class ElevateCommand : public AbstractCommand
{
public:
    int activate(Client* c, list<string> params);
};

