#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "registryhelper.h"
#include "utils.h"

using namespace std;

bool adv_interactive = false;
bool verbose = false;
bool exclusive = false;
wstring dllPath;
wstring progname;

void interactiveConsole()
{
    printHelpHeader(progname);
    printInteractiveUsage();

    verbose = true;

    while (true) {
        wcout << L"\nEnter your choice: ";
        wstring input;
        getline(wcin, input);

        if (input.empty()) {
            wcout << L"Please enter a letter." << endl;
            printInteractiveUsage();
            continue;
        }

        wchar_t choice = towlower(input[0]);

        switch (choice) {
            case L'i': {
                adv_interactive = true;
                return;
            }
            case L'n': {
                wstring dllPath = getDllPath();
                clearFTClient();
                setNPClientPath(dllPath);
                break;
            }
            case L'f': {
                wstring dllPath = getDllPath();
                clearNPClient();
                setFreetrackClientPath(dllPath);
                break;
            }
            case L'a': {
                wstring dllPath = getDllPath();
                setNPClientPath(dllPath);
                setFreetrackClientPath(dllPath);
                break;
            }
            case L'd':
                clearBoth();
                break;
            case L'p':
                printCurrentSettings();
                break;
            case L'q':
                wcout << L"Exiting." << endl;
                return;
            default:
                wcout << L"Invalid choice. Please try again." << endl;
                printInteractiveUsage();
                break;
        }
    }
}


// Moved here from utils.cpp
void ParseCommandLine(vector<wstring>& argv_vec,LPWSTR cmdLine) {
    int argc=0;
    LPWSTR* szArglist = CommandLineToArgvW(cmdLine, &argc);
    if (szArglist) {
        for (int i = 0; i < argc; i++) {
            argv_vec.push_back(szArglist[i]);
        }
        LocalFree(szArglist);
    }
}

int processArgs(vector<wstring>& argv_vec){
    int argc = argv_vec.size();
    for (int i = 0; i < argc; ++i) {
        const wstring& arg = argv_vec[i];
        wstring opt;
        if (arg[0] == L'-') {
            opt = arg.substr(1);
dispatch:
            if (opt == L"i") {
                adv_interactive = true;
            }
            else if (opt == L"h" || opt == L"help") {
                printHelp(progname);
            }
            else if (opt == L"p" || opt == L"print") {
                printCurrentSettings();
            }
            else if (opt == L"x" || opt == L"exclusive") {
                exclusive = true;
                if (verbose) wcout << L"Exclusive mode enabled" << endl;
            }
            else if (opt == L"c" || opt == L"currentDir") {
                dllPath = getCurrentDir();
                if (verbose) wcout << L"Using current directory " << dllPath << endl;
            } else if ((opt == L"l" || opt==L"location") && i + 1 < argc) {
                dllPath = argv_vec[++i];
                if (verbose) wcout << L"Using custom directory " << dllPath << endl;
            }
            else if (opt == L"v" || opt == L"verbose") {
                verbose = true;
            } else if ((opt == L"e" || opt == L"enable") && i + 1 < argc) {
                wstring nextArg = argv_vec[++i];

                if (iequals(nextArg,L"npclient") || iequals(nextArg,L"np")) {
                    setNPClientPath(dllPath);
                    if(exclusive) clearFTClient();
                } else if (iequals(nextArg,L"freetrack") || iequals(nextArg,L"ft")) {
                    setFreetrackClientPath(dllPath);
                    if(exclusive) clearNPClient();
                }
                else if (iequals(nextArg,L"both") || iequals(nextArg,L"all")) {
                    setNPClientPath(dllPath);
                    setFreetrackClientPath(dllPath);
                } else {
                    wcerr << L"Unknown protocol: " << nextArg << endl;
                }
            } else if ((opt == L"d" || opt == L"disable") && i + 1 < argc) {
                wstring nextArg = argv_vec[++i];
                if (iequals(nextArg,L"npclient") || iequals(nextArg,L"np")) {
                    clearNPClient();
                } else if (iequals(nextArg,L"freetrack") || iequals(nextArg,L"ft")) {
                    clearFTClient();
                }
                else if (iequals(nextArg,L"both") || iequals(nextArg,L"all")) {
                    clearBoth();
                } else {
                    wcerr << L"Unknown protocol: " << nextArg << endl;
                }
            }
            else if (opt == L"s") {
                exit(printCurrentSettings());
            }
            else {
                printUsage(progname);
                return 1;
            }
        } else {
            // normal argument (not starting with '-')

            if(arg==L"help"){
                printHelp(progname);
            }
            else if(arg==L"quit" || arg==L"exit" || arg==L"q"){
                wcout << L"Exiting." << endl;
                exit(0);
            }
            else if(adv_interactive && arg.length() >= 1) {
                opt = arg;
                goto dispatch;
            }
            else {
                wcerr << L"Unexpected argument: " << arg << endl;
                printUsage(progname);
                return 1;
            }
        }
    }
    return 0;
}

void interactiveConsoleAdvanced()
{
    verbose = true;

    while (true) {
        wcout << L"> ";
        wstring input;
        getline(wcin, input);


        if (input.empty()) {
            continue;
        }


        vector<wstring> argv_vec;
        ParseCommandLine(argv_vec, const_cast<LPWSTR>(input.c_str()));
        processArgs(argv_vec);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

    vector<wstring> argv_vec;
    ParseCommandLine(argv_vec,GetCommandLineW());

    exclusive = false;
    dllPath = getDllPath();
    progname = getProgramName(argv_vec[0]);

    argv_vec.erase(argv_vec.begin()); // remove program name

    if(argv_vec.size() == 0){
        interactiveConsole();
    }
    else  {
        processArgs(argv_vec);
    }


    // If no arguments, enter interactive mode
    if (adv_interactive) {
        interactiveConsoleAdvanced();
        return 0;
    }

    return 0;
}
