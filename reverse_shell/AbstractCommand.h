#pragma once
#include "Client.h"
#include<string>
#include<list>
using namespace std;

class AbstractCommand
{
public:
	virtual int activate(Client* c, list<string> params) = 0;
};

