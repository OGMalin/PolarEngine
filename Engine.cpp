#include <process.h>
#include "Engine.h"
#include "SearchThread.h"

using namespace std;

CRITICAL_SECTION engCS;

Engine::Engine()
{
	debug = false;
	InitializeCriticalSection(&engCS);
	hEngineEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	start();
}

Engine::~Engine()
{
	stop();
	DeleteCriticalSection(&engCS);
	CloseHandle(hEngineEvent);
}

void Engine::start()
{
	hThread = (HANDLE)_beginthread(SearchThread, 0, this);
}

void Engine::stop()
{
	EngineRequest req;
	req.cmd = ENG_abort;
	write(req);
	if (WaitForSingleObject(hThread, 5000) == WAIT_TIMEOUT)
		TerminateThread(hThread, 2);
}

bool Engine::read(EngineResponse& res)
{
	bool ret = false;
	EnterCriticalSection(&engCS);
	if (!outQue.empty())
	{
		res = outQue.front();
		outQue.pop();
		ret = true;
	}
	LeaveCriticalSection(&engCS);
	return ret;
}

void Engine::write(EngineRequest& req)
{
	EnterCriticalSection(&engCS);
	inQue.push(req);
	LeaveCriticalSection(&engCS);
}

bool Engine::get(EngineRequest& req)
{
	bool ret = false;
	EnterCriticalSection(&engCS);
	if (!inQue.empty())
	{
		req = inQue.front();
		inQue.pop();
		ret = true;
	}
	LeaveCriticalSection(&engCS);
	return ret;
}

void Engine::set(EngineResponse& res)
{
	EnterCriticalSection(&engCS);
	outQue.push(res);
	LeaveCriticalSection(&engCS);
}

