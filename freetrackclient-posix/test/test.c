

#include "freetrackclient_posix.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LINK_DEPENDENT
#include <dlfcn.h>
#endif




int main(int argc,char** argv) {


#ifndef LINK_DEPENDENT

    importedFTXGetData getDataFunc  = NULL;
    importedFTXGetVersion getVersionFunc = NULL;
    importedFTXGetProvider getProviderFunc = NULL;

    #ifdef __APPLE__
        #define DLLNAME "libfreetrackclient.dylib"
    #else
        #if defined __i386__
        #define DLLNAME "libfreetrackclient.so"
        #else
        #define DLLNAME "libfreetrackclient64.so"
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

    getDataFunc = dlsym(lib_handle, "FTXGetData");
    if(getDataFunc == NULL){
        fprintf(stderr,"Unable to dlsym FTXGetData : %s\n",dlerror());
        exit(EXIT_FAILURE);
    }
    getVersionFunc = dlsym(lib_handle, "FTXGetVersion");
    if(getVersionFunc == NULL){
        fprintf(stderr,"Unable to dlsym FTXGetVersion : %s\n",dlerror());
        exit(EXIT_FAILURE);
    }

    getProviderFunc = dlsym(lib_handle, "FTXGetProvider");
    if(getProviderFunc == NULL){
        fprintf(stderr,"Unable to dlsym FTXGeProvider : %s\n",dlerror());
        exit(EXIT_FAILURE);
    }


    printf("Test: opentrack client version %s (%s)\n",getVersionFunc(),getProviderFunc());

#else

    printf("Test: opentrack client version %s (%s)\n",FTXGetVersion(),FTXGetProvider());


#endif


    FTXData data;

    while(1) {
#ifdef LINK_DEPENDENT
        if(!FTXGetData(&data))
            break;
#else
        if(!getDataFunc(&data))
            break;
#endif


        printf("x=%f y=%f z=%f y=%f p=%f r=%f\n",data.X,
                                    data.Y,
                                    data.Z,
                                    data.Yaw,
                                    data.Pitch,
                                    data.Roll);

        usleep(1000*250);
    }


}
