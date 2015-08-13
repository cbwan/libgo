// libgo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libgo.h"

#include "gnugo.h"
//#include "board.h"

static int i=-1;
static int j=-1;

// This is an example of an exported function.
LIBGO_API void init(void)
{
	//init_gnugo(500,13951);
}

LIBGO_API int cb_genmove(int color)
{
	int resign;
	float value;
	int move = genmove(color,&value,&resign);
	return move;
}
