// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBIGS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBIGS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBIGS_EXPORTS
#define LIBIGS_API __declspec(dllexport)
#else
#define LIBIGS_API __declspec(dllimport)
#endif

#include <iostream>

// Events
enum
{
	IGS_EVENT_UNKNOWN=-1,
	IGS_EVENT_NO_EVENT,
	IGS_EVENT_CONNECTED,
	IGS_EVENT_LOGIN_PROMPT,
	IGS_EVENT_LOGGED_IN,
	IGS_EVENT_LOGIN_FAILED,
	IGS_EVENT_RECEIVED_CHALLENGE,
	IGS_EVENT_CHALLENGE_ACCEPTED,
	IGS_EVENT_CHALLENGE_DECLINED,
	IGS_EVENT_MOVE,
	IGS_EVENT_PASS,
	IGS_EVENT_RESIGN,
	IGS_EVENT_WIN,
	IGS_EVENT_LOSE,
	IGS_EVENT_CHAT_MSG,
};

// Status
enum
{
	IGS_STATUS_UNKNOWN=-1,
	IGS_STATUS_DISCONNECTED,
	IGS_STATUS_WAITING_LOGIN_PROMPT,
	IGS_STATUS_AT_LOGIN_PROMPT,
	IGS_STATUS_LOBBY,
	IGS_STATUS_WAITING_CHALLENGE_ANSWER,
	IGS_STATUS_IN_GAME
	//IGS_STATUS_IN_GAME_OPPONENTS_TURN
};

extern "C" {

LIBIGS_API void  cbgo_igs_init(void);
LIBIGS_API bool  cbgo_connect_igs(void);
LIBIGS_API void  cbgo_disconnect_igs(void);
LIBIGS_API bool  cbgo_igs_is_connected(void);

LIBIGS_API int   cbgo_igs_get_status(void);
LIBIGS_API int   cbgo_igs_get_events_nb(void);
LIBIGS_API int   cbgo_igs_get_event(void);
LIBIGS_API bool  cbgo_igs_login(char* iLogin, char* iPwd);
LIBIGS_API bool  cbgo_igs_challenge(char* iUser, int iMyColor, int iBoardSize, int iMainTime, int iByoyomiTime );
LIBIGS_API bool  cbgo_igs_play( int x, int y ); // "A1"
LIBIGS_API bool  cbgo_igs_say( char* iMsg);
LIBIGS_API bool  cbgo_igs_pass();
LIBIGS_API bool  cbgo_igs_resign();
LIBIGS_API char* cbgo_igs_get_challenger();
LIBIGS_API void  cbgo_igs_accept_challenge();
LIBIGS_API void  cbgo_igs_decline_challenge();
LIBIGS_API char* cbgo_igs_get_chat_msg();

// blocking reads
LIBIGS_API bool  cbgo_igs_read_event();
LIBIGS_API int   cbgo_igs_wait_event();

LIBIGS_API int   cbgo_igs_get_last_move_index();
LIBIGS_API int   cbgo_igs_get_last_move_stone();
LIBIGS_API int   cbgo_igs_get_last_move_x();
LIBIGS_API int   cbgo_igs_get_last_move_y();
LIBIGS_API void  cbgo_log( char* );
}

LIBIGS_API std::string cbgo_igs_readline(void);
LIBIGS_API bool cbgo_igs_writeline(std::string iLine);

