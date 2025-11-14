
#include <windows.h>
#include <stdio.h>

#define MESSAGE "You've loaded opentrack wine-bridge's fake dll. This most likely means\n" \
"that the coreesponding builtin wine dll is not loaded or not found. Did you install wine-bridge into your wine installation? \n" \
"Try reinstalling Opentrack's builtin wine dlls.\n\n" \
"If instead it is your intention to load a windows-native dll,\n" \
"don't load those dlls and/or update the registry to point to the intended directory using your tool of choice.\n" \


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if(fdwReason==DLL_PROCESS_ATTACH){
        fprintf(stderr,"%s\n",MESSAGE);
        MessageBox(0,MESSAGE, "Fake-Dll", 0);
    }
    return FALSE;
}
