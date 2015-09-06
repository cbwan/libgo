// ConsoleGo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "libgo.h"
#include "libigs.h"

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

void play_random()
{
	static int m=1;
	cb_log( "----- my turn ------" );
	cb_log( "Me playing randomly:" );
	cb_igs_play(m,m);
	m++;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//cb_init_gnugo();

	cb_connect_igs();

	// Starting FSM
	while(1)
	{
		cb_igs_wait_event();

		int event = cb_igs_get_event();
		do
		{
			switch( event )
			{
			case IGS_EVENT_CONNECTED: cb_igs_login("cbtwo","pandanet"); break;
			case IGS_EVENT_LOGGED_IN: cb_igs_challenge( "cbone", "B" ); break;
			case IGS_EVENT_CHALLENGE_DECLINED: cout << "SNIIIFFFF match declined" << endl; break;
			case IGS_EVENT_CHALLENGE_ACCEPTED:
				{
					cout << "Challenge accepted!" << endl;
					cb_igs_say("Thanks! I am playing from an experimental virtual reality application!");
					play_random();
					break;
				}
			case IGS_EVENT_MOVE:
				{
					if( string(cb_igs_get_last_move_stone()) != "B" )
					{
						int x = cb_igs_get_last_move_x();
						int y = cb_igs_get_last_move_y();

						cout << "Opponent played: " << x << ":" << y << endl;
						play_random();
					}
				}
			}

			event = cb_igs_get_event();
		} while( event != IGS_EVENT_NO_EVENT );
	}

	cb_disconnect_igs();
	return 0;
}

