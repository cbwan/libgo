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

int _tmain(int argc, _TCHAR* argv[])
{
	//cb_init_gnugo();

	cb_connect_igs();

	if( cb_igs_wait_event() != EVENT_LOGIN )
	{
		cout << "Error waiting for login, got something else!" << endl;
		return -1;
	}

	cb_igs_login("cbtwo","pandanet");

	if( cb_igs_wait_event() != EVENT_LOGGED_IN )
	{
		cout << "Error waiting for logged, got something else!" << endl;
		return -1;
	}

	if( cb_igs_get_status() )
	{
		cout << "Logged in!" << endl;
	}

	cb_igs_challenge( "cbone", "B" );

	int event = cb_igs_wait_event();

	if( event == EVENT_GAME_DECLINED )
	{
		cout << "SNIIIFFFF match declined" << endl;
	}
	else if( event == EVENT_GAME_ACCEPTED )
	{
		cout << "Challenge accepted!" << endl;
		cb_igs_say("Thanks! I am playing from an experimental virtual reality application!");
	}

	int m=1;

	while(1)
	{
		ostringstream str;
		char a = 'A' + m - 1;

		str << a << m;
		m++;

		//cout << "playing: " << str.str() << endl;
		cb_igs_play(str.str());
		
		//cout << "waiting opponent." << endl;
		bool nextMove=false;
		while( !nextMove )
		{
			if( cb_igs_wait_event() == EVENT_MOVE )
			{
				if( cb_igs_get_last_move_stone() != "B" )
				{
					nextMove = true;
					cout << "!! This was opponent's move !!" << endl;
				}
			}
		}
	}

	cb_disconnect_igs();
	return 0;
}

