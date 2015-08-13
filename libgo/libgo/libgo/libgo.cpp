// libgo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libgo.h"

extern "C" {
#include "gnugo.h"
//#include "board.h"


static int ri=-1;
static int rj=-1;

// This is an example of an exported function.
LIBGO_API void cb_init_gnugo(void)
{
	init_gnugo(500,13951);
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
	gnugo_genmove(&i,&j,2);
	gnugo_play_move(i,j,2);

	printf("gen: %d, %d\n", i,j);

	showboard(0);

	float score = gnugo_estimate_score(0,0);
	printf("\nScore: %d\n", score);
}

}