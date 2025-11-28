#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "tool.h"

using namespace std;
using namespace ftnoir_registry_tool;

bool adv_interactive = false;
bool exclusive = false;
string dllPath;
string progname;

#define PROGRAM_DESCRIPTION "This program manages the registry entries that dictate if and where games will look for the freetrackclient and NPCLient dlls."

/// Extracts the program name from a full path.
std::string getProgramName(const std::string& fullPath);
/// Case-insensitive comparison of two wide strings.
bool iequals(const std::string& a, const std::string& b);
/// Prints usage information for the command-line tool.
void printUsage(const std::string& progname);
/// Prints help header for the command-line tool.
void printHelpHeader(const std::string& progname);
/// Prints help information for the command-line tool.
void printHelp(const std::string& progname);
/// Prints usage information for the interactive console mode.
void printInteractiveUsage();

void interactiveConsole()
{
    printHelpHeader(progname);
    printInteractiveUsage();

    setVerbose(true);

    while (true) {
        cout << "\nEnter your choice: ";
        string input;
        getline(cin, input);

        if (input.empty()) {
            cout << "Please enter a letter." << endl;
            printInteractiveUsage();
            continue;
        }

        char choice = tolower(input[0]);

        switch (choice) {
            case 'i': {
                adv_interactive = true;
                return;
            }
            case 'n': {
                string dllPath = getDllPath();
                clearFTClient();
                setNPClientPath(dllPath);
                break;
            }
            case 'f': {
                string dllPath = getDllPath();
                clearNPClient();
                setFreetrackClientPath(dllPath);
                break;
            }
            case 'a': {
                string dllPath = getDllPath();
                setNPClientPath(dllPath);
                setFreetrackClientPath(dllPath);
                break;
            }
            case 'd':
                clearBothProtocols();
                break;
            case 'p':
                printCurrentSettings();
                break;
            case 'q':
                cout << "Exiting." << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
                printInteractiveUsage();
                break;
        }
    }
}


PCHAR*
CommandLineToArgvA(
    PCHAR CmdLine,
    int* _argc
    )
{
    PCHAR* argv;
    PCHAR  _argv;
    ULONG   len;
    ULONG   argc;
    CHAR   a;
    ULONG   i, j;

    BOOLEAN  in_QM;
    BOOLEAN  in_TEXT;
    BOOLEAN  in_SPACE;

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
        i + (len+2)*sizeof(CHAR));

    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while( a = CmdLine[i] ) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}

// Moved here from utils.cpp
void ParseCommandLine(vector<string>& argv_vec,LPSTR cmdLine) {
    int argc=0;
    LPSTR* szArglist = CommandLineToArgvA(cmdLine, &argc);
    if (szArglist) {
        for (int i = 0; i < argc; i++) {
            argv_vec.push_back(szArglist[i]);
        }
        LocalFree(szArglist);
    }
}

int processArgs(vector<string>& argv_vec){
    int argc = argv_vec.size();
    for (int i = 0; i < argc; ++i) {
        const string& arg = argv_vec[i];
        string opt;
        if (arg[0] == '-') {
            opt = arg.substr(1);
dispatch:
            if (opt == "i") {
                adv_interactive = true;
            }
            else if (opt == "h" || opt == "help") {
                printHelp(progname);
            }
            else if (opt == "p" || opt == "print") {
                printCurrentSettings();
            }
            else if (opt == "x" || opt == "exclusive") {
                exclusive = true;
                if (isVerbose()) cout << "Exclusive mode enabled" << endl;
            }
            else if (opt == "c" || opt == "currentDir") {
                dllPath = getCurrentDir();
                if (isVerbose()) cout << "Using current directory " << dllPath << endl;
            } else if ((opt == "l" || opt=="location") && i + 1 < argc) {
                dllPath = argv_vec[++i];
                if (isVerbose()) cout << "Using custom directory " << dllPath << endl;
            }
            else if (opt == "v" || opt == "verbose") {
                setVerbose(true);
            } else if ((opt == "e" || opt == "enable") && i + 1 < argc) {
                string nextArg = argv_vec[++i];

                if (iequals(nextArg,"npclient") || iequals(nextArg,"np")) {
                    setNPClientPath(dllPath);
                    if(exclusive) clearFTClient();
                } else if (iequals(nextArg,"freetrack") || iequals(nextArg,"ft")) {
                    setFreetrackClientPath(dllPath);
                    if(exclusive) clearNPClient();
                }
                else if (iequals(nextArg,"both") || iequals(nextArg,"all")) {
                    setNPClientPath(dllPath);
                    setFreetrackClientPath(dllPath);
                } else {
                    cerr << "Unknown protocol: " << nextArg << endl;
                }
            } else if ((opt == "d" || opt == "disable") && i + 1 < argc) {
                string nextArg = argv_vec[++i];
                if (iequals(nextArg,"npclient") || iequals(nextArg,"np")) {
                    clearNPClient();
                } else if (iequals(nextArg,"freetrack") || iequals(nextArg,"ft")) {
                    clearFTClient();
                }
                else if (iequals(nextArg,"both") || iequals(nextArg,"all")) {
                    clearBothProtocols();
                } else {
                    cerr << "Unknown protocol: " << nextArg << endl;
                }
            }
            else if (opt == "s") {
                exit(printCurrentSettings());
            }
            else {
                printUsage(progname);
                return 1;
            }
        } else {
            // normal argument (not starting with '-')

            if(arg=="help"){
                printHelp(progname);
            }
            else if(arg=="quit" || arg=="exit" || arg=="q"){
                cout << "Exiting." << endl;
                exit(0);
            }
            else if(adv_interactive && arg.length() >= 1) {
                opt = arg;
                goto dispatch;
            }
            else {
                cerr << "Unexpected argument: " << arg << endl;
                printUsage(progname);
                return 1;
            }
        }
    }
    return 0;
}

void interactiveConsoleAdvanced()
{
    setVerbose(true);

    while (true) {
        cout << "> ";
        string input;
        getline(cin, input);


        if (input.empty()) {
            continue;
        }


        vector<string> argv_vec;
        ParseCommandLine(argv_vec, const_cast<LPSTR>(input.c_str()));
        processArgs(argv_vec);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    vector<string> argv_vec;
    ParseCommandLine(argv_vec,GetCommandLine());

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

string getProgramName(const string& fullPath)
{
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != string::npos)
        return fullPath.substr(pos + 1); // includes trailing slash
    return fullPath;
}

bool iequals(const string& a, const string& b)
{
    auto ichar_equals = [](char a, char b) {
        return tolower(a) == tolower(b);
    };
    return equal(a.begin(), a.end(), b.begin(), b.end(), ichar_equals);
}


void printUsage(const string& progname) {
    cout << "Usage: " << progname << " cmd1 cmd2 ... cmdN  where cmd is one of -h, -v, -p, -x, -s, -e ft|np|all, -d ft|np|all, -c or -l <path>" << endl;
}


void printHelpHeader(const string& progname) {
    cout << "=== " << progname << " ===" << endl;
    cout << PROGRAM_DESCRIPTION << endl ;
}

void printHelp(const string& progname) {
    printHelpHeader(progname);
    printUsage(progname);
    cout << "  -h                  \t\tPrint help." << endl;
    cout << "  -v                  \t\tEnable verbose output." << endl;
    cout << "  -p                  \t\tPrint current settings." << endl;
    cout << "  -x                  \t\tExclusive mode: When enabling a specific protocl then the other protocol" << endl;
    cout << "                      \t\twill be disabled automatically." << endl;
    cout << "  -e np | ft | all    \t\tEnable npclient, freetrack or all protocols." << endl;
    cout << "  -d np | ft | all    \t\tDisable npclient, freetrack or all protocols." << endl;
    cout << "  -c                  \t\tWith this option the current directory will be used." << endl;
    cout << "  -l C:\\custom\\loc\\\t\tSet a custom location. Pass before any occurence of -e to be effective." << endl;
    cout << "                      \t\tDo not use forward slashes although the registry keys use them." << endl;
    cout << "  -s                  \t\tExits immediately with exit code: 0=no protocol configured," << endl;
    cout << "                      \t\t1=freetrack enabled, 2=npclient enabled, 3=bot enabled. " << endl;
    // cout << "  -i                                 Enters advanced interactive mode where you can enter all of the above" << endl;
    cout << endl;
    cout << "Notes:" << endl;
    cout << "Without arguments the program enters an ineractive mode." << endl;
    cout << endl;
    cout << "The commands are executed in the order of appearance on the commandline. Therefore you can combine -c or -d with one -e to enable one protocol exclusively." << endl;
    cout << endl;
    cout << "The program assumes the head-tracking client dlls are located in the same directory as this programs. You can define a custom location via -l C:\\other\\location\\" << endl;
    cout << endl;
    cout << "Examples:" << endl;
    cout << "Enable npclient and freetrack:" << endl;
    cout << "\t" << progname << " -e all" << endl;
    cout << "Disabling npclient without touching anything else:"<< endl;
    cout << "\t" << progname << " -d np"<< endl;
    cout << "Prints the current state then enables freetrack without touching anything else and prints the new state:" << endl;
    cout << "\t" << progname << " -p -e ft -p" << endl;
    cout << "Enabling npclient exclusively:" << endl;
    cout << "\t" << progname << " -x -e np" << endl;
    cout << "Using a custom directory for NPClient:" << endl;
    cout << "\t" << progname << " -l C:\\customdir\\ -e np enables npclient exclusively:" << endl;
    cout << "Using a different custom directory for NPClient and FreeTrackClient:" << endl;
    cout << "\t" << progname << " -l C:\\nppath\\ -e np -l C:\\ftpath\\ -e ft" << endl;
}

void printInteractiveUsage()
{
    cout << endl;
    cout << "[n] Enable NPClient protocol exclusively" << endl;
    cout << "[f] Enable FreeTrackNoir protocol exclusively" << endl;
    cout << "[a] Enable all protocols (recommended)" << endl;
    cout << "[d] Disable all protocols" << endl;
    cout << "[p] Print current Settings" << endl;
    // cout << "[i] Enter advanced interactive Console" << endl;
    cout << "[q] Quit" << endl;
}
