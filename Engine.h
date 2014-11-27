#pragma once

#include <Windows.h>

enum{
	ENG_none = 0, // No operation
	ENG_debug,	// 
	ENG_go,
	ENG_stop
};

struct EngineMessage
{
	int cmd;
};

class Engine
{
	HANDLE hThread;
public:
	bool debug;
	HANDLE hEvent;
	Engine();
	virtual ~Engine();
	void write(EngineMessage& msg);
	bool read(EngineMessage& msg);
};
