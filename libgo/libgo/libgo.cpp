// libgo.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include "libgo.h"

#include "SgSystem.h"

#include "FuegoMainEngine.h"
#include "FuegoMainUtil.h"
#include "GoInit.h"
#include "GoBoard.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgInit.h"
#include "SgPlatform.h"

#include <ostream>

using namespace std;

extern "C" {


static int ri=-1;
static int rj=-1;

bool g_IsInit = false;

FuegoMainEngine* g_Engine = 0;

float g_MainTime = 25.0f;
float g_Overtime = 5.0f;
int   g_Overtime_Moves = 25;

LIBGO_API void cbgo_init(void)
{
	//init_gnugo(500,13951);
	SgInit();
	GoInit();
	g_Engine = new FuegoMainEngine(19);
	g_IsInit = true;
}

LIBGO_API bool cbgo_is_init(void)
{
	return g_IsInit;
}

LIBGO_API void cbgo_clear_board(int size)
{
	g_Engine->Init(size);
}

LIBGO_API bool cbgo_is_legal(int i, int j, int color)
{
	const GoBoard& board = g_Engine->Board();
	return g_Engine->Board().IsLegal(SgPointUtil::Pt(i+1,j+1), color);
}

LIBGO_API void cbgo_play_move(int i, int j, int color )
{
	g_Engine->Play(color, SgPointUtil::Pt(i+1,j+1) );
}

LIBGO_API int cbgo_get_genmove_x()
{
	return ri;
}

LIBGO_API int cbgo_get_genmove_y()
{
	return rj;
}

LIBGO_API void cbgo_genmove(int color)
{
	float val=-1;
	int res=-1;
	/*
	int move = genmove(2, &val, &res);
	
	ri = I(move);
	rj = J(move);
	*/
}

LIBGO_API int cbgo_get_board_color(int i, int j)
{
	int color = g_Engine->Board().GetColor( SgPointUtil::Pt(i+1,j+1) );
	return color;
}

LIBGO_API bool cbgo_undo_move(int n)
{
	g_Engine->Undo(n);
	return true;
}

LIBGO_API float cbgo_get_score()
{
	float komi = g_Engine->Board().Rules().Komi().ToFloat();
    float score = GoBoardUtil::Score(g_Engine->Board(), komi);

	return score;
}

LIBGO_API int cbgo_get_white_captured()
{
	return g_Engine->Board().NumPrisoners(SG_WHITE);
}

LIBGO_API int cbgo_get_black_captured()
{
	return g_Engine->Board().NumPrisoners(SG_BLACK);
}

LIBGO_API int cbgo_get_move_number(void)
{
	return g_Engine->Game().CurrentMoveNumber();
}

LIBGO_API void cbgo_set_komi(float new_komi)
{
	GtpCommand cmd; cmd.Init("komi 6.5");
	g_Engine->CmdKomi(cmd);
}

LIBGO_API float cbgo_get_komi()
{
	return g_Engine->Board().Rules().Komi().ToFloat();
}

LIBGO_API int cbgo_placehand(int handicap)
{
	return 0;
	//return gnugo_sethand(handicap,0);
}

LIBGO_API bool cbgo_is_ko()
{
	if( g_Engine->Board().KoPoint() != SG_NULLPOINT )
	{
		return true;
	}
	return false;
}

LIBGO_API int cbgo_get_ko_x()
{
	return SgPointUtil::Col(g_Engine->Board().KoPoint()) - 1;
}

LIBGO_API int cbgo_get_ko_y()
{
	return SgPointUtil::Row(g_Engine->Board().KoPoint()) - 1;
}

LIBGO_API bool cbgo_save_sgf( char* iFileName )
{
	try
	{
		g_Engine->SaveGame( iFileName );
		return true;
	}
	catch(...)
	{
		return false;
	}
}

LIBGO_API bool cbgo_is_capture_move()
{
	return g_Engine->Board().CapturingMove();
}

LIBGO_API void  cbgo_set_main_time( float time )
{
	g_MainTime = time;
}

LIBGO_API void  cbgo_set_overtime( float time )
{
	g_Overtime = time;
}

LIBGO_API void  cbgo_set_overtime_moves( int moves )
{
	g_Overtime_Moves = moves;
}

LIBGO_API void  cbgo_start_timer()
{
	SgTimeSettings settings( g_MainTime, g_Overtime, g_Overtime_Moves);

	ostringstream str; str << "time_settings " << g_MainTime << " " << g_Overtime << " " << g_Overtime_Moves;

	GtpCommand cmd; cmd.Init(str.str());

	g_Engine->CmdTimeSettings(cmd);
}

LIBGO_API float cbgo_get_time_left( int color )
{
	GtpCommand cmd; cmd.Init("clock");
	g_Engine->CmdClock(cmd);
	const SgTimeRecord& time = g_Engine->Game().Time();
	return time.TimeLeft( color );
}

LIBGO_API int cbgo_get_moves_left( int color )
{
	const SgTimeRecord& time = g_Engine->Game().Time();
	return time.MovesLeft( color );
}


}