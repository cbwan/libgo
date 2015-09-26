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

LIBGO_API void cbgo_init(void);
LIBGO_API bool cbgo_is_init();
LIBGO_API void cbgo_clear_board( int boardsize );
LIBGO_API void cbgo_play_move(int i, int j, int color );
LIBGO_API void cbgo_genmove(int color);
LIBGO_API int  cbgo_get_genmove_x();
LIBGO_API int  cbgo_get_genmove_y();
LIBGO_API bool cbgo_is_legal(int i, int j, int color);
LIBGO_API int  cbgo_get_board_color(int i, int j);
LIBGO_API bool cbgo_undo_move(int n);
LIBGO_API float cbgo_get_score(void);
LIBGO_API int   cbgo_get_move_number(void);
LIBGO_API int   cbgo_get_white_captured(void);
LIBGO_API int   cbgo_get_black_captured(void);
LIBGO_API void  cbgo_set_komi(float new_komi);
LIBGO_API float cbgo_get_komi(void);
LIBGO_API int   cbgo_placehand(int handicap);
LIBGO_API bool  cbgo_is_ko();
LIBGO_API int   cbgo_get_ko_x();
LIBGO_API int   cbgo_get_ko_y();

//LIBGO_API void  cb_gnugo_recordboard(

// void gnugo_recordboard(SGFNode *node);
// void gnugo_add_stone(int i, int j, int color);
// void gnugo_remove_stone(int i, int j);
// int is_self_atari(int pos, int color);
//#ifdef __cplusplus
}
//#endif
