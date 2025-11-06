/* Copyright (c) 2025 Matteo Ceruti <matteo.ceruti@gmail.com>
 * https://github.com/matatata/opentrack
 *
 * This library facilitates loading opentrackclient's exported functions dynamically.
 * This is useful, because you probably will not want to link to opentrackclient since
 * users might not have installed it. Instead this loader tries to load it via dlload.
 * On macOS we can do this by weak-linking the framework. On Linux similar techniqes exists,
 * but I did not manage to get it to work properly.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */


#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#ifndef OTR_OPTIONAL
#error "You should have defined OTR_OPTIONAL when intending to use the loader"
//#define OTR_OPTIONAL
#endif
#include "opentrackclient/opentrackclient.h"

bool (*importedOTRGetData)(OTRHeadpose*) = NULL;
bool (*importedOTRGetNPData)(OTRHeadpose*) = NULL;
const char* (*importedOTRGetVersion)(void) = NULL;
const char* (*importedOTRGetProvider)(void) = NULL;

bool verbose = false;
void *lib_handle = NULL;


void *load_symbol(void *lib_handle,const char* symbol_name){
    void *symbol = dlsym(lib_handle, symbol_name);
    if(symbol == NULL){
        fprintf(stderr,"Unable to dlsym %s : %s\n",symbol_name,dlerror());
        return NULL;
    }
    return symbol;
}

static const char* dlopen_prefixes[] = {
    "", // first let rpath and LD_LIBRARY_PATH DYLD_LIBRARY_PATH do it's magic
#if __APPLE__
    "/Library/Frameworks/opentrackclient.framework"
#else
    "/opt/opentrack/lib"
#endif
    ,NULL // NULL terminate
};


#if __APPLE__
// framework
#define LIB_FILE_NAME "opentrackclient"
#else
#if defined __i386__
#define LIB_FILE_NAME "libopentrackclient.so"
#else
#define LIB_FILE_NAME "libopentrackclient64.so"
#endif
#endif

#define OTRGetDataSymbol "OTRGetData"
#define OTRGetNPDataSymbol "OTRGetNPData"
#define OTRGetVersionSymbol "OTRGetVersion"
#define OTRGetProviderSymbol "OTRGetProvider"

__attribute__((constructor))
static void initializer(void) {
    const char* path = LIB_FILE_NAME;

    verbose = getenv("OTR_VERBOSE")!=NULL;

    for(int i=0; dlopen_prefixes[i]!=NULL; i++){
        char loc[PATH_MAX]={0};
        const char *prefix = dlopen_prefixes[i];
        int prefix_len = strlen(prefix);

        if(prefix_len>0){
            strcat(loc,prefix);
            if(prefix[prefix_len-1] != '/')
                strcat(loc,"/");
        }
        strcat(loc,LIB_FILE_NAME);
        if(verbose) printf("opentrackclient-loader: Trying %s\n",loc);
        lib_handle = dlopen(loc, RTLD_NOW);
        if(lib_handle!=NULL)
            break;
    }

    if (!lib_handle) {
        if(verbose) fprintf(stderr,"opentrackclient-loader: Unable to open library %s error: %s\n",LIB_FILE_NAME,dlerror());
        return;
    }

    OTRGetData = load_symbol(lib_handle, OTRGetDataSymbol);
    OTRGetNPData = load_symbol(lib_handle, OTRGetNPDataSymbol);
    OTRGetVersion = load_symbol(lib_handle, OTRGetVersionSymbol);
    OTRGetProvider = load_symbol(lib_handle, OTRGetProviderSymbol);

    if( OTRGetData != NULL && OTRGetNPData != NULL &&  OTRGetVersion != NULL && OTRGetProvider != NULL){
        if(verbose) printf("opentrackclient-loader: opentrackclient loaded version %s (%s)\n",OTRGetVersion(),OTRGetProvider());
    }
    else {
        fprintf(stderr,"opentrackclient-loader: opentrackclient incomplete or not compatible");
    }
}

__attribute__((destructor))
static void finalizer(void) {
    if(lib_handle) dlclose(lib_handle);
}


