#pragma once

#include <Windows.h>
#include <queue>

class Console
{
	bool abort;
	HANDLE hRead;
	HANDLE hWrite;
	HANDLE hThread;
	void input(const std::string s);
	static void threadLoop(void* lpv);
	std::queue<std::string> inQue;
public:
	HANDLE hEvent;
	Console();
	virtual ~Console();
	bool read(std::string &s);
	void write(std::string s);
};
