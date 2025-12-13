#pragma once


#include <windows.h>

#include <string>

class Log {
    public:
        Log(HWND hwnd,int ctrlId);
        ~Log();

        void log(const std::string &text);
        void logln(const std::string &text);
        void clear();

    private:
        HWND hwnd;
        int ctrlId;
        bool newLineRequired = false;
        CRITICAL_SECTION criticalSection;
};
