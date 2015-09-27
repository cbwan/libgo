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

extern "C" {


static int ri=-1;
static int rj=-1;

bool g_IsInit = false;

FuegoMainEngine* g_Engine = 0;

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
	//return state.move_number;
	return 0;
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

}