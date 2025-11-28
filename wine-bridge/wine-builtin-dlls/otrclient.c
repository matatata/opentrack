#include "otrclient.h"


#ifndef WINE_BUILTIN_DLL
#error WINE_BUILTIN_DLL not defined
#endif



BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, void *reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {

        if(__wine_init_unix_call()){
            WINE_ERR("__wine_init_unix_call failed\n");
            return FALSE;
        }
        WINE_TRACE("__wine_init_unix_call success\n");
        DisableThreadLibraryCalls(instance);
    }
    else if(reason == DLL_PROCESS_DETACH){

    }
    return TRUE;
}



FT_EXPORT(int) OTR_GetFTHeap(FTHeap* ftheap){
    return WINE_UNIX_CALL(otr_get_ftheap,(void*)ftheap);
}
