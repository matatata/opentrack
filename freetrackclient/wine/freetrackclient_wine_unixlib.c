#ifdef WINE_UNIX_LIB
#include "freetrackclient_posix_private.h"
#include "freetrackclient_wine_unixlib.h"
#include "fttypes.h"


BOOL FTGetData(FTHeap* ftheap)
{
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    FTData* data = &ftheap->data;
    data->Yaw = shm_posix->data[FTX_YAW];
    data->Pitch = shm_posix->data[FTX_PITCH];
    data->Roll = shm_posix->data[FTX_ROLL];
    data->X = shm_posix->data[FTX_X];
    data->Y = shm_posix->data[FTX_Y];
    data->Z = shm_posix->data[FTX_Z];


    //  printf("posix: %f %f %f %f %f %f\n",shm_posix->data[FTX_YAW],
    //                                 shm_posix->data[FTX_PITCH],
    //                                 shm_posix->data[FTX_ROLL],
    //                                 shm_posix->data[FTX_X],
    //                                 shm_posix->data[FTX_Y],
    //                                 shm_posix->data[FTX_Z]);

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


#endif
