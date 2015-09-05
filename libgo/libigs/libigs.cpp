// libigs.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libigs.h"

#include <fstream>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/regex.hpp>

using namespace std;

int m_Status=IGS_STATUS_UNKNOWN;

boost::asio::io_service*      m_IOService;
boost::asio::ip::tcp::socket* m_Socket;
ofstream* g_log = 0;

using boost::asio::ip::tcp;

struct Move {
	int    index;
	string stone;
	string x;
	int    y;
	string captured;
};

Move g_lastMove;

//const string g_moveRE = "  (\\d)*\\((B|W)\\):( \\w\\d)+";

const string g_moveRE = "  (\\d)*\\((B|W)\\): (\\w)(\\d) ?(.*)";

bool extractMove( string iLine )
{
	boost::regex re;
	try
	{
		// Set up the regular expression for case-insensitivity
		re.assign(g_moveRE, boost::regex_constants::icase);
	}
	catch (boost::regex_error& e)
	{
		cout << g_moveRE << " is not a valid regular expression: \""
			<< e.what() << "\"" << endl;
		return false;
	}

	boost::cmatch matches;
	if (boost::regex_match(iLine.c_str(), matches, re))
	{
		cout << re << " matches " << iLine << endl;
		int nbMatches = matches.size();

		for ( unsigned int i = 1; i < matches.size(); i++)
		{
			// sub_match::first and sub_match::second are iterators that
			// refer to the first and one past the last chars of the
			// matching subexpression
			string match(matches[i].first, matches[i].second);
			cout << "\tmatches[" << i << "] = '" << match << "'" << endl;
		}

		istringstream indexStr(matches[1]); indexStr >> g_lastMove.index;
		g_lastMove.stone = matches[2];
		g_lastMove.x = matches[3];
		istringstream nbStr(matches[4]); nbStr >> g_lastMove.y;
		g_lastMove.captured = matches[5];

		if( g_lastMove.captured != "" )
		{
			cout << "CAPTURES!! " << g_lastMove.captured << endl;
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
		cout << iRegex << " is not a valid regular expression: \""
			<< e.what() << "\"" << endl;
		return false;
	}

	boost::cmatch matches;
	if (boost::regex_match(iLine.c_str(), matches, re))
	{
		//cout << re << " matches " << iLine << endl;

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

LIBIGS_API bool cb_connect_igs()
{
	// test

	//lineMatches("  0(B): A1","  (\\d)\\((B|W)\\): (\\w)(\\d)");
	//extractMove("  0(B): A1 A2 A3");
	g_lastMove.index = -1;

	m_Status = IGS_STATUS_DISCONNECTED;

	boost::asio::io_service* const service = new boost::asio::io_service;
	tcp::resolver resolver( *service );

	tcp::resolver::iterator endpoint_iterator;

	try
	{
		cout << "Looking for address." << endl;

		const string addr("igs.joyjoy.net");

		const tcp::resolver::query query(
			addr.c_str(),
			"6969",
			boost::asio::ip::resolver_query_base::numeric_service );

		endpoint_iterator = resolver.resolve( query );
	}
	catch( ... )
	{
		cout << "Failed to connect to server." << endl;
		return 0;
	}

	tcp::resolver::iterator end;

	cout << "Creating socket." << endl;
	m_Socket = new tcp::socket( *service );

	m_Socket->open( boost::asio::ip::tcp::v4() );

	const boost::asio::ip::tcp::no_delay option( true );
	boost::system::error_code ec;
	m_Socket->set_option( option, ec );
	cout << "TCP::No_Delay status: " << ec.value() << endl;

	tcp::endpoint currentEndPoint = *endpoint_iterator;

	while( currentEndPoint.protocol().family() != 2 && endpoint_iterator != end )
	{
		cout
			<< "Testing ... to : " << (*endpoint_iterator).host_name()
			<< " ( " << currentEndPoint.address().to_string()
			<< " : " << currentEndPoint .port() << ", "
			<< currentEndPoint .protocol().family() << " ) "
			<< endl;

		++endpoint_iterator;
		currentEndPoint = *endpoint_iterator;
	}

	boost::system::error_code error = boost::asio::error::host_not_found;
	//while (error && endpoint_iterator != end)
	for( int i = 0; i < 10; i++ )
	{
		//m_Socket->close();
		m_Socket->connect( *endpoint_iterator/*++*/, error );

		if( !error )
		{
			break;
		}
		else
		{
			cout << "Attempt " << ( i + 1 ) << ": Can't connect to server." << endl;
			Sleep( 3000 );
			//throw boost::system::system_error( error );
			//return false;
		}
	}

	if( error )
	{
		cout << "Definitely can't connect to server" <<  endl;
		//throw boost::system::system_error(error);
		return 0;
	}
	else
	{
		cout << "Successfully connected to server." << endl;
			
		boost::asio::socket_base::keep_alive keepAlive( true );
		m_Socket->set_option( keepAlive );

		g_log = new ofstream( "c:\\temp\\go.txt" );

		m_Status = IGS_STATUS_WAITING_LOGIN;
	}

	return true;
}

LIBIGS_API void cb_disconnect_igs()
{
	cb_igs_writeline("quit");
	delete m_Socket;
	m_Socket=0;
	m_Status = IGS_STATUS_DISCONNECTED;
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
			size_t len = boost::asio::read( *m_Socket, boost::asio::buffer( &chr, 1 ));
			if( chr != '\r' && chr != '\n' ) line += chr;
			if( chr == '\n' ) lineOk=true;

			if( line == "Login: " ) lineOk=true;
		}

		//VRLOGD2 << VR_INFO << "Waited : " << ( VRTIME_MS - startTime ) << " ms." << endl;
		catch( ... )
		{
			cout << "Exception while reading form network. Server closed connection ?" << endl;
		}
	}

	(*g_log) << line << endl;

	if( line != "1 5" && line != "" && lineContains( line, "connected") )
	{
		cout << line << endl;
	}

	// Save to file

	return line;
}

LIBIGS_API bool cb_igs_writeline( std::string iLine )
{
	cout << ">>> '" << iLine << "'" << endl;

	(*g_log) << ">>> '" << iLine << "'" << endl;

	string line = iLine + '\r' + '\n';
	boost::asio::write( *m_Socket, boost::asio::buffer( line, line.size() ) );
	return true;
}

LIBIGS_API int cb_igs_read_event()
{
	string line = cb_igs_readline();

	if( line == "Login: " )
	{
		return IGS_EVENT_LOGIN;
	}
	else if( lineContains( line, "IGS entry") )
	{
		// XXX Check if status was WAITING_LOGIN
		m_Status = IGS_STATUS_MAIN_HALL;
		return IGS_EVENT_LOGGED_IN;
	}
	else if( lineContains( line, "accepted") )
	{
		// XXX check previous status
		m_Status = IGS_STATUS_IN_GAME;
		return IGS_EVENT_GAME_ACCEPTED;
	}
	else if( lineContains( line, "declined") )
	{
		// XXX check previous status
		m_Status = IGS_STATUS_MAIN_HALL;
		return IGS_EVENT_GAME_DECLINED;
	}

	/////// IN GAME
	else if( m_Status == IGS_STATUS_IN_GAME )
	{
		if( lineMatches( line, g_moveRE) )
		{
			if( extractMove(line) )
			{
				cout << "Last Move : " << g_lastMove.stone << ":" << g_lastMove.x << g_lastMove.y << endl;
				return IGS_EVENT_MOVE;
			}
		}
		else
		{
			cout << "Unknown line: " << line << endl;
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
	m_Status = IGS_STATUS_WAITING_CHALLENGE_ANSWER;

	return true;
}

LIBIGS_API bool  cb_igs_say(char* iMsg)
{
	cb_igs_writeline("say " + string(iMsg) );

	return true;
}

LIBIGS_API bool  cb_igs_play(string iMove)
{
	if( m_Status == IGS_STATUS_IN_GAME )
	{
		cb_igs_writeline( iMove );
	}
	else
	{
		cout << "NOT IN GAME!" << endl;
	}

	return true;
}

LIBIGS_API int cb_igs_get_status()
{
	return m_Status;
}

LIBIGS_API int cb_igs_get_last_move_index()
{
	return g_lastMove.index;
}

LIBIGS_API char* cb_igs_get_last_move_stone()
{
	return (char*)g_lastMove.stone.c_str();
}

LIBIGS_API char* cb_igs_get_last_move_x()
{
	return (char*)g_lastMove.x.c_str();
}

LIBIGS_API int cb_igs_get_last_move_y()
{
	return g_lastMove.y;
}


/*
// This is the constructor of a class that has been exported.
// see libigs.h for the class definition
Clibigs::Clibigs()
{
	return;
}
*/