// #include "../opentrack-wrapper-wine.h"
#include "resource.h"
#include <windows.h>
#include <stdint.h>
#include <stdio.h>


#define NPCLIENT_LOCATION_REG_KEY "Software\\NaturalPoint\\NATURALPOINT\\NPClient Location"
#define FREETRACKCLIENT_LOCATION_REG_KEY "Software\\Freetrack\\FreeTrackClient"
#define PATH "Path"

#define FREETRACK_SELECTED(selection) ((selection & 1)==1)
#define NPCLIENT_SELECTED(selection) ((selection & 2)==2)

#define SETTINGS_HKEY_PATH "Software\\OpentrackWineBridge"
#define SETTING_CLEAR_REG_KEYS_ON_EXIT "clearRegistryKeysOnExit"
#define SETTING_PROTOCOL "protocol"
#define SETTING_CUSTOM_DLL_CHECKED "customDllChecked"
#define SETTING_CUSTOM_DLL_LOC "customDllLoc"

BOOL clearRegistryKeysOnExit = FALSE;
BOOL customDllChecked = FALSE;


#define ID_TEXTFIELD 12345
#define IDB_CONFIGURE 666

// HWND hEdit;
HWND dialogHwnd;
HWND mainHwnd;
HBITMAP hBitmap;
HMENU hMenu;

char statusTextBuffer[MAX_PATH * 2 + 1024] = {0};
void UpdateStatus();
void PrintOutput(const char* format, ...);

char customDllLoc[MAX_PATH];


BOOL readRegKey(char * key,char* subkey,int type,void *valPtr,DWORD bufSize){
    BOOL ok = FALSE;
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
                        ok = TRUE;
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
                    ok = TRUE;
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

BOOL readRegInt(char * key,char* subkey,DWORD *val){
    return readRegKey(key,subkey,REG_DWORD,val,sizeof(DWORD));
}

void writeRegInt(char * key,char* subkey,DWORD value){
    writeRegKey(key,subkey,REG_DWORD,&value);
}

BOOL readRegString(char * key,char* subkey,char *value,int bufSize){
    return readRegKey(key,subkey,REG_SZ,value,bufSize);
}

void writeRegString(char * key,char* subkey,void *value){
    writeRegKey(key,subkey,REG_SZ,value);
}

BOOL getDllPath(char *dir,uint32_t bufSize)
{

    int customDllLocLen = customDllChecked ? strlen(customDllLoc) : 0;
    if(customDllLocLen > 0)
    {
        if(bufSize >= customDllLocLen){
            strcpy(dir,customDllLoc);
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    if (GetCurrentDirectoryA(bufSize, dir) < bufSize)
    {

        // always termoinate with slash!

        for (int i = 0; dir[i]; i++)
            if (dir[i] == '\\')
                dir[i] = '/';
        // just use the same dir:
        strcat(dir, "/");
        return TRUE;
    }
    return FALSE;
}


DWORD getWineProtocol(){
    DWORD protocol = 0;
    if(!readRegInt(SETTINGS_HKEY_PATH,SETTING_PROTOCOL,&protocol)){
        fprintf(stderr,"cannot read protocol from registry\n");
    }

    if(protocol == 0) protocol = 3; // both is default
    return protocol;
}



// selection:
// 0 none
// 1 freetrack
// 2 npclient
// >=3 both
BOOL createRegistryKeys(int selection)
{
    char dllPath[MAX_PATH];

    if(!getDllPath(dllPath,sizeof(dllPath))){
        return FALSE;
    }

    PrintOutput("Setting DLL-Location to (protocols %d): %s\n",selection,dllPath);

    writeRegString(FREETRACKCLIENT_LOCATION_REG_KEY, PATH, FREETRACK_SELECTED(selection) ? dllPath : "");
    writeRegString(NPCLIENT_LOCATION_REG_KEY, PATH,NPCLIENT_SELECTED(selection) ? dllPath : "");

    return TRUE;
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

BOOL CheckRegistryKeys(){
    uint8_t proto = getWineProtocol();

    char npLoc[MAX_PATH];
    readRegString(NPCLIENT_LOCATION_REG_KEY,PATH,npLoc,sizeof(npLoc));
    PrintOutput("NPClient DLL-Location: %s\n",npLoc);

    char ftLoc[MAX_PATH];
    readRegString(FREETRACKCLIENT_LOCATION_REG_KEY,PATH,ftLoc,sizeof(ftLoc));
    PrintOutput("Freetrack DLL-Location: %s\n",ftLoc);

    BOOL ok = FALSE;

    char expectedDllLoc[MAX_PATH];
    if(!getDllPath(expectedDllLoc,sizeof(expectedDllLoc))){
        return FALSE;
    }

    switch (proto)
    {
    case 0:
        ok = strcmp(npLoc,"") == 0 && strcmp(ftLoc,"") == 0;
        break;
    case 1:
        ok = strcmp(npLoc,"") == 0 && strcmp(ftLoc,expectedDllLoc)==0;
    case 2:
        ok = strcmp(npLoc,expectedDllLoc) == 0 && strcmp(ftLoc,"")==0;
    case 3:
        ok = strcmp(npLoc,expectedDllLoc) == 0 && strcmp(ftLoc,expectedDllLoc)==0;
    default:
        break;
    }


    return ok;
}


#if _WIN64
#define DLLNAME "freetrackclient64.dll"
#else
#define DLLNAME "freetrackclient.dll"
#endif


/**
 * returns 0 if lib could not be loaded
 * returns 1 if library could be loaded
 * returns 2 if library oculd be loaded anda procedure with given name exists
 */
char* TryLoadDll(char*dllLoc,char *dllname,char *procName){
    int status = 0;
    strcat(dllLoc,dllname);
    #if _WIN64
        strcat(dllLoc,"64");
    #endif
    strcat(dllLoc,".dll");

    for (int i = 0; dllLoc[i]; i++)
		if (dllLoc[i] == '/')
			dllLoc[i] = '\\';

    PrintOutput("trying %s\n",dllLoc);

    HINSTANCE hDLL = LoadLibrary(dllLoc);
    if(hDLL){
        status=1;

        FARPROC proc = GetProcAddress(hDLL, procName);
        if(proc)
            status = 2;

        FreeLibrary(hDLL);
    }

    switch(status){
        case 0:
            return "can't load";
        case 1:
            return "native DLL loaded";
        case 2:
            return "builtin Unix-Lib loaded";

    }


    return "n/a";
}


void UpdateStatus(){
    char * ftStatus = "n/a";
    char * npStatus = "n/a";

    char dllLoc[MAX_PATH];

    readRegString(FREETRACKCLIENT_LOCATION_REG_KEY,PATH,dllLoc,sizeof(dllLoc));
    if(strlen(dllLoc))
        ftStatus = TryLoadDll(dllLoc,"freetrackclient","OpentrackUnixLibVersion");


    readRegString(NPCLIENT_LOCATION_REG_KEY,PATH,dllLoc,sizeof(dllLoc));
    if(strlen(dllLoc))
        npStatus = TryLoadDll(dllLoc,"npclient","OpentrackUnixLibVersion");


    sprintf(statusTextBuffer,"Freetrack: %s\nNPClient: %s\n",
        ftStatus,
        npStatus
        );

    // does not work:
    // SendMessage(mainHwnd,WM_PAINT,0,0);
    // this does:
    InvalidateRect(mainHwnd, NULL, TRUE);
    UpdateWindow(mainHwnd);
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
                    createRegistryKeys(protocol);
                }
                if(customDllChecked)
                {
                    int len = strlen(customDllLoc);
                    if(len>0 && customDllLoc[len-1] == '/'){
                        writeRegString(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_LOC,customDllLoc);
                        EndDialog(hwndDlg, 0);
                    }
                    else {
                        MessageBox(hwndDlg,"custom DLL location must not be empty and ending with / !","Error",MB_OK);
                    }
                }

                UpdateStatus();
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


void AppendText( HWND hwnd, char *newText,int ctrlId )
{
    // get edit control from dialog
    HWND hwndOutput = GetDlgItem( hwnd, ctrlId );

    if(hwndOutput == 0){
        fprintf(stderr,"cannot find item %d\n",ctrlId);
        return;
    }

    // get the current selection
    DWORD StartPos, EndPos;
    SendMessage( hwndOutput, EM_GETSEL, (WPARAM)&StartPos, (WPARAM)&EndPos );

    // move the caret to the end of the text
    int outLength = GetWindowTextLength( hwndOutput );
    SendMessage( hwndOutput, EM_SETSEL, outLength, outLength );

    // insert the text at the new caret position
    SendMessage( hwndOutput, EM_REPLACESEL, TRUE, (LPARAM)(newText) );

    // restore the previous selection
    // SendMessage( hwndOutput, EM_SETSEL, StartPos, EndPos );
}


void PrintOutput(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    char buf[MAX_PATH]={0};
    // vfprintf(stderr, format, argptr);
    vsprintf(buf,format,argptr);
    va_end(argptr);

    AppendText(mainHwnd,buf,ID_TEXTFIELD);
    // SendMessage(mainHwnd,WM_PAINT,0,0);
}

void Startup()
{

    readRegString(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_LOC,customDllLoc,sizeof(customDllLoc));
    readRegInt(SETTINGS_HKEY_PATH,SETTING_CUSTOM_DLL_CHECKED,(DWORD *)&customDllChecked);

    PrintOutput("OpentrackWineBride Util 0.9b1\n");

    if(!CheckRegistryKeys()){
        PrintOutput("Warning: Registry did not match settings. Updating...\n");
        createRegistryKeys(getWineProtocol());
    }

    UpdateStatus();
}




LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    static HWND hwndEdit;

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

        break;

        case WM_CTLCOLORSTATIC:
        {
            if( (HWND)lParam == GetDlgItem(hwnd, ID_TEXTFIELD) )
            {
                SetBkMode( (HDC)wParam, TRANSPARENT );
                SetTextColor((HDC)wParam, RGB(0, 0, 0));
                return (LRESULT)( (HBRUSH)GetStockObject(WHITE_BRUSH) );
                // if edit control is in dialog procedure change LRESULT to INT_PTR
            }
            else  // this is some other static control, do not touch it!!
                return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }


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

            // // TEXT:

            RECT rc = {79,10,300,200};
            // GetClientRect(hwnd, &rc);

            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));



            // TextOut(hdc,10,10,szBuffer,strlen(szBuffer));
            DrawText(hdc, statusTextBuffer,strlen(statusTextBuffer), &rc,0/*DT_CENTER | DT_VCENTER*//* | DT_SINGLELINE */);

            EndPaint(hwnd, &ps);


            // print text:

            //  PAINTSTRUCT ps;
            //     HDC hDC;
            //     char szBuffer[]="Hello, World!";
            //     hDC=BeginPaint(hwnd,&ps);
            //     TextOut(hDC,10,10,szBuffer,strlen(szBuffer));
            //     EndPaint(hwnd,&ps);

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
        320, 193+25 + 100,
        NULL, NULL, hInstance, NULL
    );

    if (!mainHwnd) {
        return 1;
    }

    fprintf(stderr,"mainHwnd=%d\n",mainHwnd);

    UpdateWindow(mainHwnd);
    PlaceInCenterOfScreen(mainHwnd,0,0);
    ShowWindow(mainHwnd, nCmdShow);

    Startup();




    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(clearRegistryKeysOnExit){
         createRegistryKeys(0);
    }

    return 0;
}


