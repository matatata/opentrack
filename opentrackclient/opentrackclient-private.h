/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 * https://github.com/matatata/opentrack
 *
 * Based/Inspired on/by opentrack/freetracknoir
 *
 * Stanislaw Halik and others.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
#ifndef OPENTRACKCLIENT_PRIVATE_H
#define OPENTRACKCLIENT_PRIVATE_H

/*
 * !!!ATTENTION!!!
 * Please do not use this private API unless you really need to,
 * as it may change and thus break things in future. Instead rely on
 * the api defined in the public header opentrackclient.h
 * If you use private API then you should link statically.
 */
#include "opentrackclient.h"

#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

// For now the shared memory is make compatible with the origina proto-wine see proto-wine/wine-shm.h
// It's subject to change in future
#define OTR_SHM_NAME "facetracknoir-wine-shm"
#define OTR_MTX_NAME "facetracknoir-wine-mtx"

// for now it's compatible with FT, except that Yaw and Pitch are inverted
typedef double ft_headpose[6];

typedef struct OTRPosixSHM
{
    ft_headpose data;
    // compatibility to support freetrack and npclient under WINE jus like proto-wine does
    int gameid, gameid2;
    unsigned char table[8];
    // ---
    bool stop;
} OTRPosixSHM;

// currently uses flocks
// I plan to get rid of locks and maye use posix semaphores.
// Or even better _Atomic so that we do not need to (b)lock at all.
void OTR_Priv_Lock(void);

// when accessing the shm put in in between Lock() and Unlock()
OTRPosixSHM *OTR_Priv_GetSHM(void);

void OTR_Priv_Unlock(void);

// notify the user - this is very platform dependent. At least on windows and macOS we'll pop up some dialog
// if supported on the platform the dialog will be dismissed automatically in case you specify timeout > 0
void OTR_Priv_NotifyUser(const char* title,const char* message,unsigned long timeout_secs);

/** Transformations
 *
 * Otr          FT                          NP                          proto-wine             x-plane
 * Yaw          -1 * OtrYaw * M_PI/180      FTYaw * 16383 / M_PI        OtrYaw * M_PI/180       -1 * FTYaw * 180/M_PI
 * Pitch        -1 * OtrPitch * M_PI/180    FTPitch * 16383 / M_PI      OtrPitch * M_PI/180     -1 * FTPitch * 180/M_PI
 * Roll         OtrRoll * M_PI/180          FTRoll * 16383 / M_PI       OtrRoll * M_PI/180      FTRoll * 180/M_PI
 * X            OtrX * 10                   FTX * 16383 / 500           OtrX * 10               -1 * FTX * 1e-3
 * Y            OtrY * 10                   FTY * 16383 / 500           OtrY * 10               FTY * 1e-3
 * Z            OtrZ * 10                   FTZ * 16383 / 500           OtrZ * 10               FTZ * 1e-3
 */

enum {
    OTR_X = 0,
    OTR_Y,
    OTR_Z,
    OTR_YAW,
    OTR_PITCH,
    OTR_ROLL
};

// From SHM to FT
#define _OTR_GetFT_Yaw(data) - (data)[OTR_YAW]
#define _OTR_GetFT_Pitch(data) - (data)[OTR_PITCH]
#define _OTR_GetFT_Roll(data) (data)[OTR_ROLL]
#define _OTR_GetFT_X(data) (data)[OTR_X]
#define _OTR_GetFT_Y(data) (data)[OTR_Y]
#define _OTR_GetFT_Z(data) (data)[OTR_Z]

// From SHM to NP
#define _OTR_GetNP_Yaw(data) (_OTR_GetFT_Yaw(data) * 16383 / M_PI)
#define _OTR_GetNP_Pitch(data) (_OTR_GetFT_Pitch(data) * 16383 / M_PI)
#define _OTR_GetNP_Roll(data) (_OTR_GetFT_Roll(data) * 16383 / M_PI)
#define _OTR_GetNP_X(data) (_OTR_GetFT_X(data) * 16383 / 500)
#define _OTR_GetNP_Y(data) (_OTR_GetFT_Y(data) * 16383 / 500)
#define _OTR_GetNP_Z(data) (_OTR_GetFT_Z(data) * 16383 / 500)

// From opentrack app's internal represenation to representation in SHM
#define _OTR_SetSHM_Yaw(data, otr_val) { data[OTR_YAW] = otr_val * M_PI/180; }
#define _OTR_SetSHM_Pitch(data, otr_val) { data[OTR_PITCH] = otr_val * M_PI/180;}
#define _OTR_SetSHM_Roll(data, otr_val) { data[OTR_ROLL] = otr_val * M_PI/180;}
#define _OTR_SetSHM_X(data, otr_val) { data[OTR_X] = otr_val * 10;}
#define _OTR_SetSHM_Y(data, otr_val) { data[OTR_Y] = otr_val * 10;}
#define _OTR_SetSHM_Z(data, otr_val) { data[OTR_Z] = otr_val * 10;}


#ifdef __cplusplus
}
#endif


#endif // OPENTRACKCLIENT_PRIVATE_H
