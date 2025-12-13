#include "app.h"

#define PREFS_REGISTRY_KEY_BASE "Software\\Opentrack-Wine-Bridge"

Log *logger=NULL;
Bridge* bridge = NULL;
WineBridgeApp app;
WineBridgeSettings settings;

const char WineBridgeApp::CLASS_NAME[] = "WineBridgeApp";

void WineBridgeSettings::ApplyProtoSelection(int newProto){
    stringstream output;
    if(proto != newProto) {
        WritePaths(newProto,output);
        logger->log(output.str());

        if(newProto == 0) {// was runnign need to stop now
            bridge->stop();
        }
        else if(proto == 0){ // was not runnong, must run now
            bridge->runAsync();
        }

    }
    proto = newProto;

    app.RedrawStatusText();
}

void WineBridgeSettings::Revert(){
    int oldProto=proto;
    GetCurrentSettings();

    logger->logln("Settings reverted");

    if(proto == 0) {// was runnign need to stop now
        bridge->stop();
    }
    else if(oldProto == 0){ // was not runnong, must run now
        bridge->runAsync();
    }

    app.RedrawStatusText();

}

int WineBridgeSettings::GetProtocols() {
    int index = SendDlgItemMessage(hwndDlg, ID_PROTO, CB_GETCURSEL, 0, 0);
    if (index == CB_ERR) {
        return -1;
    }
    return index;
}



void WineBridgeSettings::WritePaths(int protocol,ostream &output){

    string dllpath = WINEBRIDGE_BIN_PATH; // getDllPath()

    switch (protocol)
    {
    case 0:
        clearBothProtocols(output,output);
        break;
    case 1:
        clearNPClient(output,output);
        setFreetrackClientPath(dllpath,output,output);
        break;
    case 2:
        clearFTClient(output,output);
        setNPClientPath(dllpath,output,output);
        break;
    case 3:
        setFreetrackClientPath(dllpath,output,output);
        setNPClientPath(dllpath,output,output);
        break;
    default:
        break;
    }
}



void WineBridgeSettings::GetCurrentSettings(){
    DWORD readVal=0;
    if(Registry::ReadDwordValue(PREFS_REGISTRY_KEY_BASE,"fps",readVal)){
        fps = (int)readVal;
        if(fps <= 0) {
            fps = 25;
        }
    }
    else {
        fps = 25;
    }
    if(Registry::ReadDwordValue(PREFS_REGISTRY_KEY_BASE,"disableAllOnExit",readVal)){
        disable_on_close = (BOOL)readVal;
    }
    else {
        disable_on_close = FALSE;
    }

    int prefProto=-1;

    if(Registry::ReadDwordValue(PREFS_REGISTRY_KEY_BASE,"proto",readVal)){
        prefProto = (int)readVal;
    }
    else {
        prefProto = 3;
    }

    stringstream output;
    printCurrentSettings(output);
    WritePaths(prefProto,output);
    logger->log(output.str());

    proto = prefProto;
}


void WineBridgeSettings::UpdateGui() {
    if(proto != -1) {
        SendMessage(GetDlgItem(hwndDlg, ID_PROTO), CB_SETCURSEL, (WPARAM)proto, 0);
    }
    SetWindowText( GetDlgItem(hwndDlg,ID_FPS), to_string(fps).c_str());
    SendMessage(GetDlgItem(hwndDlg, ID_DISABLE_ON_CLOSE), BM_SETCHECK, disable_on_close ? BST_CHECKED : BST_UNCHECKED, 0);
}


void WineBridgeSettings::CenterDialogOnOwner()
{
    HWND hwndOwner;
    if ((hwndOwner = GetParent(hwndDlg)) == NULL)
    {
        hwndOwner = GetDesktopWindow();
    }

    RECT rc,rcOwner,rcDlg;

    GetWindowRect(hwndOwner, &rcOwner);
    GetWindowRect(hwndDlg, &rcDlg);
    CopyRect(&rc, &rcOwner);
    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
    OffsetRect(&rc, -rc.left, -rc.top);
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);
    SetWindowPos(hwndDlg,
                HWND_TOP,
                rcOwner.left + (rc.right / 2),
                rcOwner.top + (rc.bottom / 2),
                0, 0,          // Ignores size arguments.
                SWP_NOSIZE);
}

int WineBridgeSettings::WriteSettings(){

    stringstream output;

    if (proto == -1) {
        return -1;
    }

    Registry::WriteDwordValue(PREFS_REGISTRY_KEY_BASE,"fps",bridge->getFPS());
    Registry::WriteDwordValue(PREFS_REGISTRY_KEY_BASE,"disableAllOnExit",disable_on_close);
    Registry::WriteDwordValue(PREFS_REGISTRY_KEY_BASE,"proto",proto);

    UpdateGui();
    return 0;
}


void WineBridgeApp::RedrawStatusText(){
    string protoText = "None";
    switch(settings.proto) {
        case 1: protoText = "FreeTrack"; break;
        case 2: protoText = "TrackIR"; break;
        case 3: protoText = "FreeTrack and TrackIR"; break;
    }

    statusText = "Protocol: " + protoText;

    SetDlgItemText(mainHwnd,ID_STATUS1, statusText.c_str());
}

void WineBridgeSettings::InitDialog(HWND hwnd){
    hwndDlg = hwnd;
    HWND hCombo = GetDlgItem(hwnd, ID_PROTO);
    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"None");
    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"FreeTrack");
    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"TrackIR");
    SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Both (recommended)");

    GetCurrentSettings();

    UpdateGui();
    CenterDialogOnOwner();
}



void WineBridgeApp::InitWindow(HWND hwnd, HINSTANCE hInstance){
    mainHwnd = hwnd;
}


void PlaceInCenterOfScreen(HWND window, DWORD style, DWORD exStyle,WORD dpi) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    RECT clientRect;
    GetClientRect(window, &clientRect);
    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    clientWidth = MulDiv(clientWidth, dpi, 96);
    clientHeight = MulDiv(clientHeight, dpi, 96);

    SetWindowPos(window, NULL, screenWidth / 2 - clientWidth / 2, screenHeight / 2 - clientHeight / 2, 0,0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(window,0,0,0,clientWidth,clientHeight,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);


}

