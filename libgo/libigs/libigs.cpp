// libigs.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libigs.h"

#include <iostream>

#include <boost/asio.hpp>
//#include "RakPeerInterface.h"

using namespace std;

boost::asio::io_service*      m_IOService;
boost::asio::ip::tcp::socket* m_Socket;

using boost::asio::ip::tcp;
/*
RakNet::RakPeerInterface *peer=0;
RakNet::Packet *packet=0;
*/

LIBIGS_API bool cb_connect_igs()
{
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
			cout
				<< "Attempt " << ( i + 1 ) << ": Can't connect to server." << endl;
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
		cout
			<< "Successfully connected to server." << endl;
			
		boost::asio::socket_base::keep_alive keepAlive( true );
		m_Socket->set_option( keepAlive );
		return service;
	}

	/*
	peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd;
	sd.socketFamily=AF_INET;
	peer->Startup(1,&sd, 1);
	RakNet::ConnectionAttemptResult res = peer->Connect("igs.joyjoy.net", 6969, 0,0);

	if( res !=RakNet::CONNECTION_ATTEMPT_STARTED )
	{
		cout << "server :(" << endl;
		return false;
	}
	//if( res == RakNet::ConnectionAttemptResult::
	*/
	return true;
}

LIBIGS_API bool cb_read_igs()
{
	/*
	for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
	{
		printf("%s",packet->data);
	}*/

	boost::system::error_code error;

	char msg[1];

	try
	{
		//cout << "Waiting for packet..." << endl;

		size_t len = boost::asio::read(
			*m_Socket,
			boost::asio::buffer( msg, 1 ));

		cout << msg[0]; // << endl;
	}

		//VRLOGD2 << VR_INFO << "Waited : " << ( VRTIME_MS - startTime ) << " ms." << endl;
	catch( ... )
		{
			cout << "Exception while reading form network. Server closed connection ?" << endl;
		}

	return true;
}

LIBIGS_API void cb_disconnect_igs()
{
	//RakNet::RakPeerInterface::DestroyInstance(peer);
}

/*
// This is the constructor of a class that has been exported.
// see libigs.h for the class definition
Clibigs::Clibigs()
{
	return;
}
*/