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
#include <stdio.h>
#include <stdbool.h>

#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "freetrackclient_posix_private.h"
#include "shm.h"

#define EXPORT __attribute__((visibility("default")))

// #define DEBUG 1
#if DEBUG
#define dbg_report(...) { printf(__VA_ARGS__); }
#else
#define dbg_report(...)
#endif

#ifndef LIBFREETRACKCLIENT_VERSION
#define LIBFREETRACKCLIENT_VERSION "0.0.0"
#endif

static shm_wrapper *wrapper = 0;

EXPORT const char* FTXGetVersion(void){
   return LIBFREETRACKCLIENT_VERSION;
}

EXPORT const char* FTXGetProvider(void){
   return "Opentrack";
}

// TODO use atomic instead of (b)locking
// #include <stdatomic.h>

EXPORT bool FTXGetData(FTXData *headpose)
{
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    if(shm_posix){
        FTXData* data = headpose;
        data->Yaw = shm_posix->data.Yaw;
        data->Pitch = shm_posix->data.Pitch;
        data->Roll = shm_posix->data.Roll;
        data->X = shm_posix->data.X;
        data->Y = shm_posix->data.Y;
        data->Z = shm_posix->data.Z;
    }
    else {
        dbg_report("oho\n");
    }
    FTX_Priv_Unlock();
    return true;
}


/*EXPORT */ bool FTX_Priv_Init(void) {
    dbg_report("FTXInit called()\n");
    if(wrapper == NULL)
        wrapper = shm_wrapper_init(FTX_SHM_NAME, FTX_MTX_NAME, sizeof(FTPosixSHM));
    return wrapper != NULL;
}

/*EXPORT */ void FTX_Priv_Close(void) {
    dbg_report("FTXClose called()\n");
    if(wrapper!=NULL)
        shm_wrapper_free(wrapper);
    wrapper = NULL;
}

EXPORT void FTX_Priv_Lock(void)
{
    shm_wrapper_lock(wrapper);
}

EXPORT void FTX_Priv_Unlock(void)
{
    shm_wrapper_unlock(wrapper);
}

EXPORT FTPosixSHM *FTX_Priv_GetSHM(void)
{
    return (FTPosixSHM*)wrapper->mem;
}


__attribute__((constructor))
static void initializer(void) {
    dbg_report("FreeTrackClient (Posix) %s\n",LIBFREETRACKCLIENT_VERSION);
    FTX_Priv_Init();
}

__attribute__((destructor))
static void finalizer(void) {
    dbg_report("opentrackclient library: finalizer called\n");
    FTX_Priv_Close();
}

