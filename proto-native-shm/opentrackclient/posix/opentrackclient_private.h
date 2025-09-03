
#ifndef OTR_CLIENT_PRIVATE_H
#define OTR_CLIENT_PRIVATE_H

/*
 * Please do not use this private API unless you really need to.
 * as it may change and thus break things in future. Instead rely on the public header:
 */
#include "opentrackclient_public.h"


#ifdef __cplusplus
extern "C"
{
#endif

// For now we use the same SHM model as the prot-wine:
#define WINE_SHM_NAME "facetracknoir-wine-shm"
#define WINE_MTX_NAME "facetracknoir-wine-mtx"

enum Axis {
    TX = 0, TY, TZ, Yaw, Pitch, Roll
};

typedef struct WineSHM
{
    double data[6];

    // compatibility stuff for freetrack and npclient
    int gameid, gameid2;
    unsigned char table[8];
    // ---

    bool stop;
} WineSHM;

typedef struct shm_wrapper
{
    void* mem;
    int fd;
    unsigned size;
} shm_wrapper;

void OTR_Priv_Lock();

// when accessing the shm put in in between Lock() and Unlock()
WineSHM *OTR_Priv_GetSHM();

void OTR_Priv_Unlock();

#ifdef __cplusplus
}
#endif


#endif
