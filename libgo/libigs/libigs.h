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

LIBIGS_API bool  cb_connect_igs(void);
LIBIGS_API void  cb_disconnect_igs(void);
LIBIGS_API bool  cb_read_igs(void);
