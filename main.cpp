#include <Windows.h>
#include <string>
#include "Console.h"
#include "Engine.h"
#include "Utility.h"
#include "StopWatch.h"
#include "ChessBoard.h"
#include "MoveGenerator.h"

//#include <iostream>

using namespace std;

#define ID_NAME "PolarChess 2.0b001" 
#define ID_AUTHOR "Odd Gunnar Malin" 

enum { WAIT_CONSOLE = WAIT_OBJECT_0, WAIT_ENGINE };

HANDLE hEvent[2];
Console con;
Engine eng;

bool debug=false;
bool ucimode = false;
ChessBoard startBoard, currentBoard;
MoveList currentMoves;

void start(std::string inifile)
{
	hEvent[0] = con.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[1] = eng.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	startBoard.setFen(STARTPOSITION);
	currentBoard.copy(startBoard);
	currentMoves.clear();
}

int perft(int depth, bool init, int ply)
{
	int moveit;
	static int testNodes;
	static MoveList testList[30];
	static MoveGenerator testGen;
	static ChessBoard b;
	if (init)
	{
		testNodes = 0;
		b.copy(currentBoard);
	}
	if (depth == 0)
		return ++testNodes;
	testGen.makeMoves(b, testList[ply]);
	moveit = 0;
	while (moveit != testList[ply].end())
	{
		testGen.doMove(b, testList[ply].list[moveit]);
		perft(depth - 1, false, ply + 1);
		testGen.undoMove(b, testList[ply].list[moveit]);
		++moveit;
	};
	return testNodes;
}

void startPerft(int depth)
{
	double d;
	int i;
	char sz[256];
	StopWatch sw;
	sw.start();
	i = perft(depth, true, 0);
	sw.stop();
	d = (double)sw.read() / 1000.0;
	sprintf_s(sz, 256, "%i nodes in %.2f seconds (%ik n/s).", i, d, (int)((double)i / (d * 1000)));
	con.write(sz);
}

// Command interpreter
bool doCommand(std::string& s)
{
	int next = 1;
	string arg1, arg2, arg3, cmd;

	cmd = lowercase(getWord(s, next++));
	if (cmd.empty())
		return true;

	//*uci
	//	tell engine to use the uci(universal chess interface),
	//	this will be sent once as a first command after program boot
	//	to tell the engine to switch to uci mode.
	//	After receiving the uci command the engine must identify itself with the "id" command
	//	and send the "option" commands to tell the GUI which engine settings the engine supports if any.
	//	After that the engine should send "uciok" to acknowledge the uci mode.
	//	If no uciok is sent within a certain time period, the engine task will be killed by the GUI.
	if (cmd == "uci")
	{
		ucimode = true;
		con.write("id name " + string(ID_NAME));
		con.write("id author " + string(ID_AUTHOR));

		con.write("uciok");
		return true;
	}

	//*debug[on | off]
	//	switch the debug mode of the engine on and off.
	//	In debug mode the engine should send additional infos to the GUI, e.g.with the "info string" command,
	//	to help debugging, e.g.the commands that the engine has received etc.
	//	This mode should be switched off by default and this command can be sent
	//	any time, also when the engine is thinking.
	if (cmd == "debug")
	{
		debug = con.debug = eng.debug = booleanString(getWord(s, next++));
		return true;
	}

	//* isready
	//	this is used to synchronize the engine with the GUI.When the GUI has sent a command or
	//	multiple commands that can take some time to complete,
	//	this command can be used to wait for the engine to be ready again or
	//	to ping the engine to find out if it is still alive.
	//	E.g. this should be sent after setting the path to the tablebases as this can take some time.
	//	This command is also required once before the engine is asked to do any search
	//	to wait for the engine to finish initializing.
	//	This command must always be answered with "readyok" and can be sent also when the engine is calculating
	//	in which case the engine should also immediately answer with "readyok" without stopping the search.
	if (cmd == "isready")
	{
		con.write("readyok");
		return true;
	}
	
	//* setoption name <id>[value <x>]
	//	this is sent to the engine when the user wants to change the internal parameters
	//	of the engine.For the "button" type no value is needed.
	//	One string will be sent for each parameter and this will only be sent when the engine is waiting.
	//	The name and value of the option in <id> should not be case sensitive and can inlude spaces.
	//	The substrings "value" and "name" should be avoided in <id> and <x> to allow unambiguous parsing,
	//	for example do not use <name> = "draw value".
	//	Here are some strings for the example below :
	//		"setoption name Nullmove value true\n"
	//		"setoption name Selectivity value 3\n"
	//		"setoption name Style value Risky\n"
	//		"setoption name Clear Hash\n"
	//		"setoption name NalimovPath value c:\chess\tb\4;c:\chess\tb\5\n"
	if (cmd == "setoption")
	{
		return true;
	}

	//* register
	//	this is the command to try to register an engine or to tell the engine that registration
	//	will be done later.This command should always be sent if the engine	has sent "registration error"
	//	at program startup.
	//	The following tokens are allowed :
	//	*	later
	//			the user doesn't want to register the engine now.
	//	* name <x>
	//			the engine should be registered with the name <x>
	//	* code <y>
	//			the engine should be registered with the code <y>
	//	Example:
	//			"register later"
	//			"register name Stefan MK code 4359874324"
	if (cmd == "register")
	{
		return true;
	}

	//* ucinewgame
	//	this is sent to the engine when the next search(started with "position" and "go") will be from
	//	a different game.This can be a new game the engine should play or a new game it should analyse but
	//	also the next position from a testsuite with positions only.
	//	If the GUI hasn't sent a "ucinewgame" before the first "position" command, the engine shouldn't
	//	expect any further ucinewgame commands as the GUI is probably not supporting the ucinewgame command.
	//	So the engine should not rely on this command even though all new GUIs should support it.
	//	As the engine's reaction to "ucinewgame" can take some time the GUI should always send "isready"
	//	after "ucinewgame" to wait for the engine to finish its operation.
	if (cmd == "ucinewgame")
	{
		startBoard.setFen(STARTPOSITION);
		currentBoard.copy(startBoard);
		currentMoves.clear();
		// Clear hash etc.
		return true;
	}

	//* position [fen <fenstring> | startpos]  moves <move1> .... <movei>
	//	set up the position described in fenstring on the internal board and
	//	play the moves on the internal chess board.
	//	if the game was played  from the start position the string "startpos" will be sent
	//	Note : no "new" command is needed.However, if this position is from a different game than
	//					 the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween.
	if (cmd == "position")
	{
		currentBoard.clear();
		startBoard.clear();
		currentMoves.clear();
		arg1 = getWord(s, next++);
		while (!arg1.empty())
		{
			if (arg1 == "fen")
			{
				string fen;
				arg2 = getWord(s, next++);
				while (!arg2.empty())
				{
					if (arg2 == "moves")
					{
						--next;
						break;
					}
					fen += ' ';
					fen += arg2;
					arg2 = getWord(s, next++);
				}
				fen = trim(fen);
				startBoard.setFen(fen.c_str());
			}
			else if (arg1 == "startpos")
			{
				startBoard.setFen(STARTPOSITION);
			}
			else if (arg1 == "moves")
			{
				ChessMove m;
				currentBoard.copy(startBoard);
				arg2 = getWord(s, next++);
				while (!arg2.empty())
				{
					m=currentBoard.getMoveFromText(arg2);
					currentMoves.push_back(m);
					currentBoard.move(m);
					arg2 = getWord(s, next++);
				}
			}
			arg1 = getWord(s, next++);
		}
		return true;
	}

	//* go
	//		start calculating on the current position set up with the "position" command.
	//		There are a number of commands that can follow this command, all will be sent in the same string.
	//		If one command is not sent its value should be interpreted as it would not influence the search.
	//		* searchmoves <move1> .... <movei>
	//			restrict search to this moves only
	//			Example : After "position startpos" and "go infinite searchmoves e2e4 d2d4"
	//			the engine should only search the two moves e2e4 and d2d4 in the initial position.
	//		* ponder
	//			start searching in pondering mode.
	//			Do not exit the search in ponder mode, even if it's mate!
	//			This means that the last move sent in in the position string is the ponder move.
	//			The engine can do what it wants to do, but after a "ponderhit" command
	//			it should execute the suggested move to ponder on.This means that the ponder move sent by
	//			the GUI can be interpreted as a recommendation about which move to ponder.However, if the
	//			engine decides to ponder on a different move, it should not display any mainlines as they are
	//			likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
	//			on the suggested move.
	//			* wtime <x>
	//				white has x msec left on the clock
	//			* btime <x>
	//				black has x msec left on the clock
	//			* winc <x>
	//				white increment per move in mseconds if x > 0
	//			* binc <x>
	//				black increment per move in mseconds if x > 0
	//			* movestogo <x>
	//				there are x moves to the next time control,
	//				this will only be sent if x > 0,
	//				if you don't get this and get the wtime and btime it's sudden death
	//			* depth <x>
	//				search x plies only.
	//			* nodes <x>
	//				search x nodes only,
	//			*	mate <x>
	//				search for a mate in x moves
	//			* movetime <x>
	//				search exactly x mseconds
	//			* infinite
	//				search until the "stop" command.Do not exit the search without being told so in this mode!
	if (cmd == "go")
	{
		EngineRequest req;
		ChessMove m;
		MoveList ml;

		req.cmd = ENG_go;
		req.board = startBoard;
		req.moves = currentMoves;

		arg1 = getWord(s, next++);
		while (!arg1.empty())
		{
			if (arg1 == "searchmoves")
			{
				arg2 = getWord(s, next++);
				while (!arg2.empty())
				{
					m = currentBoard.getMoveFromText(arg2);
					if (m.empty())
					{
						--next;
						break;
					}
					req.searchmoves.push_back(m);
					arg2 = getWord(s, next++);
				}
			}
			else if (arg1 == "ponder")
			{
				req.ponder = true;
			}
			else if (arg1 == "wtime")
			{
				req.wtime = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "btime")
			{
				req.btime = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "winc")
			{
				req.winc = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "binc")
			{
				req.binc = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "movestogo")
			{
				req.movestogo = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "depth")
			{
				req.depth = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "nodes")
			{
				req.nodes = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "mate")
			{
				req.mate = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "movetime")
			{
				req.movetime = atoi(getWord(s, next++).c_str());
			}
			else if (arg1 == "infinite")
			{
				req.infinite = true;
			}
			arg1 = getWord(s, next++);
		}
		eng.write(req);
		return true;
	}

	//* stop
	//	stop calculating as soon as possible,
	//	don't forget the "bestmove" and possibly the "ponder" token when finishing the search
	if (cmd == "stop")
	{
		EngineRequest msg;
		msg.cmd = ENG_stop;
		eng.write(msg);
		return true;
	}

	//* ponderhit
	//	the user has played the expected move.This will be sent if the engine was told to ponder on the same move
	//	the user has played.The engine should continue searching but switch from pondering to normal search.
	if (cmd == "ponderhit")
	{
		return true;
	}
	
	//*quit
	//	quit the program as soon as possible
	if (cmd == "quit")
	{
		return false;
	}

	//* test
	//	Testcommand for use under developing.
	//	* perft <depth>
	if (cmd == "test")
	{
		if (ucimode)
			return true;
		arg1 = getWord(s, next++);
		if (arg1 == "perft")
		{
			arg2 = getWord(s, next++);
			startPerft(atoi(arg2.c_str()));
		}
		return true;
	}

	if (debug)
		con.write("info string Unknown command: " + cmd);

	return true;
}

void EngineMsg(EngineResponse& msg)
{

}

// The engines main loop.
void run()
{
	DWORD dw;
	string s;
	EngineResponse engine_msg;
	while (1)
	{
		dw = WaitForMultipleObjects(2, hEvent, FALSE, 100);
		switch (dw)
		{
			case WAIT_CONSOLE: // Command from GUI
				while (con.read(s))
				{
					if (debug)
						con.write("info string " + s);
					if (!doCommand(s))
						return;
				}
				break;
			case WAIT_ENGINE: // Meldinger fra sjakkmotor
				while (eng.read(engine_msg))
				{
					EngineMsg(engine_msg);
				}
				break;
			case WAIT_TIMEOUT:
				break;
			default:
				if (debug)
					con.write("info string Unknown event in main loop.");
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
	CloseHandle(hEvent[1]);
}