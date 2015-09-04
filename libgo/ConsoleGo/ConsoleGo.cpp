// ConsoleGo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "libgo.h"
#include "libigs.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//cb_init_gnugo();

	cb_connect_igs();

	SleepEx(3000,false);

	while(1)
	{
		cb_read_igs();
	}

	cb_disconnect_igs();
	return 0;
}

