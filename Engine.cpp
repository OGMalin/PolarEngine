#include "Engine.h"

using namespace std;

CRITICAL_SECTION engCS;

Engine::Engine()
{
	debug = false;
	InitializeCriticalSection(&engCS);
}

Engine::~Engine()
{
	DeleteCriticalSection(&engCS);
}

bool Engine::read(EngineMessage& msg)
{
	return false;
}

void Engine::write(EngineMessage& msg)
{

}