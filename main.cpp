#include <Windows.h>
#include <string>
#include "Console.h"
#include "Engine.h"
#include "Utility.h"
#include <iostream>

using namespace std;

enum { WAIT_CONSOLE = WAIT_OBJECT_0, WAIT_ENGINE };

HANDLE hEvent[2];
Console con;
Engine eng;

void start(std::string inifile)
{
	hEvent[0] = con.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[1] = eng.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

bool doCommand(std::string& s)
{
	int next = 1;
	string cmd = lowercase(getWord(s, next++));
	if (cmd.empty())
		return true;

	if (cmd == "quit")
	{
		return false;
	}

	if (cmd == "uci")
	{
		return true;
	}

	cerr << "Ukjent kommando: " << cmd << endl;
	return true;
}

void run()
{
	DWORD dw;
	string s;
	while (1)
	{
		dw = WaitForMultipleObjects(2, hEvent, FALSE, 100);
		switch (dw)
		{
			case WAIT_CONSOLE: // Kommandoer fra konsolle
				while (con.read(s))
				{
					if (!doCommand(s))
						return;
				}
				break;
			case WAIT_ENGINE: // Meldinger fra sjakkmotor
//				while (eng.read(s))
//				{
//					engine(s);
//				}
				break;
			case WAIT_TIMEOUT:
				break;
			default:
				cerr << "Ukjent event." << endl;
				break;
		}
	}
}

int main(int argc, char* argv[])
{
	string inifile;

	if (argc > 1)
		inifile = argv[1];
	else
		inifile = "config.ini";

	start(inifile);
	run();

	CloseHandle(hEvent[0]);
}