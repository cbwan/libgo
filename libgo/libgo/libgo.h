// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBGO_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBGO_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBGO_EXPORTS
#define LIBGO_API __declspec(dllexport)
#else
#define LIBGO_API __declspec(dllimport)
#endif

//#ifdef __cplusplus
extern "C" {
//#endif

LIBGO_API void cb_init_gnugo(void);
LIBGO_API bool cb_is_gnugo_init();
LIBGO_API void cb_gnugo_clear_board( int boardsize );
LIBGO_API void cb_gnugo_play_move(int i, int j, int color );
LIBGO_API void cb_genmove(int color);
LIBGO_API bool cb_gnugo_is_legal(int i, int j, int color);
LIBGO_API int  cb_get_board_color(int i, int j);
LIBGO_API bool cb_gnugo_undo_move(int n);
LIBGO_API float cb_get_score(void);
LIBGO_API int   cb_gnugo_get_move_number(void);
LIBGO_API int   cb_get_white_captured(void);
LIBGO_API int   cb_get_black_captured(void);
//#ifdef __cplusplus
}
//#endif
