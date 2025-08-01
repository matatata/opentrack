#include "opentrack-wrapper-wine.h"
#include "resource.h"
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#define SETTINGS_HKEY_PATH "Software\\OpentrackWineBridge"
#define SETTING_CLEAR_REG_KEYS_ON_EXIT "clearRegistryKeysOnExit"
#define SETTING_PROTOCOL "protocol"

bool clearRegistryKeysOnExit = false;



void readRegKey(char * key,char* subkey,int type,void *valPtr){
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
            case REG_SZ:
                fprintf(stderr,"reading REG_SZ ot implemented\n");
            break;
            case REG_DWORD: {
                DWORD val;
                DWORD dataSize = sizeof(val);
                if (ERROR_SUCCESS == RegGetValue(hkpath, 0, subkey, RRF_RT_DWORD, 0,&val, &dataSize)) {
                    *(DWORD*)valPtr = val;
                }
                else {
                    fprintf(stderr,"Error reading.\n");
                }
            }
            break;
        }

        RegCloseKey(hkpath);
    }
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
                RegSetValueExA(hkpath, subkey, 0, type, (BYTE*) valPtr, sizeof(int32_t));
            break;
        }

        RegCloseKey(hkpath);
    }
}


DWORD readRegInt(char * key,char* subkey){
    DWORD value;
    readRegKey(key,subkey,REG_DWORD,&value);
    return value;
}

void writeRegString(char * key,char* subkey,void *value){
    writeRegKey(key,subkey,REG_SZ,value);
}

void writeRegInt(char * key,char* subkey,DWORD value){
    writeRegKey(key,subkey,REG_DWORD,&value);
}


int getWineProtocol(){
    int protocol = readRegInt(SETTINGS_HKEY_PATH,SETTING_PROTOCOL);
    if(protocol == 0) protocol = 3; // both is default
    return protocol;
}


DWORD WINAPI ThreadFunction(LPVOID lpParam) {

    otrdllmain(getWineProtocol());

    if(clearRegistryKeysOnExit) {
        clearRegistryKeys();
    }
    return 0;
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

            int protocol = getWineProtocol();
            SendMessage(hCombo, CB_SETCURSEL, (WPARAM)protocol-1, 0);


            // init checkbox
            clearRegistryKeysOnExit = readRegInt(SETTINGS_HKEY_PATH,SETTING_CLEAR_REG_KEYS_ON_EXIT);
            SendMessage(GetDlgItem(hwndDlg, IDCK_REMOVE), BM_SETCHECK, clearRegistryKeysOnExit ? BST_CHECKED : BST_UNCHECKED, 0);


            return TRUE;
        }
        case WM_COMMAND:
            // if(LOWORD(wParam) == IDC_EDIT1){
            //     switch (HIWORD(wParam)){
            //         case EN_CHANGE:{
            //             TCHAR buffer[256];
            //             GetWindowText(GetDlgItem(hwndDlg,IDC_EDIT1), buffer, sizeof(buffer)/sizeof(buffer[0]));
            //             // 'buffer' now contains the text from the edit control
            //             fprintf(stderr,"text changed %s",buffer);

            //          }break;

            //     }
            // }

            if(LOWORD(wParam) == IDCK_REMOVE){
                clearRegistryKeysOnExit = IsDlgButtonChecked(hwndDlg, IDCK_REMOVE);
                writeRegInt(SETTINGS_HKEY_PATH,SETTING_CLEAR_REG_KEYS_ON_EXIT,clearRegistryKeysOnExit);
            }

            if(LOWORD(wParam) == IDC_COMBO1){
                switch (HIWORD(wParam))
                {
                    case CBN_SELCHANGE: {
                        int index = SendDlgItemMessage(hwndDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
                        if (index != CB_ERR) {
                            int protocol = index+1;
                            fprintf(stderr,"proto %i",protocol);
                            writeRegInt(SETTINGS_HKEY_PATH,SETTING_PROTOCOL,protocol);
                            createRegistryKeys(protocol,protocol);
                        }
                    } break;
                }
            }
            return DefWindowProc(hwndDlg, uMsg, wParam, lParam);

        case WM_DESTROY:

            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwndDlg, uMsg, wParam, lParam);
    }
    return FALSE;
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SampleWindowClass";

  HWND hwnd = NULL;



    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    // PlaceInCenterOfScreen(hwnd,0,0);



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


