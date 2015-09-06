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
		//cb_igs_wait_event();
		Sleep(100);

		int event = cb_igs_get_event();
		if( event != IGS_EVENT_NO_EVENT )
		{
			cout << cb_igs_get_events_nb()  << " >>> 1-Event:" << event << endl;
		}
		do
		{
			switch( event )
			{
			case IGS_EVENT_LOGIN_PROMPT: cb_igs_login("cbtwo","pandanet"); break;
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

					if( cb_igs_get_last_move_stone() != BLACK )
					{
						int x = cb_igs_get_last_move_x();
						int y = cb_igs_get_last_move_y();

						cout << "Opponent played: " << x << ":" << y << endl;
						play_random();
						cb_disconnect_igs();
						exit(0);
					}
					break;
				}
				default: break;
			}

			event = cb_igs_get_event();
			if( event != IGS_EVENT_NO_EVENT )
			{
				cout << cb_igs_get_events_nb() << " >>> 2-Event:" << event << endl;
			}
		} while( event != IGS_EVENT_NO_EVENT );
	}

	cb_disconnect_igs();
	return 0;
}

