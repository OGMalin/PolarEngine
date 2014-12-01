#pragma once

#include <Windows.h>
#include <queue>
#include "MoveList.h"
#include "BasicBoard.h"

enum{
	ENG_none = 0, // No operation
	ENG_abort,
	ENG_go,
	ENG_ponderhit,
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
	virtual ~EngineRequest(){};
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

	// Assignment
	EngineRequest& operator=(const EngineRequest& req)
	{
		if (&req != this)
			copy(req);
		return *this;
	};
};

struct EngineResponse
{
	int cmd;
	EngineResponse(){ clear(); };
	EngineResponse(const EngineResponse& res){ copy(res); };
	virtual ~EngineResponse(){};
	void clear()
	{
		cmd = ENG_none;
	};
	void copy(const EngineResponse& res)
	{
		cmd = res.cmd;
	};
	// Assignment
	EngineResponse& operator=(const EngineResponse& res)
	{
		if (&res != this)
			copy(res);
		return *this;
	};
};

class Engine
{
	HANDLE hThread;
	std::queue<EngineRequest> inQue;
	std::queue<EngineResponse> outQue;
	void start();
	void stop();
public:
	bool debug;
	HANDLE hEvent;
	HANDLE hEngineEvent;
	Engine();
	virtual ~Engine();
	// Write/read fom GUI
	void write(EngineRequest& msg);
	bool read(EngineResponse& msg);

	// Write/read from search
	bool get(EngineRequest& req);
	void set(EngineResponse& req);
};
