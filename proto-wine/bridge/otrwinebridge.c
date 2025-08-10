#include "../opentrack-wrapper-wine.h"
#include "resource.h"
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#define SETTINGS_HKEY_PATH "Software\\OpentrackWineBridge"
#define SETTING_CLEAR_REG_KEYS_ON_EXIT "clearRegistryKeysOnExit"
#define SETTING_PROTOCOL "protocol"
#define SETTING_CUSTOM_DLL_CHECKED "customDllChecked"
#define SETTING_CUSTOM_DLL_LOC "customDllLoc"

bool clearRegistryKeysOnExit = false;
bool customDllChecked = false;


#define IDB_CONFIGURE 666

// HWND hEdit;
HWND dialogHwnd;
HWND mainHwnd;
HBITMAP hBitmap;
HMENU hMenu;

char customDllLoc[MAX_PATH];


bool readRegKey(char * key,char* subkey,int type,void *valPtr,DWORD bufSize){
    bool ok = false;
    HKEY hkpath=NULL;
    if (RegCreateKeyExA(HKEY_CURRENT_USER,
                        key,
                        0,
                        NULL,
                        0,
                        KEY_ALL_ACCESS,
                        NULL,
                        &hkpath,
                        NULL) == ERROR_SUCCESS)
    {
        switch(type){
            case REG_SZ: //REG_SZ:
                DWORD dataSize=0;
                if (ERROR_SUCCESS == RegQueryValueExA(hkpath,  subkey,  NULL, NULL,NULL, &dataSize)) {
                    if(dataSize <= bufSize &&
                        ERROR_SUCCESS == RegQueryValueExA(hkpath,  subkey, NULL,NULL, valPtr, &dataSize)) {
                        ok = true;
                    }
                    else {
                        fprintf(stderr,"String too long\n");
                    }
                }
                else {
                    fprintf(stderr,"Error reading\n");
                }
            break;
            case REG_DWORD: {
                DWORD val;
                DWORD dataSize = sizeof(val);
                if (ERROR_SUCCESS == RegQueryValueExA(hkpath,  subkey, NULL, NULL, (LPBYTE)&val, &dataSize)) {
                    *((DWORD*)valPtr) = val;
                    ok = true;
                    // fprintf(stderr,"ok got value\n");
                }
                else {
                    fprintf(stderr,"Error reading\n");
                }
            }
            break;
        }

        RegCloseKey(hkpath);
    }
    return ok;
}

void writeRegKey(char * key,char* subkey,int type,void *valPtr){
    HKEY hkpath=NULL;
    char* strVal=(char*)valPtr;

    if (RegCreateKeyExA(HKEY_CURRENT_USER,
                        key,
                        0,
                        NULL,
                        0,
                        KEY_ALL_ACCESS,
                        NULL,
                        &hkpath,
                        NULL) == ERROR_SUCCESS)
    {
        switch(type){
            case REG_SZ: {
                RegSetValueExA(hkpath, subkey, 0, type, (BYTE*) valPtr, strlen(strVal) + 1);
            }break;
            case REG_DWORD:
                RegSetValueExA(hkpath, subkey, 0, type, (BYTE*) valPtr, sizeof(DWORD));
            break;
        }

        RegCloseKey(hkpath);
    }
}

bool readRegInt(char * key,char* subkey,DWORD *val){
    return readRegKey(key,subkey,REG_DWORD,val,sizeof(DWORD));
}

void writeRegInt(char * key,char* subkey,DWORD value){
    writeRegKey(key,subkey,REG_DWORD,&value);
}

bool readRegString(char * key,char* subkey,char *value,int bufSize){
    return readRegKey(key,subkey,REG_SZ,value,bufSize);
}

void writeRegString(char * key,char* subkey,void *value){
    writeRegKey(key,subkey,REG_SZ,value);
}


DWORD getWineProtocol(){
    DWORD protocol = 0;
    if(!readRegInt(SETTINGS_HKEY_PATH,SETTING_PROTOCOL,&protocol)){
        fprintf(stderr,"cannot read protocol from registry\n");
    }

    if(protocol == 0) protocol = 3; // both is default
    return protocol;
}


DWORD WINAPI ThreadFunction(LPVOID lpParam) {

    mainloop(getWineProtocol());

    if(clearRegistryKeysOnExit) {
        clearRegistryKeys();
    }
    return 0;
}

void CenterDialogOnOwner(HWND hwndDlg)
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


void UpdateCustomDllLoc(HWND hwndDlg){
    char dllLoc[MAX_PATH];
    SendMessage(GetDlgItem(hwndDlg, IDCK_DLL_OVERRIDE), BM_SETCHECK, customDllChecked ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(hwndDlg, IDC_DLL_LOC), EM_SETREADONLY, ! customDllChecked, 0);

    if(customDllChecked)
    {
        SetWindowText( GetDlgItem(hwndDlg,IDC_DLL_LOC), customDllLoc);
    }
    else if(getDllPath(dllLoc,sizeof(dllLoc)))
    {
        SetWindowText(GetDlgItem(hwndDlg,IDC_DLL_LOC), dllLoc);
    }
}

// Dialog box procedure
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwndDlg, GWLP_HINSTANCE);

    switch (uMsg) {
        case WM_INITDIALOG: {
            // Add items to combo box
            HWND hCombo = GetDlgItem(hwndDlg, IDC_COMBO1);
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Freetrack");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"NPCLient");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Both");

            DWORD protocol = getWineProtocol();
            SendMessage(hCombo, CB_SETCURSEL, (WPARAM)protocol-1, 0);

            // init checkbox
            if(!readRegInt(SETTINGS_HKEY_PATH,SETTING_CLEAR_REG_KEYS_ON_EXIT,(DWORD *)&clearRegistryKeysOnExit)){
                fprintf(stderr,"could not read checkbox from registry");
            }
            SendMessage(GetDlgItem(hwndDlg, IDCK_REMOVE), BM_SETCHECK, clearRegistryKeysOnExit ? BST_CHECKED : BST_UNCHECKED, 0);

            // init dll location
            readRegString(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_LOC,customDllLoc,sizeof(customDllLoc));
            readRegInt(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_CHECKED,(DWORD *)&customDllChecked);

            UpdateCustomDllLoc(hwndDlg);

            CenterDialogOnOwner(hwndDlg);
            return TRUE;
        }
        case WM_COMMAND:
        {
            if(LOWORD(wParam) == IDC_DLL_LOC){
                if(HIWORD(wParam) == EN_CHANGE){
                    if(customDllChecked){
                        GetWindowText(GetDlgItem(hwndDlg,IDC_DLL_LOC), customDllLoc, sizeof(customDllLoc));
                    }
                }
            }

            if(LOWORD(wParam) == IDCK_REMOVE){
                clearRegistryKeysOnExit = IsDlgButtonChecked(hwndDlg, IDCK_REMOVE);
            }

            if(LOWORD(wParam) == IDCK_DLL_OVERRIDE){
                customDllChecked = IsDlgButtonChecked(hwndDlg, IDCK_DLL_OVERRIDE);
                UpdateCustomDllLoc(hwndDlg);
            }

            if(LOWORD(wParam) == IDOK)
            {
                writeRegInt(SETTINGS_HKEY_PATH,SETTING_CLEAR_REG_KEYS_ON_EXIT,clearRegistryKeysOnExit);
                writeRegInt(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_CHECKED,customDllChecked);

                int index = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
                if (index != CB_ERR) {
                    int protocol = index+1;
                    writeRegInt(SETTINGS_HKEY_PATH,SETTING_PROTOCOL,protocol);
                    createRegistryKeys(protocol,protocol);
                }

                writeRegString(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_LOC,customDllLoc);
                EndDialog(hwndDlg, 0);
            }

            if(LOWORD(wParam) == IDCANCEL)
            {
                 EndDialog(hwndDlg, 0);
            }
        }
        break;
        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            hwndDlg = NULL;
        break;
        default:
            return DefWindowProc(hwndDlg, uMsg, wParam, lParam);

    }
    return TRUE;
}




LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
      HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    switch (uMsg) {
        case WM_DESTROY:
             PostQuitMessage(0);
             DeleteObject(hBitmap);
             hBitmap = NULL;
             DestroyMenu(hMenu);
        break;
        case WM_CREATE:
            hBitmap =  LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1));

            RECT wndRect;
            GetWindowRect(hwnd, &wndRect);

            HWND hwndButton = CreateWindow(
                    "BUTTON",  // Predefined class; Unicode assumed
                    "Options",      // Button text
                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                    (wndRect.right - wndRect.left) / 2 - 50 ,         // x position
                    (wndRect.bottom - wndRect.top) / 2 + 10,         // y position
                    100,        // Button width
                    20,        // Button height
                    hwnd,     // Parent window
                    (HMENU)IDB_CONFIGURE,       // id.
                    hInstance,
                    NULL);      // Pointer not needed.

        break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDB_CONFIGURE)
            {
                DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
            }
            break;
        break;


        case WM_PAINT:
            PAINTSTRUCT     ps;
            HDC             hdc;
            BITMAP          bitmap;
            HDC             hdcMem;
            HGDIOBJ         oldBitmap;

            hdc = BeginPaint(hwnd, &ps);
            hdcMem = CreateCompatibleDC(hdc);
            oldBitmap = SelectObject(hdcMem, hBitmap);

            GetObject(hBitmap, sizeof(bitmap), &bitmap);
            BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
            SelectObject(hdcMem, oldBitmap);
            DeleteDC(hdcMem);

            EndPaint(hwnd, &ps);
        break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "OtrWineBridgeClass";
    WNDCLASS wc = {0};

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
    wc.lpszClassName = CLASS_NAME;
    wc.lpszMenuName  = NULL;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;


    if (!RegisterClass(&wc)) {
        return 1;
    }

    mainHwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "OpentrackWineBridge",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        320, 193+25,
        NULL, NULL, hInstance, NULL
    );

    if (!mainHwnd) {
        return 1;
    }

    UpdateWindow(mainHwnd);
    PlaceInCenterOfScreen(mainHwnd,0,0);
    ShowWindow(mainHwnd, nCmdShow);


    HANDLE hThread = CreateThread(
        NULL,           // Default security attributes
        0,              // Default stack size
        ThreadFunction, // Thread function
        NULL,           // No parameters
        0,              // Default creation flags
        NULL            // No thread ID
    );



    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    // terminate the headpose loop
    quit();

    if (hThread != NULL) {
        // Wait for the thread to finish
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    return 0;
}


