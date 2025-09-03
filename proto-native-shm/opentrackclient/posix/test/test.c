

#include "opentrackclient_public.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LINK_DEPENDENT
#include <dlfcn.h>
#endif



typedef bool (*f_funci)(OTRHeadpose *ft);

int main(int argc,char** argv) {

f_funci funci  = NULL;

#ifdef LINK_DEPENDENT

funci = &OTRGetHeadpose;

#else
    #ifdef __APPLE__
        #define DLLNAME ".lib//libopentrackclient.dylib"
    #else
        #if defined __i386__
        #define DLLNAME "./lib/libopentrackclient.so"
        #else
        #define DLLNAME "./lib/libopentrackclient64.so"
        #endif
    #endif


    char *lib_name = DLLNAME;
    void *lib_handle = dlopen(lib_name, RTLD_NOW);
    if (lib_handle) {
        fprintf(stderr,"dlopen(\"%s\", RTLD_NOW): Successful\n", lib_name);
    }
    else {
        fprintf(stderr,"Unable to open library: %s\n",dlerror());
        exit(EXIT_FAILURE);
    }

    funci = dlsym(lib_handle, "OTRGetHeadpose");
    if(funci == NULL){
        fprintf(stderr,"Unable to dlsym OTRGetHeadpose : %s\n",dlerror());
        exit(EXIT_FAILURE);
    }
#endif

    OTRHeadpose data;

    while(1) {
        if(!funci(&data))
            break;


        printf("x=%f y=%f z=%f y=%f p=%f r=%f\n",data.X,
                                    data.Y,
                                    data.Z,
                                    data.Yaw,
                                    data.Pitch,
                                    data.Roll);

        usleep(1000*250);
    }



}
