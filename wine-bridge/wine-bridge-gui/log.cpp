

#include "log.h"
#include <iostream>

using namespace std;

Log::Log(HWND hwnd,int ctrlId):hwnd(hwnd),ctrlId(ctrlId){
    //
}
Log::~Log(){
    //
}


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


void Log::log(const string &text) {
    std::cout << text;
    AppendText(hwnd,text,ctrlId);
}

void Log::logln(const string &text) {
   log(text + "\n");
}

void Log::clear() {
    SetWindowText(GetDlgItem(hwnd,ctrlId),"");
}
