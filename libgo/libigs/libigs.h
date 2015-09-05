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
	EVENT_UNKNOWN=-1,
	EVENT_GAME_ACCEPTED=0,
	EVENT_GAME_DECLINED=1,
	EVENT_MOVE=2,
	EVENT_PASS=3,
	EVENT_RESIGN=4,
	EVENT_WIN=5,
	EVENT_LOGIN=6,
	EVENT_LOGGED_IN=7
};

// Status
enum
{
	STATUS_UNKNOWN=-1,
	STATUS_DISCONNECTED,
	STATUS_WAITING_LOGIN,
	STATUS_MAIN_HALL,
	STATUS_WAITING_CHALLENGE_ANSWER,
	STATUS_IN_GAME,
};

LIBIGS_API bool  cb_connect_igs(void);
LIBIGS_API void  cb_disconnect_igs(void);

LIBIGS_API int   cb_igs_get_status(void);
LIBIGS_API bool  cb_igs_login(std::string iLogin, std::string iPwd);
LIBIGS_API bool  cb_igs_challenge(std::string iUser, std::string iMyColor);
LIBIGS_API bool  cb_igs_play( std::string iMove ); // "A1" ..
LIBIGS_API bool  cb_igs_say(std::string iMsg);
LIBIGS_API int   cb_igs_read_event();
LIBIGS_API int   cb_igs_wait_event();

LIBIGS_API int         cb_igs_get_last_move_index();
LIBIGS_API std::string cb_igs_get_last_move_stone();
LIBIGS_API std::string cb_igs_get_last_move_x();
LIBIGS_API int         cb_igs_get_last_move_y();

LIBIGS_API std::string cb_igs_readline(void);
LIBIGS_API bool cb_igs_writeline(std::string iLine);
