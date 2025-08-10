#ifndef __WIN32
#   error "bad cross"
#endif

#define shm_wrapper ShmWine
#define __WINE_OLE2_H
// OSX sdk 10.8 build error otherwise
#undef _LIBCPP_MSVCRT

#include "compat/shm.h"
#include "compat/shm.cpp"
#include "wine-shm.h"
#include "compat/library-path.hpp"
#include <cstdlib>
#include <cstring>
#include <sysexits.h>

using std::strcat;

bool get_dll_path(char *dir,uint32_t bufSize)
{
    if (GetCurrentDirectoryA(bufSize, dir) < bufSize)
    {
        for (int i = 0; dir[i]; i++)
            if (dir[i] == '\\')
                dir[i] = '/';
        // there's always a leading and trailing slash
        #if DLL
        strcat(dir, "/libs/");
        #else
        strcat(dir, OPENTRACK_LIBRARY_PATH);
        #endif
        //strcat(dir, "/");
        return TRUE;
    }
    return FALSE;
}

/**
 * path = false removes values from the registry
 */
static void write_path(const char* key, const char* subkey, bool path)
{
    char dir[8192] {};

    if (get_dll_path(dir,8192))
    {
        HKEY hkpath;
        if (RegCreateKeyExA(HKEY_CURRENT_USER,
                            key,
                            0,
                            NULL,
                            0,
                            KEY_ALL_ACCESS,
                            NULL,
                            &hkpath,
                            NULL) == ERROR_SUCCESS)
        {
            if (!path)
                dir[0] = '\0';
            (void) RegSetValueExA(hkpath, subkey, 0, REG_SZ, (BYTE*) dir, strlen(dir) + 1);
            RegCloseKey(hkpath);
        }
    }
}

/**
 * selection 1=freetrack, 2=NPClient 3=Both, 0 = look for OTR_WINE_PROTO env variable
 * -1 == erase keys
 */
void create_registry_key(int32_t selection)
{
    bool use_freetrack, use_npclient;
    if(selection==0) {
        const char* env = getenv("OTR_WINE_PROTO");
        char* endptr;
        if (!env) env = "";
        selection = strtol(env, &endptr, 10);
        if (*endptr)
            selection = 0;
    }

    switch (selection)
    {
    case -1: use_freetrack = false, use_npclient = false; break;
    case 1: use_freetrack = true, use_npclient = false; break;
    case 2: use_freetrack = false, use_npclient = true; break;
    default:
    case 3: use_freetrack = true, use_npclient = true; break;
    }

    write_path("Software\\NaturalPoint\\NATURALPOINT\\NPClient Location", "Path", use_npclient);
    write_path("Software\\Freetrack\\FreeTrackClient", "Path", use_freetrack);
}
