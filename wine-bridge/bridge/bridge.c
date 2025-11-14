

#include "freetrackclient/fttypes.h"
#include <stdio.h>
#include <windows.h>
#include "compat/shm.h"
#include "wine-bridge/wine-builtin-dlls/otrclient.h"


#include "compat/shm.cx"

BOOL running = TRUE;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        running = false;
    }
    return TRUE;
}



int main(int argc,char** argv) {
    int fps = 25;

    if (argc==3 && (strcmp(argv[1],"--fps")==0)) {
        fps = atoi(argv[2]);
        if(fps<= 0) {
            fprintf(stderr,"bad fps %s\n",argv[2]);
            return 1;
        }
    }

    int frame_time = 1000/fps;

    printf("FPS=%i, frame_time=%i millis\n",fps,frame_time);

#if _WIN64
#define DLLNAME "otrclient64.dll"
#else
#define DLLNAME "otrclient.dll"
#endif

    f_otr_GetFTData otr_GetFTData  = NULL;

    HINSTANCE hGetProcIDDLL = LoadLibrary(OTRCLIENT_DLLNAME);
    if (hGetProcIDDLL) {
        otr_GetFTData = (f_otr_GetFTData)GetProcAddress(hGetProcIDDLL, OTR_GET_FT_HEAP_FUNC_NAME);
    }
    else {
         fprintf(stderr,"Could not LoadLibrary %s\n",OTRCLIENT_DLLNAME);
         return 1;
    }

    if(!otr_GetFTData)
    {
         fprintf(stderr,"Could not get ProcAddress %s\n",OTR_GET_FT_HEAP_FUNC_NAME);
         return 1;
    }

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
        printf("\nERROR: Could not set control handler");
        return 1;
    }

    shm_wrapper_t *shm_wrapper = malloc(sizeof(shm_wrapper_t));
    if(!shm_wrapper_init(shm_wrapper,FREETRACK_HEAP,FREETRACK_MUTEX,sizeof(FTHeap),false)){
        fprintf(stderr,"Failed to init shm\n");
        return 1;
    }

    FTHeap *ftheap = (FTHeap*)shm_wrapper->mem;

    bool stopped = false;
    while(running) {
        int tracking = 0;

        Sleep(frame_time);

        tracking = otr_GetFTData(ftheap);

        if(!tracking && !stopped){
            printf("Tracking stopped\n");
            stopped = true;
            Sleep(1000);
            // break;
        }

        if(stopped && tracking) {
            printf("Tracking continued\n");
            stopped = false;
        }

        // printf("%f %f %f %f %f %f\n",ftheap->data.X,
        //                             ftheap->data.Y,
        //                             ftheap->data.Z,
        //                             ftheap->data.Yaw,
        //                             ftheap->data.Pitch,
        //                             ftheap->data.Roll);
    }


    if(shm_wrapper) {
        shm_wrapper_destroy(shm_wrapper);
        free(shm_wrapper);
    }

    return 0;
}

