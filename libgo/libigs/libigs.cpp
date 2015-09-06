// libigs.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libigs.h"

#include <fstream>
#include <iostream>
#include <queue>

#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/thread/thread.hpp>

using namespace std;

// Finite state machine status
int g_Status=IGS_STATUS_UNKNOWN;
queue<int> g_Events;

struct Move {
	int    index;
	string stone;
	string x;
	int    y;
	string captured;
};

Move g_LastMove;

// Network
using boost::asio::ip::tcp;
boost::asio::io_service*      g_IOService;
boost::asio::ip::tcp::socket* g_Socket;

// Log
ofstream* g_Log = 0;

//const string g_MoveRE = "  (\\d)*\\((B|W)\\):( \\w\\d)+";

//  9(W): A1
// 10(B): F6
// 25(W): A2 B2 C3 // other stones are captures

const string g_MoveRE = " *(\\d+)\\((B|W)\\): (\\w)(\\d) ?(.*)";

void Log( string iLog )
{
	cout << iLog << endl;
	(*g_Log) << iLog << endl;
}

LIBIGS_API void cb_log( char* iLog )
{
	Log( string( iLog ) );
}

bool extractMove( string iLine )
{
	boost::regex re;
	try
	{
		// Set up the regular expression for case-insensitivity
		re.assign(g_MoveRE, boost::regex_constants::icase);
	}
	catch (boost::regex_error& e)
	{
		Log( g_MoveRE + " is not a valid regular expression: \""
			+ e.what() + "\"" );
		return false;
	}

	boost::cmatch matches;
	if (boost::regex_match(iLine.c_str(), matches, re))
	{
		//Log( re.str() + " matches " + iLine );
		int nbMatches = matches.size();

		for ( unsigned int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			ostringstream str; str << "\tmatches[" << i << "] = '" << match << "'" << endl;
			//Log( str.str() );
		}

		istringstream indexStr(matches[1]); indexStr >> g_LastMove.index;
		g_LastMove.stone = matches[2];
		g_LastMove.x = matches[3];
		istringstream nbStr(matches[4]); nbStr >> g_LastMove.y;
		g_LastMove.captured = matches[5];

		if( g_LastMove.captured != "" )
		{
			Log( "CAPTURES!! " + g_LastMove.captured );
		}

		return true;
	}

	return false;
}

bool lineMatches( string iLine, string iRegex )
{
	boost::regex re;
	try
	{
		// Set up the regular expression for case-insensitivity
		re.assign(iRegex, boost::regex_constants::icase);
	}
	catch (boost::regex_error& e)
	{
		Log( iRegex + " is not a valid regular expression: \""
			+ e.what() + "\"" );
		return false;
	}

	boost::cmatch matches;
	if (boost::regex_match(iLine.c_str(), matches, re))
	{
		cout << re << " matches " << iLine << endl;

		/*
		for (int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			cout << "\tmatches[" << i << "] = " << match << endl;
		}*/

		return true;
	}

	return false;
}

bool lineContains(string iLine, string iPattern)
{
	if( iLine.find( iPattern ) != string::npos )
	{
		return true;
	}
	else
	{
		return false;
	}
}

LIBIGS_API int cb_igs_get_events_nb(void)
{
	return g_Events.size();
}

LIBIGS_API int cb_igs_get_event()
{
	int event = IGS_EVENT_NO_EVENT;
	if( !g_Events.empty() )
	{
		event = g_Events.front();
		g_Events.pop();
	}
	return event;
}

void cb_igs_push_event( int iEvent )
{
	g_Events.push( iEvent );

	ostringstream str; str << "> Event: ";

	switch( iEvent )
	{
	case IGS_EVENT_UNKNOWN:           str << "Unknown"; break;
	case IGS_EVENT_CONNECTED:         str << "Connected"; break;
	case IGS_EVENT_LOGGED_IN:         str << "Logged in"; break;
	case IGS_EVENT_LOGIN_FAILED:      str << "Login failed"; break;
	case IGS_EVENT_RECEIVED_CHALLENGE:str << "Received challenge"; break;
	case IGS_EVENT_CHALLENGE_ACCEPTED:str << "Challenge accepted"; break;
	case IGS_EVENT_CHALLENGE_DECLINED:str << "Challenge declined"; break;
	case IGS_EVENT_MOVE:              str << "Move"; break;
	case IGS_EVENT_PASS:              str << "Pass"; break;
	case IGS_EVENT_RESIGN:            str << "Resign"; break;
	case IGS_EVENT_WIN:               str << "Win"; break;
	default: str << "Worse than unknown: " << iEvent; break;
	}

	Log( str.str() );
}

void cb_igs_set_status( int iStatus )
{
	g_Status = iStatus;

	ostringstream str; str << "> Status: ";

	switch( g_Status )
	{
	case IGS_STATUS_UNKNOWN:           str << "Unknown"; break;
	case IGS_STATUS_DISCONNECTED:      str << "Disconnected"; break;
	case IGS_STATUS_WAITING_LOGIN:     str << "Waiting Login"; break;
	case IGS_STATUS_LOBBY:             str << "Lobby"; break;
	case IGS_STATUS_WAITING_CHALLENGE_ANSWER:str << "Waiting challenge answer"; break;
	case IGS_STATUS_IN_GAME: str << "In Game"; break;
	default: str << "Worse than unknown: " << g_Status; break;
	}

	Log( str.str() );
}

LIBIGS_API bool cb_connect_igs()
{
	g_Log = new ofstream( "c:\\temp\\go.txt" );

	//g_Events = queue<Event>();

	// test
	//lineMatches("  0(B): A1","  (\\d)\\((B|W)\\): (\\w)(\\d)");
	//extractMove("  10(B): A1 A2 A3");
	lineMatches("  10(B): F6", g_MoveRE);
	extractMove("  10(B): F6");
	g_LastMove.index = -1;

	g_Status = IGS_STATUS_DISCONNECTED;

	boost::asio::io_service* const service = new boost::asio::io_service;
	tcp::resolver resolver( *service );

	tcp::resolver::iterator endpoint_iterator;

	try
	{
		Log( "Looking for address." );

		const string addr("igs.joyjoy.net");

		const tcp::resolver::query query(
			addr.c_str(),
			"6969",
			boost::asio::ip::resolver_query_base::numeric_service );

		endpoint_iterator = resolver.resolve( query );
	}
	catch( ... )
	{
		Log( "Failed to connect to server." );
		return 0;
	}

	tcp::resolver::iterator end;

	Log( "Creating socket." );
	g_Socket = new tcp::socket( *service );

	g_Socket->open( boost::asio::ip::tcp::v4() );

	const boost::asio::ip::tcp::no_delay option( true );
	boost::system::error_code ec;
	g_Socket->set_option( option, ec );
	Log( "TCP::No_Delay status: " + ec.value() );

	tcp::endpoint currentEndPoint = *endpoint_iterator;

	while( currentEndPoint.protocol().family() != 2 && endpoint_iterator != end )
	{
		ostringstream str;
		str << "Testing ... to : " << (*endpoint_iterator).host_name()
			<< " ( " << currentEndPoint.address().to_string()
			<< " : " << currentEndPoint .port() << ", "
			<< currentEndPoint .protocol().family() << " ) "
			<< endl;
		Log( str.str() );

		++endpoint_iterator;
		currentEndPoint = *endpoint_iterator;
	}

	boost::system::error_code error = boost::asio::error::host_not_found;
	//while (error && endpoint_iterator != end)
	for( int i = 0; i < 10; i++ )
	{
		//g_Socket->close();
		g_Socket->connect( *endpoint_iterator/*++*/, error );

		if( !error )
		{
			break;
		}
		else
		{
			ostringstream str; str << "Attempt " << ( i + 1 ) << ": Can't connect to server." << endl;
			Log( str.str() );
			Sleep( 3000 );
			//throw boost::system::system_error( error );
			//return false;
		}
	}

	if( error )
	{
		Log( "Definitely can't connect to server" );
		//throw boost::system::system_error(error);
		return 0;
	}
	else
	{
		Log( "Successfully connected to server." );
			
		boost::asio::socket_base::keep_alive keepAlive( true );
		g_Socket->set_option( keepAlive );

		//cb_igs_push_event(IGS_EVENT_CONNECTED);
		//cb_igs_set_status(IGS_STATUS_WAITING_LOGIN);
	}

	return true;
}

LIBIGS_API void cb_disconnect_igs()
{
	cb_igs_writeline("quit");
	delete g_Socket;
	g_Socket=0;
	cb_igs_set_status( IGS_STATUS_DISCONNECTED );
}

LIBIGS_API string cb_igs_readline()
{
	boost::system::error_code error;

	string line;
	char chr;
	bool lineOk = false;

	while( !lineOk )
	{
		try
		{
			size_t len = boost::asio::read( *g_Socket, boost::asio::buffer( &chr, 1 ));
			if( chr != '\r' && chr != '\n' ) line += chr;
			if( chr == '\n' ) lineOk=true;

			if( line == "Login: " ) lineOk=true;
		}

		//VRLOGD2 << VR_INFO << "Waited : " << ( VRTIME_MS - startTime ) << " ms." << endl;
		catch( ... )
		{
			Log( "Exception while reading form network. Server closed connection ?" );
		}
	}

	// XXX uncomment here to see all received lines
	//(*g_Log) << line << endl;

	if( line != "1 5" && line != "" && lineContains( line, "connected") )
	{
		Log( line );
	}

	// Save to file

	return line;
}

LIBIGS_API bool cb_igs_writeline( std::string iLine )
{
	Log( ">>> '" + iLine + "'" );

	string line = iLine + '\r' + '\n';
	try
	{
		boost::asio::write( *g_Socket, boost::asio::buffer( line, line.size() ) );
	}
	catch( ... )
	{
		Log( "Exception while writing to network. Server closed connection ?" );
	}
	return true;
}

LIBIGS_API int cb_igs_read_event()
{
	string line = cb_igs_readline();

	if( line == "Login: " )
	{
		cb_igs_push_event(IGS_EVENT_CONNECTED);
		cb_igs_set_status(IGS_STATUS_WAITING_LOGIN);
		return IGS_EVENT_CONNECTED;
	}
	else if( lineContains( line, "IGS entry") )
	{
		cb_igs_push_event(IGS_EVENT_LOGGED_IN);
		cb_igs_set_status( IGS_STATUS_LOBBY );
		return IGS_EVENT_LOGGED_IN;
	}
	else if( lineContains( line, "accepted") )
	{
		cb_igs_push_event(IGS_EVENT_CHALLENGE_ACCEPTED);
		cb_igs_set_status( IGS_STATUS_IN_GAME );
		return IGS_EVENT_CHALLENGE_ACCEPTED;
	}
	else if( lineContains( line, "declined") )
	{
		cb_igs_push_event(IGS_EVENT_CHALLENGE_DECLINED);
		cb_igs_set_status( IGS_STATUS_LOBBY );
		return IGS_EVENT_CHALLENGE_DECLINED;
	}

	/////// IN GAME
	else if( g_Status == IGS_STATUS_IN_GAME )
	{
		if( lineMatches( line, g_MoveRE) )
		{
			if( extractMove(line) )
			{
				ostringstream str;
				str << "Last Move : " << g_LastMove.stone << ":" << g_LastMove.x << g_LastMove.y;
				Log( str.str() );
				cb_igs_push_event(IGS_EVENT_MOVE);
				return IGS_EVENT_MOVE;
			}
		}
		else if( lineMatches( line, ".*#> Game.*") )
		{
			// ignore
		}
		else if( lineContains( line, "Please use say"))
		{

		}
		else
		{
			Log( "Unknown line: " + line );
		}
	}

	return IGS_EVENT_UNKNOWN;
}

LIBIGS_API int cb_igs_wait_event()
{
	int event = IGS_EVENT_UNKNOWN;
	while( event == IGS_EVENT_UNKNOWN )
	{
		event = cb_igs_read_event();
	}

	return event;
}

LIBIGS_API bool  cb_igs_login(char* iLogin, char* iPwd)
{
	cb_igs_writeline(iLogin);
	cb_igs_writeline(iPwd);

	return true;
}

LIBIGS_API bool  cb_igs_challenge(char* iUser, char* iMyColor)
{
	cb_igs_writeline("match " + string(iUser) + " " + string(iMyColor) + " 19 15 10");
	g_Status = IGS_STATUS_WAITING_CHALLENGE_ANSWER;

	return true;
}

LIBIGS_API bool  cb_igs_say(char* iMsg)
{
	cb_igs_writeline("say " + string(iMsg) );

	return true;
}

LIBIGS_API bool  cb_igs_play(char* iMove)
{
	if( g_Status == IGS_STATUS_IN_GAME )
	{
		cb_igs_writeline( iMove );
	}
	else
	{
		Log( "NOT IN GAME!" );
	}

	return true;
}

LIBIGS_API int cb_igs_get_status()
{
	return g_Status;
}

LIBIGS_API int cb_igs_get_last_move_index()
{
	return g_LastMove.index;
}

LIBIGS_API char* cb_igs_get_last_move_stone()
{
	return (char*)g_LastMove.stone.c_str();
}

LIBIGS_API char* cb_igs_get_last_move_x()
{
	return (char*)g_LastMove.x.c_str();
}

LIBIGS_API int cb_igs_get_last_move_y()
{
	return g_LastMove.y;
}


/*
// This is the constructor of a class that has been exported.
// see libigs.h for the class definition
Clibigs::Clibigs()
{
	return;
}
*/