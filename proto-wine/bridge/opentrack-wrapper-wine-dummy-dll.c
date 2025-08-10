#include "../opentrack-wrapper-wine.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>

static bool running = true;

#define LABEL "This is the dummy dll"

 __declspec(dllexport) void quit()
 {
     MessageBox(NULL, "Goodbye", LABEL, MB_OK);
    running = false;
 }

 __declspec(dllexport) int mainloop(int protocol){
    MessageBox(NULL, "Start Processing", LABEL, MB_OK);
    while(running){
        Sleep(1000);
    }
    return 0;
}


__declspec(dllexport)  void createRegistryKeys(int32_t x, int32_t selection) {
    MessageBox(NULL, "Writing keys", LABEL, MB_OK);
}

__declspec(dllexport)  void clearRegistryKeys() {
    MessageBox(NULL, "Clearing keys", LABEL, MB_OK);
}


__declspec(dllexport)  bool getDllPath(char *dir,unsigned int bufSize){
    unsigned int req = strlen("Hello");
    if(req < bufSize)
        strcpy(dir,"Hello");
}

