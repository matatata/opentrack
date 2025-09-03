#ifdef WINE_UNIX_LIB

#include <stdio.h>

#include "npclient_wine_unixlib.h"
#include "fttypes.h"
#include "opentrackclient_private.h"

NTSTATUS NP_GetFTData(FTHeap *ftheap){
    OTR_Priv_Lock();
    WineSHM *shm_posix = OTR_Priv_GetSHM();
    FTData* data = &ftheap->data;
    data->Yaw = -shm_posix->data[Yaw];
    data->Pitch = -shm_posix->data[Pitch];
    data->Roll = shm_posix->data[Roll];
    data->X = shm_posix->data[TX];
    data->Y = shm_posix->data[TY];
    data->Z = shm_posix->data[TZ];
    data->DataID++;
    data->CamWidth = 250;
    data->CamHeight = 100;
    //game id??
    ftheap->GameID = shm_posix->gameid;
    ftheap->GameID2 = shm_posix->gameid2;

    for (int i = 0; i < 8; i++)
        ftheap->table[i] = shm_posix->table[i];

    OTR_Priv_Unlock();

    return 0;
}


NTSTATUS NP_RegisterProgramProfileID(unsigned short id){
    WineSHM *shm_posix = OTR_Priv_GetSHM();
    shm_posix->gameid = id;
    return 0;
}



static NTSTATUS unix_NP_GetFTData(void *args)
{
   return (NTSTATUS)NP_GetFTData((FTHeap*)args);
}

static NTSTATUS unix_NP_RegisterProgramProfileID(void *args)
{
   return (NTSTATUS)NP_RegisterProgramProfileID(*(int *)args);
}

static NTSTATUS unix_OTRInit(void *args)
{
   if(!OTRInit())
        return -1;
   return 0;
}

static NTSTATUS unix_OTRClose(void *args)
{
   OTRClose();
   return 0;
}


const unixlib_entry_t __wine_unix_call_funcs[] =
{
    unix_OTRInit,
    unix_OTRClose,
    unix_NP_GetFTData,
    unix_NP_RegisterProgramProfileID
};

#ifdef _WIN64

// https://list.winehq.org/pipermail/wine-devel/2022-April/213679.html

const unixlib_entry_t __wine_unix_call_wow64_funcs[] =
{
    unix_OTRInit,
    unix_OTRClose,
    unix_NP_GetFTData,
    unix_NP_RegisterProgramProfileID
};
#endif

#endif

