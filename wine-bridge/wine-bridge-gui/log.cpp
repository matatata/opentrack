

#include "log.h"
#include <iostream>

using namespace std;

Log::Log(HWND hwnd,int ctrlId):hwnd(hwnd),ctrlId(ctrlId){
    InitializeCriticalSection(&criticalSection);
}
Log::~Log(){
    DeleteCriticalSection(&criticalSection);
}


void AppendText( HWND hwnd, const string &text,int ctrlId )
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

    string newText = text;
    // newText.erase(newText.find_last_not_of(" \n\r\t") + 1);

    // insert the text at the new caret position
    SendMessage( hwndOutput, EM_REPLACESEL, TRUE, (LPARAM)(newText.c_str()) );

    // deselec all
    SendMessage( hwndOutput, EM_SETSEL, -1, 0 );
}


void Log::log(const string &text) {

    if(text.length() == 0)
        return;

    EnterCriticalSection(&criticalSection);
    std::string t = text;

    if(newLineRequired) {
        t = "\n" + t;
        newLineRequired = false;
    }

    AppendText(hwnd,t,ctrlId);
    LeaveCriticalSection(&criticalSection);
}

void Log::logln(const string &text) {
   log(text);
   EnterCriticalSection(&criticalSection);
   newLineRequired = true;
   LeaveCriticalSection(&criticalSection);
}

void Log::clear() {
    EnterCriticalSection(&criticalSection);
    SetWindowText(GetDlgItem(hwnd,ctrlId),"");
    newLineRequired = false;
    LeaveCriticalSection(&criticalSection);
}
