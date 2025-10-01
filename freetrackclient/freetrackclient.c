/***********************************************************************************
 * * FTTypes            FTTypes contains the specific type definitions for the      *
 * *                    FreeTrack protocol.                                         *
 * *                    It was loosely translated from FTTypes.pas                  *
 * *                    which was created by the FreeTrack-team.                    *
 * *                                                                                *
 * * Wim Vriend (Developing)                                                        *
 * * Ron Hendriks (Testing and Research)                                            *
 * *                                                                                *
 * * Homepage               <http://facetracknoir.sourceforge.net/home/default.htm> *
 * *                                                                                *
 * * This program is distributed in the hope that it will be useful, but            *
 * * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY     *
 * * or FITNESS FOR A PARTICULAR PURPOSE.                                           *
 * *                                                                                *
 * *                                                                                *
 * * The FreeTrackClient sources were translated from the original Delphi sources   *
 * * created by the FreeTrack developers.
 *
 *
 * * September 2025: The Wine unix calls were added
 * * by matatata https://github.com/matatata/opentrack                                       *
 */

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <string.h>
#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"

#include "fttypes.h"

#if defined _MSC_VER && !defined _WIN64
#   define FT_EXPORT(t) t __stdcall
#else
#   define FT_EXPORT(t) __declspec(dllexport) t __stdcall
#endif

#if DEBUG
#   include <stdio.h>
static FILE *debug_stream = 0;
#   define dbg_report(...) if (debug_stream) { fprintf(debug_stream, __VA_ARGS__); fflush(debug_stream); }
#else
#define dbg_report(...) ((void)0)
#endif
#ifdef FT_WINE_UNIX_LIB
#include "freetrackclient_wine_unixlib.h"
#else
static HANDLE hFTMemMap = 0;
static FTHeap* ipc_heap = 0;
static HANDLE ipc_mutex = 0;
#endif
static const char* dllVersion = "1.0.0.0";
static const char* dllProvider = "FreeTrack";


BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        #if DEBUG
        debug_stream = fopen("c:\\FreeTrackClient.log", "a");
        #endif

        dbg_report("DLL_PROCESS_ATTACH\n");
#ifdef FT_WINE_UNIX_LIB
        if(__wine_init_unix_call()){
            dbg_report("__wine_init_unix_call failed\n");
            return FALSE;
        }
#endif
        DisableThreadLibraryCalls(instance);
    }
    else if(reason == DLL_PROCESS_DETACH){
        dbg_report("DLL_PROCESS_DETACH\n");
    }
    return TRUE;
}

#ifdef FT_WINE_UNIX_LIB
FT_EXPORT(int) OpentrackUnixLibVersion()
{
    return 1;
}
#else
static BOOL impl_create_mapping(void)
{
    if (ipc_heap != NULL)
        return TRUE;

    hFTMemMap = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                   NULL,
                                   PAGE_READWRITE,
                                   0,
                                   sizeof(FTHeap),
                                   (LPCSTR) FREETRACK_HEAP);

    if (hFTMemMap == NULL)
        return (ipc_heap = NULL), FALSE;

    ipc_heap = (FTHeap*) MapViewOfFile(hFTMemMap, FILE_MAP_WRITE, 0, 0, sizeof(FTHeap));
    ipc_mutex = CreateMutexA(NULL, FALSE, FREETRACK_MUTEX);
    return TRUE;
}
#endif



FT_EXPORT(BOOL) FTGetData(FTData* data)
{
#ifdef FT_WINE_UNIX_LIB
    // no lazy initialization, we already initialized otrclient in DllMain
    return WINE_UNIX_CALL(ft_get_data,(void*)data);
#else
    if (impl_create_mapping() == FALSE)
        return FALSE;

    if (ipc_mutex && WaitForSingleObject(ipc_mutex, 16) == WAIT_OBJECT_0) {
        memcpy(data, &ipc_heap->data, sizeof(FTData));
        if (ipc_heap->data.DataID > (1 << 29))
            ipc_heap->data.DataID = 0;
        ReleaseMutex(ipc_mutex);
    }
    return TRUE;
#endif
}

/*
// For some mysterious reason, the previously existing function FTReportID has been changed to FTReportName, but with an integer as argument.
// The Delphi-code from the FreeTrack repo suggest a char * as argument, so it cost me an afternoon to figure it out (and keep ArmA2 from crashing).
// Thanks guys!
*/
FT_EXPORT(void) FTReportName( int name )
{
    dbg_report("FTReportName request (ID = %d).\n", name);
}

FT_EXPORT(void) FTReportID( int name )
{
    dbg_report("FTReportID request (ID = %d).\n", name);
}

FT_EXPORT(const char*) FTGetDllVersion(void)
{
    dbg_report("FTGetDllVersion request.\n");
    return dllVersion;
}

FT_EXPORT(const char*) FTProvider(void)
{
    dbg_report("FTProvider request.\n");
    return dllProvider;
}

#if defined _MSC_VER && !defined _WIN64
#pragma comment (linker, "/export:FTReportID=_FTReportID@4")
#pragma comment (linker, "/export:FTReportName=_FTReportName@4")
#pragma comment (linker, "/export:FTGetDllVersion=_FTGetDllVersion@0")
#pragma comment (linker, "/export:FTProvider=_FTProvider@0")
#pragma comment (linker, "/export:FTGetData=_FTGetData@4")
#endif
