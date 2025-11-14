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
    stringstream output;
    DWORD readVal=0;
    if(Registry::ReadDwordValue(PREFS_REGISTRY_KEY_BASE,"fps",readVal)){
        fps = (int)readVal;
        if(fps <= 0) {
            output << "Bad fps val " << fps;
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

    if(isVerbose())
        printCurrentSettings(output);

    if(isVerbose())
        output << "Updating registry" << endl;

    WritePaths(prefProto,output);

    proto = prefProto;

    logger->log(output.str());
}


void WineBridgeSettings::UpdateGui() {
    if(proto != -1) {
        SendMessage(GetDlgItem(hwndDlg, ID_PROTO), CB_SETCURSEL, (WPARAM)proto, 0);
    }
    SetWindowText( GetDlgItem(hwndDlg,ID_FPS), to_string(fps).c_str());
    // SetCheckbox(hwndDlg, ID_DISABLE_ON_CLOSE, disable_on_close);
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
    // SendMessage(mainHwnd,WM_PAINT,0,0); does not work

    string protoText = "None";
    switch(settings.proto) {
        case 1: protoText = "FreeTrack"; break;
        case 2: protoText = "TrackIR"; break;
        case 3: protoText = "Both"; break;
    }

    statusText = "Bridge: " + bridgeStatus + "\nProto:  " + protoText;
    InvalidateRect(mainHwnd, NULL, TRUE);
    UpdateWindow(mainHwnd);
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

    hBitmap =  LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));

    RECT wndRect;
    GetWindowRect(hwnd, &wndRect);

    HWND hwndButton = CreateWindow(
            "BUTTON",  // Predefined class; Unicode assumed
            "Settings",      // Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            (wndRect.right - wndRect.left) / 2 - 50 ,         // x position
            150,// (wndRect.bottom - wndRect.top) / 2 + 10,         // y position
            100,        // Button width
            20,        // Button height
            hwnd,     // Parent window
            (HMENU)IDB_CONFIGURE,       // id.
            hInstance,
            NULL);      // Pointer not needed.
    HWND hwndEdit = CreateWindowEx(
            0, "EDIT",   // predefined class
            NULL,         // no window title
            WS_CHILD | WS_VISIBLE |  WS_VSCROLL | WS_HSCROLL
            | ES_READONLY |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 193, 315, 90,   // set size in WM_SIZE message
            hwnd,         // parent window
            (HMENU) ID_TEXTFIELD,   // edit control ID
            hInstance,
            NULL);        // pointer not needed

    SendMessage(hwndEdit,WM_SETFONT,(WPARAM)GetStockObject(DEFAULT_GUI_FONT),TRUE);
}

void WineBridgeApp::Paint()
{
    PAINTSTRUCT     ps;
    HDC             hdc;
    BITMAP          bitmap;
    HDC             hdcMem;
    HGDIOBJ         oldBitmap;

    hdc = BeginPaint(mainHwnd, &ps);
    hdcMem = CreateCompatibleDC(hdc);
    oldBitmap = SelectObject(hdcMem, hBitmap);

    GetObject(hBitmap, sizeof(bitmap), &bitmap);
    BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);

    // Status TEXT:
    RECT wndRect;
    GetWindowRect(mainHwnd, &wndRect);
    RECT rc = {3,4,250,100};
    SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    DrawText(hdc, statusText.c_str(),statusText.length(), &rc, 0);// DT_CENTER); //DT_CENTER | DT_VCENTER*//* | DT_SINGLELINE

    EndPaint(mainHwnd, &ps);
}

void PlaceInCenterOfScreen(HWND window, DWORD style, DWORD exStyle) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    RECT clientRect;
    GetClientRect(window, &clientRect);
    AdjustWindowRectEx(&clientRect, style, FALSE, exStyle);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;
    SetWindowPos(window, NULL, screenWidth / 2 - clientWidth / 2, screenHeight / 2 - clientHeight / 2, /*clientWidth, clientHeight, 0*/ 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

int WineBridgeApp::MakeWindow(HINSTANCE hInstance){

    mainHwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Opentrack Wine Bridge",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        320, 193+25 + 100,
        NULL, NULL, hInstance, NULL
    );

    if (!mainHwnd) {
        return 0;
    }

    UpdateWindow(mainHwnd);
    PlaceInCenterOfScreen(mainHwnd,0,0);
    return 1;
}
