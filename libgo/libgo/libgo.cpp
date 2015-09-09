// libgo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libgo.h"

extern "C" {
#include "gnugo.h"
//#include "board.h"

static int ri=-1;
static int rj=-1;

bool g_IsInit = false;

LIBGO_API void cb_init_gnugo(void)
{
	init_gnugo(500,13951);
	g_IsInit = true;
}

LIBGO_API bool cb_is_gnugo_init(void)
{
	return g_IsInit;
}

LIBGO_API void cb_gnugo_clear_board(int size)
{
	gnugo_clear_board( size );
}

LIBGO_API bool cb_gnugo_is_legal(int i, int j, int color)
{
	return (bool)gnugo_is_legal(i,j,color);
}

LIBGO_API void cb_gnugo_play_move(int i, int j, int color )
{
	gnugo_play_move(i,j,color);
}

LIBGO_API void cb_genmove(int color)
{
	int i,j;
	gnugo_genmove(&i,&j,color);
	gnugo_play_move(i,j,color);

	//printf("gen: %d, %d\n", i,j);

	//showboard(0);

	//float score = gnugo_estimate_score(0,0);
	//printf("\nScore: %d\n", score);
}

LIBGO_API int cb_get_board_color(int i, int j)
{
	static int b[MAX_BOARD][MAX_BOARD];
	gnugo_get_board(b);
	//if( 
	return b[i][j];
}

LIBGO_API bool cb_gnugo_undo_move(int n)
{
	gnugo_undo_move(n);
	return true;
}

LIBGO_API float cb_get_score()
{
	float upper,lower;

	return gnugo_estimate_score(&upper,&lower);
}

LIBGO_API int cb_get_white_captured()
{
	return white_captured;
}

LIBGO_API int cb_get_black_captured()
{
	return black_captured;
}

LIBGO_API int cb_gnugo_get_move_number(void)
{
	return gnugo_get_move_number();
}

LIBGO_API void  cb_gnugo_set_komi(float new_komi)
{
	gnugo_set_komi(new_komi);
}

LIBGO_API float cb_gnugo_get_komi()
{
	return gnugo_get_komi();
}

LIBGO_API int cb_gnugo_placehand(int handicap)
{
	return gnugo_placehand(handicap);
}

LIBGO_API bool cb_gnugo_is_ko()
{
	if( board_ko_pos > 0 )
	{
		return true;
	}
	return false;
}

LIBGO_API int cb_gnugo_get_ko_x()
{
	return I(board_ko_pos);
}

LIBGO_API int cb_gnugo_get_ko_y()
{
	return J(board_ko_pos);
}


}