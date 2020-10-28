#pragma once
#include "AbstractCommand.h"
class ShellCommand : public AbstractCommand
{
public:
	int activate(Client* c, list<string> params);
};

