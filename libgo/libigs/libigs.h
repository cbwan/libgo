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

/*
// This class is exported from the libigs.dll
class LIBIGS_API Clibigs {
public:
	Clibigs(void);
	// TODO: add your methods here.
};*/

#include <iostream>

// Events
enum
{
	IGS_EVENT_UNKNOWN=-1,
	IGS_EVENT_GAME_ACCEPTED=0,
	IGS_EVENT_GAME_DECLINED=1,
	IGS_EVENT_MOVE=2,
	IGS_EVENT_PASS=3,
	IGS_EVENT_RESIGN=4,
	IGS_EVENT_WIN=5,
	IGS_EVENT_LOGIN=6,
	IGS_EVENT_LOGGED_IN=7
};

// Status
enum
{
	IGS_STATUS_UNKNOWN=-1,
	IGS_STATUS_DISCONNECTED,
	IGS_STATUS_WAITING_LOGIN,
	IGS_STATUS_MAIN_HALL,
	IGS_STATUS_WAITING_CHALLENGE_ANSWER,
	IGS_STATUS_IN_GAME,
};

extern "C" {

LIBIGS_API bool  cb_connect_igs(void);
LIBIGS_API void  cb_disconnect_igs(void);

LIBIGS_API int   cb_igs_get_status(void);
LIBIGS_API bool  cb_igs_login(char* iLogin, char* iPwd);
LIBIGS_API bool  cb_igs_challenge(char* iUser, char* iMyColor);
LIBIGS_API bool  cb_igs_play( char* iMove ); // "A1" ..
LIBIGS_API bool  cb_igs_say( char* iMsg);
LIBIGS_API int   cb_igs_read_event();
LIBIGS_API int   cb_igs_wait_event();

LIBIGS_API int   cb_igs_get_last_move_index();
LIBIGS_API char* cb_igs_get_last_move_stone();
LIBIGS_API char* cb_igs_get_last_move_x();
LIBIGS_API int   cb_igs_get_last_move_y();
LIBIGS_API void  cb_log( char* );
}

LIBIGS_API std::string cb_igs_readline(void);
LIBIGS_API bool cb_igs_writeline(std::string iLine);

