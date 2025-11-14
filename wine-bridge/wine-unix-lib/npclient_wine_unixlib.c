#ifndef WINE_UNIX_LIB
WINE_UNIX_LIB must be defined for wine/unixlib.h
#endif

#include "npclient_wine_unixlib.h"
#include "fttypes.h"
#include "opentrackclient-private.h"

NTSTATUS NP_GetFTData(FTHeap *ftheap)
{
    OTR_Priv_Lock();
    OTRPosixSHM *shm_posix = OTR_Priv_GetSHM();
    FTData* data = &ftheap->data;
    data->Yaw = _OTR_GetFT_Yaw(shm_posix->data);
    data->Pitch = _OTR_GetFT_Pitch(shm_posix->data);
    data->Roll = _OTR_GetFT_Roll(shm_posix->data);
    data->X = _OTR_GetFT_X(shm_posix->data);
    data->Y = _OTR_GetFT_Y(shm_posix->data);
    data->Z = _OTR_GetFT_Z(shm_posix->data);

    data->DataID++;
    data->CamWidth = 250;
    data->CamHeight = 100;
    ftheap->GameID = shm_posix->gameid;
    ftheap->GameID2 = shm_posix->gameid2;
    for (int i = 0; i < 8; i++)
        ftheap->table[i] = shm_posix->table[i];

    OTR_Priv_Unlock();

    return 0;
}


NTSTATUS NP_RegisterProgramProfileID(unsigned short id){
    OTRPosixSHM *shm_posix = OTR_Priv_GetSHM();
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

