#include <windows.h>
#include "resource.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <tchar.h>

using namespace std;


HWND hDialog;
BOOL disable_on_close=false;



#if defined(_WIN64)
    #define TOOL_NAME "ftnoir-registry-tool64.exe"
#else
    #define TOOL_NAME "ftnoir-registry-tool.exe"
#endif

#define TOOL_PATH "@opentrack-install-winebridge-bin-winepath-quoted@"



void AppendText( HWND hwnd, const string &newText,int ctrlId )
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
    SendMessage( hwndOutput, EM_REPLACESEL, TRUE, (LPARAM)(newText.c_str()) );

    // restore the previous selection
    // SendMessage( hwndOutput, EM_SETSEL, StartPos, EndPos );
}


void Log(const string &text) {
    AppendText(hDialog,text,ID_TEXT);
}

int windows_system(string& cmdline, string& output) {
    DWORD ReturnValue = -1;
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    HANDLE hPipeRead, hPipeWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

    // Create a pipe for stdout
    if (!CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0)) {
        Log("Failed to create pipe\n");
        return -1;
    }

    // Configure startup info to use the pipe
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = hPipeWrite;
    si.hStdError = hPipeWrite;

    output = "";

    // Launch the process
    if (CreateProcess(NULL, cmdline.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(hPipeWrite);  // Close write end in parent

        // Read from pipe
        char buffer[256];
        DWORD bytesRead;
        while (ReadFile(hPipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &ReturnValue);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        string err = "Failed to create process " + cmdline + "\n";
        Log(err.c_str());
        return -1;
    }

    CloseHandle(hPipeRead);
    return ReturnValue;
}

void setCheckbox(HWND hwnd, UINT id, BOOL checked ){
     SendMessage(GetDlgItem(hwnd, id), BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
}

int call_registry_tool(const string& args,string &output){
    string cmdline;

    const char* e = getenv("OTR_WINEBRIDGE_BIN");
    if(e != NULL && strlen(e) > 0){
        cmdline = e;
    }
    else {
        cmdline = TOOL_PATH;
    }
    cmdline += "\\";
    cmdline += TOOL_NAME;
    cmdline += " " + args;

    cout << "Executing  " << cmdline.c_str() << endl;
    return windows_system(cmdline,output);
}

int writeSettings(HWND hwnd){
    string output;

    int index = SendDlgItemMessage(hwnd, ID_PROTO, CB_GETCURSEL, 0, 0);
    if (index == CB_ERR) {
        return -1;
    }

    int ret = -1;
    switch (index)
    {
    case 0:
        ret = call_registry_tool("-v -d all",output);
        break;
    case 1:
        ret = call_registry_tool("-v -e ft",output);
        break;
    case 2:
        ret = call_registry_tool("-v -e np",output);
        break;
    case 3:
        ret = call_registry_tool("-v -e all",output);
        break;
    default:
        break;
    }
    Log(output);
    return ret;
}

void getCurrentValues(HWND hwnd){
    string output;
    int prot = call_registry_tool("-s",output);
    Log(output);

    if(prot != -1) {
        // setCheckbox(hwnd,ID_FT,prot & 1 == 1);
        // setCheckbox(hwnd,ID_NP,prot & 2 == 2);
        HWND hCombo = GetDlgItem(hwnd, ID_PROTO);
        SendMessage(hCombo, CB_SETCURSEL, (WPARAM)prot, 0);
    }
}


INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG:{
            hDialog = hwnd;
            HWND hCombo = GetDlgItem(hwnd, ID_PROTO);
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"None");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Freetrack");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"NPCLient");
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)"Both (recommended)");
            getCurrentValues(hwnd);
            return TRUE;
        }
        case WM_COMMAND:
            switch LOWORD(wParam) {
                case ID_CLOSE:
                    EndDialog(hwnd, 0);
                break;
                case ID_WRITE: {
                    writeSettings(hwnd);
                }
                break;
                case ID_REVERT: {
                    getCurrentValues(hwnd);
                }
                break;
                case ID_CLEAR: {
                    SetWindowText(GetDlgItem(hwnd,ID_TEXT),"");
                }
                break;
                case ID_PROTO: {
                }
                break;
                case ID_DISABLE_ON_CLOSE:{
                    disable_on_close = IsDlgButtonChecked(hwnd, ID_DISABLE_ON_CLOSE);
                }
                break;
        }
        break; // WM_COMMAND

        case WM_CLOSE:
            EndDialog(hwnd, 0);
            return TRUE;
    }
    return FALSE;
}





int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
    string output;
    if(disable_on_close) {
        ret = call_registry_tool("-v -d all",output);
    }
    return ret;
}
