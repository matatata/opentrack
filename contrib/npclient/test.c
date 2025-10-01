

#include "tirtypes.h"


#include <stdio.h>
#include <windows.h>


typedef int (__stdcall *f_funci)(tir_data_t *ft);
typedef int (__stdcall *f_reg_profle_id)(short id);

int NP_GetData(tir_data_t * data);

int main(int argc,char** argv) {

#if _WIN64
#define DLLNAME "NPClient64.dll"
#else
#define DLLNAME "NPClient.dll"
#endif

    f_funci funci  = NULL;
#ifndef LINK_DEP

    printf("loading the library at runtime %s\n",DLLNAME);

    HINSTANCE hGetProcIDDLL = LoadLibrary(DLLNAME);
    if (hGetProcIDDLL) {
        funci = (f_funci)GetProcAddress(hGetProcIDDLL, "NP_GetData");
        f_reg_profle_id register_profile = (f_reg_profle_id)GetProcAddress(hGetProcIDDLL, "NP_RegisterProgramProfileID");
        register_profile(1901); // Falcon BMS
    }
    else {
        printf("failed to LoadLibrary %s\n",DLLNAME);
        exit(1);
    }


    if(!funci)
    {
         printf("Could not get ProcAddress %s\n","NP_GetData");
         exit(1);
    }
#else
    funci = &NP_GetData;
#endif

    tir_data_t tir_data;

    while(1) {
        Sleep(250);
        npclient_status status = 0;

        status = funci(&tir_data);


        if(status!=NPCLIENT_STATUS_OK){
            printf("Openntrack is not tracking\n");
            // opentrack has transitioned from tracking to "not"-tracking
            break;
        }

        printf("%f %f %f %f %f %f\n",tir_data.tx,
                                    tir_data.ty,
                                    tir_data.tz,
                                    tir_data.yaw,
                                    tir_data.pitch,
                                    tir_data.roll);


    }



}
