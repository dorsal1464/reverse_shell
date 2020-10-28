#pragma once
#include "AbstractCommand.h"

class userCommand : public AbstractCommand
{
public:
    int activate(Client* c, list<string> params);
};

