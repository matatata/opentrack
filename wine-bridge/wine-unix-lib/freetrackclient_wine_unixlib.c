#ifndef WINE_UNIX_LIB
WINE_UNIX_LIB must be defined for wine/unixlib.h
#endif

#include "opentrackclient-private.h"
#include "freetrackclient_wine_unixlib.h"
#include "fttypes.h"


#if OTR_WINE_BRIDGE_DEMO_PERIOD > 0
#include <time.h>
time_t start_time = 0;
__attribute__((constructor))
static void init() {
    char msg[1000];
    sprintf(msg,"The head pose transfer will stop after %d minutes. If the software works well for you, please consider purchasing the unrestricted version to support my work. Thank you in advance.",OTR_WINE_BRIDGE_DEMO_PERIOD);
    OTR_Priv_NotifyUser("This is a demo version",msg,15);
    start_time = time(NULL);
}
#endif


BOOL FTGetData(FTHeap* ftheap)
{
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    FTData* data = &ftheap->data;
    #if OTR_WINE_BRIDGE_DEMO_PERIOD > 0
    if((time(NULL) - start_time) > (OTR_WINE_BRIDGE_DEMO_PERIOD * 60)){
       data->Yaw = data->Pitch = data->Roll = data->X = data->Y = data->Z = 0;
    }
    else {
    #endif

    data->Yaw = _OTR_GetFT_Yaw(shm_posix->data);
    data->Pitch = _OTR_GetFT_Pitch(shm_posix->data);
    data->Roll = _OTR_GetFT_Roll(shm_posix->data);
    data->X = _OTR_GetFT_X(shm_posix->data);
    data->Y = _OTR_GetFT_Y(shm_posix->data);
    data->Z = _OTR_GetFT_Z(shm_posix->data);

    #if OTR_WINE_BRIDGE_DEMO_PERIOD > 0
    }
    #endif

    data->DataID++;
    data->CamWidth = 250;
    data->CamHeight = 100;
    ftheap->GameID2 = shm_posix->gameid2;
    shm_posix->gameid = ftheap->GameID;
    for (int i = 0; i < 8; i++)
        ftheap->table[i] = shm_posix->table[i];

    FTX_Priv_Unlock();

    return true;
}


static NTSTATUS unix_FTGetData(void *args)
{
   return (NTSTATUS)FTGetData((FTHeap*)args);
}


const unixlib_entry_t __wine_unix_call_funcs[] =
{
    unix_FTGetData
};



#ifdef _WIN64

// https://list.winehq.org/pipermail/wine-devel/2022-April/213679.html

const unixlib_entry_t __wine_unix_call_wow64_funcs[] =
{
    unix_FTGetData
};

#endif

