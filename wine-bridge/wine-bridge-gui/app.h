#pragma once

#include <windows.h>
#include "resource.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "bridge.h"
#include "defs.h"
#include "wine-bridge/ftnoir-registry-tool/tool.h"
#include "wine-bridge/ftnoir-registry-tool/registry.h"

using namespace std;
using namespace ftnoir_registry_tool;

// WINDOW
#define ID_TEXTFIELD (WM_USER + 667)
#define IDB_CONFIGURE (WM_USER + 666)


struct WineBridgeApp {

    static const char CLASS_NAME[];

    void InitWindow(HWND hwnd, HINSTANCE hInstance);
    int MakeWindow(HINSTANCE hInstance);
    void Paint();
    void RedrawStatusText();

    HWND mainHwnd;
    HBITMAP hBitmap = NULL;
    HICON hicon = NULL;

    string bridgeStatus="";
    string statusText="";
};

struct WineBridgeSettings {
    void ApplyProtoSelection(int newProto);
    void Revert();
    int GetProtocols();
    void WritePaths(int protocol,ostream &output);
    void GetCurrentSettings();
    void UpdateGui();
    void CenterDialogOnOwner();
    int WriteSettings();
    void InitDialog(HWND hwnd);


    HWND hwndDlg = NULL;
    int proto = 0;
    int fps=25;
    BOOL disable_on_close=false;

};

extern Log *logger;
extern Bridge* bridge;


extern WineBridgeApp app;
extern WineBridgeSettings settings;
