#pragma once

#include <Windows.h>
#include "MoveList.h"
#include "BasicBoard.h"

enum{
	ENG_none = 0, // No operation
	ENG_debug,	// 
	ENG_go,
	ENG_stop
};

struct EngineRequest
{
	int cmd;
	BasicBoard board;
	MoveList moves;
	MoveList searchmoves;
	bool ponder;
	DWORD wtime;
	DWORD btime;
	DWORD winc;
	DWORD binc;
	int movestogo;
	int depth;
	int nodes;
	int mate;
	int movetime;
	bool infinite;
	bool debug;
	EngineRequest(){ clear(); };
	EngineRequest(const EngineRequest& req){ copy(req); };
	~EngineRequest(){};
	void clear()
	{
		cmd = ENG_none;
		board.clear();
		moves.clear();
		searchmoves.clear();
		wtime = btime = winc = binc = 0;
		movestogo = depth = nodes = mate = movetime = 0;
		ponder = infinite = debug = false;
	};
	void copy(const EngineRequest& req)
	{
		cmd = req.cmd;
		board = req.board;
		moves = req.moves;
		searchmoves = req.searchmoves;
		ponder = req.ponder;
		wtime = req.wtime;
		btime = req.btime;
		winc = req.winc;
		binc = req.binc;
		movestogo = req.movestogo;
		depth = req.depth;
		nodes = req.nodes;
		mate = req.mate;
		movetime = req.movetime;
		infinite = req.infinite;
		debug = req.debug;
	};
};

struct EngineResponse
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
	void write(EngineRequest& msg);
	bool read(EngineResponse& msg);
};
