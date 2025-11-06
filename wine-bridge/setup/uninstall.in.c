
// x86_64-w64-mingw32-gcc uninstall.c -o "Opentrack-Remove.exe"

#include <windows.h>
#include <stdio.h>
int main(int argc, const char** argv){
    return system("start \"Opentrack Uninstall\" cmd /c call \"@opentrack-install-winebridge-bin-winepath-quoted@\\uninstall.bat\"");
}


