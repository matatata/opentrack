#include <stdarg.h>


#if _WIN64
#define OTRCLIENT_DLLNAME "otrclient64.dll"
#else
#define OTRCLIENT_DLLNAME "otrclient.dll"
#endif

#ifdef WINE_BUILTIN_DLL
#include "otrclient_wine_unixlib.h"
#endif
#include "freetrackclient/fttypes.h"


#define FT_EXPORT(t) __declspec(dllexport) t __stdcall

/**
 *  returns 0 if tracking stopped, otherwise 1
 */
FT_EXPORT(int) OTR_GetFTHeap(FTHeap* ftheap);


#define OTR_GET_FT_HEAP_FUNC_NAME "OTR_GetFTHeap"
typedef int (__stdcall *f_otr_GetFTData)(FTHeap *ft);

