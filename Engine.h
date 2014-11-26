#pragma once

#include <Windows.h>

struct EngineMessage
{
	int func;
};

class Engine
{
public:
	HANDLE hEvent;
	Engine();
	virtual ~Engine();
	bool read(EngineMessage& msg);
};
