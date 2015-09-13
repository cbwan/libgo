// Go.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
//#include "libgo.h"

extern "C" {
#include "gnugo.h"
#include "utils\gg_utils.h"
}

using namespace std;

void genmove()
{
	int i,j;
	//genmove(&i,&j,2);
	//gnugo_play_move(i,j,2);
	float val=-1;
	int res=-1;
	int move = genmove(2, &val, &res);
	gnugo_play_move(move, 2);

	printf("gen: move:%d, %d, %d\n", move, I(move), J(move));

	showboard(0);

	float score = gnugo_estimate_score(0,0);
	printf("\nScore: %d\n", score);

	printf("White captured: %d\n", white_captured);
}

int _tmain(int argc, _TCHAR* argv[])
{
	init_gnugo(500.0f,13951);

	while(1)
	{
		int x,y;

		int ux;
		char uy;
		cout << "Your turn." << endl << "A-Z ? ";
		cin >> uy;
		cout << "0-99 ? ";
		cin >> ux;

		y = uy-'A';
		x = 19-ux;

		if( is_legal(POS(x,y),1) )
		{
			gnugo_play_move(POS(x,y),1);
			genmove();
		}
		else
		{
			cout << "Wrong move. Try again!" << endl;
		}

		//gnugo_play_move(1,1,1);
	}


	//gnugo_add_stone(0,0,1);
	//set_random_seed(12345);
	return 0;
}

