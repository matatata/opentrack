
// x86_64-w64-mingw32-gcc start-registry-tool.c -o "Start-Registry-Tool.exe"

#include <windows.h>
#include <stdio.h>
int main(int argc, const char** argv){

//    return system("start \"\" cmd /c call \"@opentrack-install-winebridge-bin-winepath-quoted@\\registry-tool.bat\"");

#if defined(_WIN64)
    return system("start /wait \"\" \"@opentrack-install-winebridge-bin-winepath-quoted@\\ftnoir-registry-tool-gui64.exe\"");
#else
    return system("start /wait \"\" \"@opentrack-install-winebridge-bin-winepath-quoted@\\ftnoir-registry-tool-gui.exe\"");
#endif


}


