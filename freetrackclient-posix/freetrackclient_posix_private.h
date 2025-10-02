/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 * https://github.com/matatata/opentrack
 *
 * Based in parts on the work of Stanislaw Halik:
 *
 * Copyright (c) 2013 Stanislaw Halik <sthalik@misaki.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
#ifndef FREETRACKCLIENT_POSIX_PRIVATE_H
#define FREETRACKCLIENT_POSIX_PRIVATE_H

/*
 * Please do not use this private API unless you really need to.
 * as it may change and thus break things in future. Instead rely on the public header:
 */
#include "freetrackclient.h"

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------SHM-------------------------
// For now this is make compatible with the origina proto-wine see proto-wine/wine-shm.h
// It's subject to change in future
#define FTX_SHM_NAME "facetracknoir-wine-shm"
#define FTX_MTX_NAME "facetracknoir-wine-mtx"

// angles are in radians
typedef enum  {
    FTX_X = 0, // positive move right
    FTX_Y, // positive move up
    FTX_Z, // positive move away
    FTX_YAW, // positive yaw to the left
    FTX_PITCH, // positive pitch up
    FTX_ROLL // positive roll to the left
} FTPosixAxis;



typedef double _ft_data[6];

typedef struct FTPosixSHM
{
    _ft_data data; // index with FTPosixAxis
    // compatibility to support freetrack and npclient under WINE jus like proto-wine does
    int gameid, gameid2;
    unsigned char table[8];
    // ---
    bool stop;
} FTPosixSHM;


//-------------------SHM-------------------------


void FTX_Priv_Lock(void);

// when accessing the shm put in in between Lock() and Unlock()
FTPosixSHM *FTX_Priv_GetSHM(void);

void FTX_Priv_Unlock(void);

#ifdef __cplusplus
}
#endif


#endif
