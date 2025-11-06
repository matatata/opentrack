
// x86_64-w64-mingw32-gcc install.c -o "Opentrack-Install.exe"

#include <windows.h>
#include <stdio.h>
int main(int argc, const char** argv){
    return system("start \"Opentrack Install\" cmd /c call \"@opentrack-install-winebridge-bin-winepath-quoted@\\install.bat\"");
}


