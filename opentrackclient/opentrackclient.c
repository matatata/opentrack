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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include "opentrackclient-private.h"
#include "shm.h"

#if defined (__APPLE__)
#   include <CoreFoundation/CoreFoundation.h>
#elif defined(_WIN32)
#   include <winuser.h>
#elif defined(__linux__)
    // nothing equivalent really
#endif

#define EXPORT __attribute__((visibility("default")))

// #define DEBUG 1
#if DEBUG
#define dbg_report(...) { printf(__VA_ARGS__); }
#else
#define dbg_report(...) { if(verbose) printf(__VA_ARGS__); }
#endif

#ifndef OPENTRACKCLIENT_VERSION
#define OPENTRACKCLIENT_VERSION "0.0.0"
#endif

static bool verbose=false;
static shm_wrapper_t *wrapper = 0;

EXPORT const char* OTRGetVersion(void){
   return OPENTRACKCLIENT_VERSION;
}

EXPORT const char* OTRGetProvider(void){
   return "Opentrack";
}


EXPORT bool OTRGetData(OTRHeadpose *data)
{
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    if(shm_posix){
        data->Yaw = _OTR_GetFT_Yaw(shm_posix->data);
        data->Pitch = _OTR_GetFT_Pitch(shm_posix->data);
        data->Roll = _OTR_GetFT_Roll(shm_posix->data);
        data->X = _OTR_GetFT_X(shm_posix->data);
        data->Y = _OTR_GetFT_Y(shm_posix->data);
        data->Z = _OTR_GetFT_Z(shm_posix->data);
    }
    else {
        dbg_report("no shm?\n");
    }
    FTX_Priv_Unlock();
    return true;
}

EXPORT bool OTRGetNPData(OTRHeadpose *npdata)
{
    FTX_Priv_Lock();
    FTPosixSHM *shm_posix = FTX_Priv_GetSHM();
    if(shm_posix){
        npdata->Yaw = _OTR_GetNP_Yaw(shm_posix->data);
        npdata->Pitch = _OTR_GetNP_Pitch(shm_posix->data);
        npdata->Roll = _OTR_GetNP_Roll(shm_posix->data);
        npdata->X = _OTR_GetNP_X(shm_posix->data);
        npdata->Y = _OTR_GetNP_Y(shm_posix->data);
        npdata->Z = _OTR_GetNP_Z(shm_posix->data);
    }
    else {
        dbg_report("no shm?\n");
    }
    FTX_Priv_Unlock();
    return true;
}

#pragma mark - "Priv"


/*EXPORT */ bool FTX_Priv_Init(void) {
    dbg_report("FTXInit called()\n");
    if(wrapper == NULL) {
        wrapper = malloc(sizeof(shm_wrapper_t));
        if(!shm_wrapper_init(wrapper,FTX_SHM_NAME, FTX_MTX_NAME, sizeof(FTPosixSHM),true))
            return false;
    }
    return wrapper != NULL;
}

/*EXPORT */ void FTX_Priv_Close(void) {
    dbg_report("FTXClose called()\n");
    if(wrapper!=NULL) {
        shm_wrapper_destroy(wrapper);
        free(wrapper);
    }
    wrapper = NULL;
}

EXPORT void FTX_Priv_Lock(void)
{
    shm_wrapper_lock(wrapper,true/*shared*/);
}

EXPORT void FTX_Priv_Unlock(void)
{
    shm_wrapper_unlock(wrapper);
}

EXPORT FTPosixSHM *FTX_Priv_GetSHM(void)
{
    return (FTPosixSHM*)wrapper->mem;
}

EXPORT void OTR_Priv_NotifyUser(const char* title,const char* message,unsigned long timeout_secs)
{
#if defined (__APPLE__)
    CFStringRef titleStr = CFStringCreateWithCString(NULL, title,kCFStringEncodingUTF8);
    CFStringRef messageStr = CFStringCreateWithCString(NULL, message,kCFStringEncodingUTF8);
    CFUserNotificationDisplayNotice (
        timeout_secs, // Timeout, in seconds (use 0 for no timeout)
        kCFUserNotificationCautionAlertLevel,
        NULL,
        NULL,
        NULL,
        titleStr,
        messageStr,
        CFSTR("OK")
    );
    // clean up:
    CFRelease(titleStr);
    CFRelease(messageStr);
#elif defined(_WIN32)
    MessageBoxA(0, title, message, 0);
#elif defined(__linux__)
    fprintf(stderr,"%s: %s\n",title,message);
#endif
}

__attribute__((constructor))
static void initializer(void) {
    verbose = getenv("OTR_VERBOSE")!=NULL;
    dbg_report("opentrackclient (Posix) %s\n",OPENTRACKCLIENT_VERSION);
    FTX_Priv_Init();
}

__attribute__((destructor))
static void finalizer(void) {
    dbg_report("opentrackclient library: finalizer called\n");
    FTX_Priv_Close();
}
