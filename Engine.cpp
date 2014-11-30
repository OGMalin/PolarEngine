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

bool Engine::read(EngineResponse& msg)
{
	return false;
}

void Engine::write(EngineRequest& msg)
{

}