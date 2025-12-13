

#include "../otrclient.h"

#include <stdio.h>
#include <windows.h>



typedef int (__stdcall *f_funci)(FTHeap *ft);

int main(int argc,char** argv) {

#if _WIN64
#define DLLNAME "otrclient64.dll"
#else
#define DLLNAME "otrclient.dll"
#endif

    f_funci funci  = NULL;

    printf("Test: Trying to load %s at runtime\n",DLLNAME);

    HINSTANCE hGetProcIDDLL = LoadLibrary(DLLNAME);
    if (hGetProcIDDLL) {
        funci = (f_funci)GetProcAddress(hGetProcIDDLL, "OTR_GetFTHeap");
    }
    else {
         printf("Could not LoadLibrary %s\n",DLLNAME);
         exit(1);
    }

    if(!funci)
    {
         printf("Could not get ProcAddress %s\n","FTGetData");
         exit(1);
    }


    FTHeap ftheap;

    while(1) {
        int ok = 0;

        Sleep(250);

        ok = funci(&ftheap);

        if(!ok){
            printf("Failure\n");
            break;
        }

        printf("%f %f %f %f %f %f\n",ftheap.data.X,
                                    ftheap.data.Y,
                                    ftheap.data.Z,
                                    ftheap.data.Yaw,
                                    ftheap.data.Pitch,
                                    ftheap.data.Roll);

    }



}
