// libgo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libgo.h"

extern "C" {
#include "gnugo.h"
//#include "board.h"

static int ri=-1;
static int rj=-1;

LIBGO_API void cb_init_gnugo(void)
{
	init_gnugo(500,13951);
}

LIBGO_API void cb_gnugo_clear_board(int size)
{
	gnugo_clear_board( size );
}

LIBGO_API bool cb_gnugo_is_legal(int i, int j, int color)
{
	return gnugo_is_legal(i,j,color);
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

}