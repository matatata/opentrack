#ifdef WINE_UNIX_LIB

#include <stdio.h>

#include "npclient_wine_unixlib.h"
#include "fttypes.h"
#include "freetrackclient_posix_private.h"

NTSTATUS NP_GetFTData(FTHeap *ftheap){
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    FTData* data = &ftheap->data;
    data->Yaw = shm_posix->data.Yaw;
    data->Pitch = shm_posix->data.Pitch;
    data->Roll = shm_posix->data.Roll;
    data->X = shm_posix->data.X;
    data->Y = shm_posix->data.Y;
    data->Z = shm_posix->data.Z;
    data->DataID++;
    data->CamWidth = 250;
    data->CamHeight = 100;
    //game id??
    ftheap->GameID = shm_posix->gameid;
    ftheap->GameID2 = shm_posix->gameid2;

    for (int i = 0; i < 8; i++)
        ftheap->table[i] = shm_posix->table[i];

    FTX_Priv_Unlock();

    return 0;
}


NTSTATUS NP_RegisterProgramProfileID(unsigned short id){
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
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

const unixlib_entry_t __wine_unix_call_funcs[] =
{
    unix_NP_GetFTData,
    unix_NP_RegisterProgramProfileID
};

#ifdef _WIN64

// https://list.winehq.org/pipermail/wine-devel/2022-April/213679.html

const unixlib_entry_t __wine_unix_call_wow64_funcs[] =
{
    unix_NP_GetFTData,
    unix_NP_RegisterProgramProfileID
};
#endif

#endif

