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

// Fuego enum
enum StoneEnum {
	BLACK=0,
	WHITE=1,
	EMPTY=2
};

// Finite state machine status
int g_Status=IGS_STATUS_UNKNOWN;
queue<int> g_Events;
bool g_IsIGSConnected = false;
bool g_LocalPassed = false;

struct Move {
	int index;
	int stone;
	int x;
	int y;
	string captured;
};

Move g_LastMove;


struct Challenge {
	string user;
};

Challenge g_LastChallenge;

// Network
using boost::asio::ip::tcp;
boost::asio::io_service*      g_IOService;
boost::asio::ip::tcp::socket* g_Socket;

// Thread
boost::thread* g_Thread = 0;
boost::mutex eventsMutex;

// Log
ofstream* g_Log = 0;

bool lineMatches( string iLine, string iRegex );
bool extractMove( string iLine );
bool extractChallenge( string iLine );
void igs_loop();

//const string g_MoveRE = "  (\\d)*\\((B|W)\\):( \\w\\d)+";

//  9(W): A1
// 10(B): F6
// 25(W): A2 B2 C3 // other stones are captures

const string g_MoveRE = " *(\\d+)\\((B|W)\\): (\\w)(\\d+) ?(.*)";

// Challenge:
// Match[19x19] in 10 minutes requested with cbone as White.

//const string g_ChallengeRE = "Match\\[(\\d+).*\\] in (\\d+) minutes requested with (\\w*) as \\((White|Black)\\).";
const string g_ChallengeRE = "Match.*minutes requested with (\\w*) as .*";

void Log( string iLog )
{
	cout << iLog << endl;

	if( g_Log == 0 )
	{
		g_Log = new ofstream( "c:\\temp\\go.txt" );
	}

	if( g_Log != 0 ) (*g_Log) << iLog << endl;
}

LIBIGS_API void cbgo_log( char* iLog )
{
	Log( string( iLog ) );
}


void init()
{
	

	//extractChallenge("Match[19x19] in 10 minutes requested with cbone as White.");

	//g_Events = queue<Event>();
	// test
	//lineMatches("  0(B): A1","  (\\d)\\((B|W)\\): (\\w)(\\d)");
	//extractMove("  10(B): A1 A2 A3");
	//lineMatches("  10(B): F6", g_MoveRE);
	//extractMove("  10(B): F6");
	/*
	extractMove("  0(B): A11");

	ostringstream str;
	str << "Last Move : " << g_LastMove.stone << ":" << g_LastMove.x << ":" << g_LastMove.y;
	Log( str.str() );

	g_LastMove.index = -1;
	*/
}

int convert_stone( string iStone )
{
	if( iStone == "B" ) return BLACK;
	if( iStone == "W" ) return WHITE;
	return EMPTY;
}

string convert_stone( int iStone )
{
	if( iStone == BLACK ) return "B";
	if( iStone == WHITE ) return "W";
	return "X";
}

int convert_letter_to_number( string iLetter )
{
	if( iLetter == "A" ) return 1;
	if( iLetter == "B" ) return 2;
	if( iLetter == "C" ) return 3;
	if( iLetter == "D" ) return 4;
	if( iLetter == "E" ) return 5;
	if( iLetter == "F" ) return 6;
	if( iLetter == "G" ) return 7;
	if( iLetter == "H" ) return 8;
	if( iLetter == "J" ) return 9;
	if( iLetter == "K" ) return 10;
	if( iLetter == "L" ) return 11;
	if( iLetter == "M" ) return 12;
	if( iLetter == "N" ) return 13;
	if( iLetter == "O" ) return 14;
	if( iLetter == "P" ) return 15;
	if( iLetter == "Q" ) return 16;
	if( iLetter == "R" ) return 17;
	if( iLetter == "S" ) return 18;
	if( iLetter == "T" ) return 19;
	return -1;
}

string convert_number_to_letter( int iNumber )
{
	if( iNumber == 1 ) return "A";
	if( iNumber == 2 ) return "B";
	if( iNumber == 3 ) return "C";
	if( iNumber == 4 ) return "D";
	if( iNumber == 5 ) return "E";
	if( iNumber == 6 ) return "F";
	if( iNumber == 7 ) return "G";
	if( iNumber == 8 ) return "H";
	if( iNumber == 9 ) return "J";
	if( iNumber == 10 ) return "K";
	if( iNumber == 11 ) return "L";
	if( iNumber == 12 ) return "M";
	if( iNumber == 13 ) return "N";
	if( iNumber == 14 ) return "O";
	if( iNumber == 15 ) return "P";
	if( iNumber == 16 ) return "Q";
	if( iNumber == 17 ) return "R";
	if( iNumber == 18 ) return "S";
	if( iNumber == 19 ) return "T";
	return "Z";
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
		Log( re.str() + " matches " + iLine );
		size_t nbMatches = matches.size();

		for ( unsigned int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			ostringstream str; str << "\tmatches[" << i << "] = '" << match << "'" << endl;
			Log( str.str() );
		}

		istringstream indexStr(matches[1]); indexStr >> g_LastMove.index;
		g_LastMove.stone = convert_stone( matches[2] );
		g_LastMove.x     = convert_letter_to_number( matches[3] );
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

LIBIGS_API void cbgo_igs_accept_challenge()
{
	string acceptLine = "accept " + g_LastChallenge.user + " B 19 10 10";
	Log("cbgo_igs_accept_challenge: " + acceptLine);
	cbgo_igs_writeline( acceptLine );
}

LIBIGS_API void cbgo_igs_decline_challenge()
{
	string decLine = "decline " + g_LastChallenge.user;
	Log("cbgo_igs_decline_challenge: " + decLine );
	cbgo_igs_writeline( decLine );
}

LIBIGS_API char* cbgo_igs_get_challenger()
{
	Log("cbgo_igs_get_challenger");
	return (char*)g_LastChallenge.user.c_str();
}

bool extractChallenge( string iLine )
{
	boost::regex re;
	try
	{
		// Set up the regular expression for case-insensitivity
		re.assign(g_ChallengeRE, boost::regex_constants::icase);
	}
	catch (boost::regex_error& e)
	{
		Log( g_ChallengeRE + " is not a valid regular expression: \""
			+ e.what() + "\"" );
		return false;
	}

	boost::cmatch matches;
	if (boost::regex_match(iLine.c_str(), matches, re))
	{
		Log( re.str() + " matches " + iLine );
		size_t nbMatches = matches.size();

		for ( unsigned int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			ostringstream str; str << "\tmatches[" << i << "] = '" << match << "'" << endl;
			Log( str.str() );
		}

		g_LastChallenge.user = matches[1];
		Log("challenge from user: " + g_LastChallenge.user );

		/*
		istringstream indexStr(matches[1]); indexStr >> g_LastMove.index;
		g_LastMove.stone = convert_stone( matches[2] );
		g_LastMove.x     = convert_letter_to_number( matches[3] );
		istringstream nbStr(matches[4]); nbStr >> g_LastMove.y;
		g_LastMove.captured = matches[5];

		if( g_LastMove.captured != "" )
		{
			Log( "CAPTURES!! " + g_LastMove.captured );
		}*/

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

		
		for (int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			cout << "\tmatches[" << i << "] = " << match << endl;
		}

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

LIBIGS_API int cbgo_igs_get_events_nb(void)
{
	return (int)g_Events.size();
}

LIBIGS_API int cbgo_igs_get_event()
{
	boost::lock_guard<boost::mutex> lock(eventsMutex);

	int event = IGS_EVENT_NO_EVENT;
	if( !g_Events.empty() )
	{
		event = g_Events.front();
		g_Events.pop();
	}
	return event;
}

void cbgo_igs_push_event( int iEvent )
{
	boost::lock_guard<boost::mutex> lock(eventsMutex);

	g_Events.push( iEvent );

	ostringstream str; str << "> Event: ";

	switch( iEvent )
	{
	case IGS_EVENT_UNKNOWN:           str << "Unknown"; break;
	case IGS_EVENT_CONNECTED:         str << "Connected"; break;
	case IGS_EVENT_LOGIN_PROMPT:      str << "Login prompt"; break;
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

void cbgo_igs_set_status( int iStatus )
{
	g_Status = iStatus;

	ostringstream str; str << "> Status: ";

	switch( g_Status )
	{
	case IGS_STATUS_UNKNOWN:              str << "Unknown"; break;
	case IGS_STATUS_DISCONNECTED:         str << "Disconnected"; break;
	case IGS_STATUS_WAITING_LOGIN_PROMPT: str << "Waiting Login prompt"; break;
	case IGS_STATUS_AT_LOGIN_PROMPT:      str << "At Login prompt"; break;
	case IGS_STATUS_LOBBY:                str << "Lobby"; break;
	case IGS_STATUS_WAITING_CHALLENGE_ANSWER:str << "Waiting challenge answer"; break;
	case IGS_STATUS_IN_GAME: str << "In Game"; break;
	default: str << "Worse than unknown: " << g_Status; break;
	}

	Log( str.str() );
}

LIBIGS_API bool  cbgo_igs_is_connected(void)
{
	return g_IsIGSConnected;
}

LIBIGS_API bool cbgo_connect_igs()
{
	init();

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

		g_IsIGSConnected = true;
		cbgo_igs_push_event(IGS_EVENT_CONNECTED);
		cbgo_igs_set_status(IGS_STATUS_WAITING_LOGIN_PROMPT);

		g_Thread = new boost::thread(&igs_loop);
	}

	return true;
}

LIBIGS_API void cbgo_disconnect_igs()
{
	if( g_IsIGSConnected )
	{
		cbgo_igs_writeline("quit");
		cbgo_igs_set_status( IGS_STATUS_DISCONNECTED );

		Log("Waiting for thread to finish...");
		g_Thread->join();
		Log("Disconnected");

		delete g_Socket;
		g_Socket=0;
	}
	else
	{
		Log("Can't disconnect because IGS is not connected.");
	}
}

LIBIGS_API string cbgo_igs_readline()
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
			return "ERR";
		}
	}

	Log( "@>" + line );

	if( line != "1 5" && line != "" && lineContains( line, "connected") )
	{
		Log( line );
	}

	// Save to file

	return line;
}

LIBIGS_API bool cbgo_igs_writeline( std::string iLine )
{
	if( !g_IsIGSConnected )
		return false;
	
	Log( ">>> '" + iLine + "'" );

	string line = iLine + '\r' + '\n';
	try
	{
		boost::asio::write( *g_Socket, boost::asio::buffer( line, line.size() ) );
	}
	catch( ... )
	{
		Log( "Exception while writing to network. Server closed connection ?" );
		return false;
	}
	return true;
}

LIBIGS_API bool cbgo_igs_read_event()
{
	string line = cbgo_igs_readline();

	if( line == "ERR" )
	{
		return false;
	}

	if( line == "Login: " )
	{
		cbgo_igs_set_status(IGS_STATUS_AT_LOGIN_PROMPT);
		cbgo_igs_push_event(IGS_EVENT_LOGIN_PROMPT);
	}
	else if( lineContains( line, "IGS entry") )
	{
		cbgo_igs_set_status( IGS_STATUS_LOBBY );
		cbgo_igs_push_event(IGS_EVENT_LOGGED_IN);
	}
	else if( lineContains( line, "accepted") )
	{
		cbgo_igs_set_status( IGS_STATUS_IN_GAME );
		cbgo_igs_push_event(IGS_EVENT_CHALLENGE_ACCEPTED);
	}
	else if( lineContains( line, "declined") )
	{
		cbgo_igs_set_status( IGS_STATUS_LOBBY );
		cbgo_igs_push_event(IGS_EVENT_CHALLENGE_DECLINED);
	}
	//Match[19x19] in 10 minutes requested with cbone as White.
	else if( lineMatches( line, g_ChallengeRE))
	{
		extractChallenge(line);
		cbgo_igs_push_event(IGS_EVENT_RECEIVED_CHALLENGE);
	}

	/////// IN GAME
	else if( g_Status == IGS_STATUS_IN_GAME )
	{
		if( lineMatches( line, g_MoveRE) )
		{
			if( extractMove(line) )
			{
				ostringstream str;
				str << "Last Move : " << g_LastMove.stone << ": " << g_LastMove.x << ":" << g_LastMove.y;
				Log( str.str() );
				cbgo_igs_push_event(IGS_EVENT_MOVE);
			}
		}
		else if( lineMatches( line, ".*#> Game.*") )
		{
			// ignore
		}
		else if( lineContains( line, "Please use say"))
		{

		}
		else if( lineContains( line, ": Pass"))
		{
			//XXX revise regex for pass
			if( g_LocalPassed )
			{
				Log( "Received pass line, but we just passed, so ignoring.");
				g_LocalPassed = false;
			}
			else
			{
				cbgo_igs_push_event(IGS_EVENT_PASS);
			}
		}
		else if( lineContains( line, "resigned"))
		{
			cbgo_igs_push_event(IGS_EVENT_RESIGN);
		}
		else
		{
			Log( "Unknown line :'" + line + "'");
		}
	}

	return true;
}

void igs_loop()
{
	bool exit = false;

	while( cbgo_igs_get_status() != IGS_STATUS_DISCONNECTED )
	{
		if( !cbgo_igs_read_event() )
		{
			exit = true;
		}
	}
}

LIBIGS_API int cbgo_igs_wait_event()
{
	//Log(">WAIT");
	int event = IGS_EVENT_UNKNOWN;
	bool exit = false;

	while( g_Events.front() == IGS_EVENT_UNKNOWN && !exit )
	{
		if( cbgo_igs_read_event() )
		{
		}
		else
		{
			exit = true;
		}
	}

	//Log("<WAIT");

	return event;
}

LIBIGS_API bool  cbgo_igs_login(char* iLogin, char* iPwd)
{
	cbgo_igs_writeline(iLogin);
	cbgo_igs_writeline(iPwd);

	return true;
}

/*

match <opponentname> [color] [board size] [time] [byoyomi minutes]
    'match' is for starting a game with an opponent. You can offer or decline
  a match request. Start a game with 'match', followed by the opponent's name,
  color you wish ( W or B ), board size, time (measured in minutes) for each
  player, and byoyomi minutes per player.  Example:   match ivy W 19 15 10
  If no boundaries are given, the default settings are:  board size = 19
  color = B, time = 90 minutes per player, byoyomi = 10 minutes per player.
      Example:   match ivy   (This is the same as:  match ivy B 19 90 10)
      Note:  match ivy B 19 0 0    would mean there are no time limits.
  The first move by B (Black) can be:  handicap #    (#) is the number of
  the handicap stones.  To place moves on the board, see:   help coords
   a) A game can be 'adjourned' if both players enter:   adjourn
   b) An 'adjourned' can be restarted with the 'load' command. See: help load
** MUST READ **   To 'score' or end a game, see:   help score
   ^^^^^^^^^
    IGS supports multiple games. You can play more than one game, but if
  you want to play only one game and not accept additional 'match' requests,
  there are 2 options.   (See:   help toggle)
    1)  While playing a game, type:   toggle singlegame  (toggles off or on)
    2)  While playing a game, type:   toggle open

See also:  addtime adjourn automatch byoyomi client coords decline defs free
  games komi load open refresh resign save say score stored team undo verbose

  */

LIBIGS_API bool  cbgo_igs_challenge(char* iUser, int iMyColor, int iBoardSize, int iMainTime, int iByoyomiTime )
{
	ostringstream str;
	str << "match " << iUser;

	if( iMyColor == BLACK )
	{
		str << " b ";
	}
	else if( iMyColor == WHITE )
	{
		str << " w ";
	}

	str << iBoardSize << " " << iMainTime << " " << iByoyomiTime;

	if( cbgo_igs_writeline( str.str() ) )
	{
		g_Status = IGS_STATUS_WAITING_CHALLENGE_ANSWER;
		return true;
	}
	else return false;
}

LIBIGS_API bool  cbgo_igs_say(char* iMsg)
{
	cbgo_igs_writeline("say " + string(iMsg) );

	return true;
}

LIBIGS_API bool  cbgo_igs_play(int x, int y)
{
	string xs = convert_number_to_letter(x);
	ostringstream move; move << xs << y;

	if( g_Status == IGS_STATUS_IN_GAME )
	{
		cbgo_igs_writeline( move.str() );
	}
	else
	{
		Log( "CAN'T PLAY NOT IN GAME!" );
	}

	return true;
}

LIBIGS_API bool  cbgo_igs_pass()
{
	if( g_Status == IGS_STATUS_IN_GAME )
	{
		cbgo_igs_writeline( "pass" );
		g_LocalPassed = true;
	}
	else
	{
		Log( "CAN'T PASS NOT IN GAME!" );
	}

	return true;
}

LIBIGS_API bool  cbgo_igs_resign()
{
	if( g_Status == IGS_STATUS_IN_GAME )
	{
		cbgo_igs_writeline( "resign" );
	}
	else
	{
		Log( "CAN'T RESIGN NOT IN GAME!" );
	}

	return true;
}

LIBIGS_API int cbgo_igs_get_status()
{
	return g_Status;
}

LIBIGS_API int cbgo_igs_get_last_move_index()
{
	return g_LastMove.index;
}

LIBIGS_API int cbgo_igs_get_last_move_stone()
{
	
	return g_LastMove.stone;
}

LIBIGS_API int cbgo_igs_get_last_move_x()
{
	return g_LastMove.x;
}

LIBIGS_API int cbgo_igs_get_last_move_y()
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