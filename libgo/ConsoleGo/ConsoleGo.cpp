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
	cbgo_log( "----- my turn ------" );
	cbgo_log( "Me playing randomly:" );
	cbgo_igs_play(m,m);
	m++;
}

void print_time()
{
	float black_time = cbgo_get_time_left(BLACK);
	float white_time = cbgo_get_time_left(WHITE);

	int black_nb = cbgo_get_moves_left(BLACK);
	int white_nb = cbgo_get_moves_left(WHITE);

	cout << "Black: " << black_time << " ( " << black_nb << " moves left." << endl;
	cout << "White: " << white_time << " ( " << white_nb << " moves left." << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cbgo_init();

	bool legal = cbgo_is_legal(0,0,0);

	/*
	cbgo_set_main_time(10);
	cbgo_set_overtime(10);
	cbgo_set_overtime_moves(4);
	cbgo_start_timer();

	while(1)
	{
		print_time();
		Sleep(1000);
	}*/

	cbgo_connect_igs();

	// Starting FSM
	while(1)
	{
		//cbgo_igs_wait_event();
		Sleep(100);

		int event = cbgo_igs_get_event();
		if( event != IGS_EVENT_NO_EVENT )
		{
			cout << cbgo_igs_get_events_nb()  << " >>> 1-Event:" << event << endl;
		}
		do
		{
			switch( event )
			{
			case IGS_EVENT_LOGIN_PROMPT: cbgo_igs_login("cbtwo","pandanet"); break;
			case IGS_EVENT_LOGGED_IN: cbgo_igs_challenge( "cbone", BLACK, 19, 10, 5 ); break;
			case IGS_EVENT_RECEIVED_CHALLENGE: cout << cbgo_igs_get_challenger() << endl; break;
			case IGS_EVENT_CHALLENGE_DECLINED: cout << "SNIIIFFFF match declined" << endl; break;
			case IGS_EVENT_CHALLENGE_ACCEPTED:
				{
					cout << "Challenge accepted!" << endl;
					cbgo_igs_say("Thanks! I am playing from an experimental virtual reality application!");
					play_random();
					break;
				}
			case IGS_EVENT_MOVE:
				{

					if( cbgo_igs_get_last_move_stone() != BLACK )
					{
						int x = cbgo_igs_get_last_move_x();
						int y = cbgo_igs_get_last_move_y();

						cout << "Opponent played: " << x << ":" << y << endl;
						play_random();
						cbgo_disconnect_igs();
						exit(0);
					}
					break;
				}
			case IGS_EVENT_CHAT_MSG:
				{
					string chat = cbgo_igs_get_chat_msg();
					cout << "Player said: " + chat << endl;
					break;
				}
				default: break;
			}

			event = cbgo_igs_get_event();
			if( event != IGS_EVENT_NO_EVENT )
			{
				cout << cbgo_igs_get_events_nb() << " >>> 2-Event:" << event << endl;
			}
		} while( event != IGS_EVENT_NO_EVENT );
	}

	cbgo_disconnect_igs();
	return 0;
}
