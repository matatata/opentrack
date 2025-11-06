#include "utils.h"
#include "registryhelper.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <filesystem>

#define PROGRAM_DESCRIPTION L"This program manages the registry entries that dictate if and where games will look for the freetrackclient and NPCLient dlls."
#define NPCLIENT_LOCATION_REG_KEY L"Software\\NaturalPoint\\NATURALPOINT\\NPClient Location"
#define FREETRACKCLIENT_LOCATION_REG_KEY L"Software\\Freetrack\\FreeTrackClient"
#define PATH L"Path"

using namespace std;

extern bool verbose;

bool iequals(const wstring& a, const wstring& b)
{
    auto ichar_equals = [](wchar_t a, wchar_t b) {
        return towlower(a) == towlower(b);
    };
    return equal(a.begin(), a.end(), b.begin(), b.end(), ichar_equals);
}


void printUsage(const wstring& progname) {
    wcout << L"Usage: " << progname << L" cmd1 cmd2 ... cmdN  where cmd is one of -h, -v, -p, -x, -s, -e ft|np|all, -d ft|np|all, -c or -l <path>" << endl;
}


void printHelpHeader(const wstring& progname) {
    wcout << L"=== " << progname << " ===" << endl;
    wcout << PROGRAM_DESCRIPTION << endl ;
}

void printHelp(const wstring& progname) {
    printHelpHeader(progname);
    printUsage(progname);
    wcout << L"  -h                  \t\tPrint help." << endl;
    wcout << L"  -v                  \t\tEnable verbose output." << endl;
    wcout << L"  -p                  \t\tPrint current settings." << endl;
    wcout << L"  -x                  \t\tExclusive mode: When enabling a specific protocl then the other protocol" << endl;
    wcout << L"                      \t\twill be disabled automatically." << endl;
    wcout << L"  -e np | ft | all    \t\tEnable npclient, freetrack or all protocols." << endl;
    wcout << L"  -d np | ft | all    \t\tDisable npclient, freetrack or all protocols." << endl;
    wcout << L"  -c                  \t\tWith this option the current directory will be used." << endl;
    wcout << L"  -l C:\\custom\\loc\\\t\tSet a custom location. Pass before any occurence of -e to be effective." << endl;
    wcout << L"                      \t\tDo not use forward slashes although the registry keys use them." << endl;
    wcout << L"  -s                  \t\tExits immediately with exit code: 0=no protocol configured," << endl;
    wcout << L"                      \t\t1=freetrack enabled, 2=npclient enabled, 3=bot enabled. " << endl;
    // wcout << L"  -i                                 Enters advanced interactive mode where you can enter all of the above" << endl;
    wcout << endl;
    wcout << "Notes:" << endl;
    wcout << L"Without arguments the program enters an ineractive mode." << endl;
    wcout << endl;
    wcout << L"The commands are executed in the order of appearance on the commandline. Therefore you can combine -c or -d with one -e to enable one protocol exclusively." << endl;
    wcout << endl;
    wcout << L"By default the location for the client dlls is assumed to be this program's location. Use -l and -c to modify this behavior" << endl;
    wcout << endl;
    wcout << L"The program assumes the provided dlls are located in a directory 'clientlibs' relative to this programs location. You can define a custom location via -l C:\\other\\location\\" << endl;
    wcout << endl;
    wcout << L"Examples:" << endl;
    wcout << "Enable npclient and freetrack:" << endl;
    wcout << "\t" << progname << L" -e all" << endl;
    wcout << "Disabling npclient without touching anything else:"<< endl;
    wcout << "\t" << progname << L" -d np"<< endl;
    wcout << "Prints the current state then enables freetrack without touching anything else and prints the new state:" << endl;
    wcout << "\t" << progname << L" -p -e ft -p" << endl;
    wcout << "Enabling npclient exclusively:" << endl;
    wcout << "\t" << progname << L" -x -e np" << endl;
    wcout << L"Using a custom directory for NPClient:" << endl;
    wcout << "\t" << progname << L" -l C:\\customdir\\ -e np enables npclient exclusively:" << endl;
    wcout << L"Using a different custom directory for NPClient and FreeTrackClient:" << endl;
    wcout << "\t" << progname << L" -l C:\\nppath\\ -e np -l C:\\ftpath\\ -e ft" << endl;
}

void printInteractiveUsage()
{
    wcout << endl;
    wcout << L"[n] Enable NPClient protocol exclusively" << endl;
    wcout << L"[f] Enable FreeTrackNoir protocol exclusively" << endl;
    wcout << L"[a] Enable all protocols (recommended)" << endl;
    wcout << L"[d] Disable all protocols" << endl;
    wcout << L"[p] Print current Settings" << endl;
    // wcout << L"[i] Enter advanced interactive Console" << endl;
    wcout << L"[q] Quit" << endl;
}

wstring getExecutableDir()
{
    wchar_t path[MAX_PATH];
    DWORD len = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        return L"";
    wstring fullPath(path);
    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos != wstring::npos)
        return fullPath.substr(0, pos + 1); // includes trailing slash
    return L"";
}

wstring getCurrentDir()
{
    wchar_t path[MAX_PATH];
    DWORD len = GetCurrentDirectoryW(MAX_PATH, path);
    if (len != 0 && len < MAX_PATH)
        return wstring(path);
    return L"";
}

wstring convertBackslashesToSlashes(const wstring& path)
{
    wstring converted = path;
    std::replace(converted.begin(), converted.end(), L'\\', L'/');
    return converted;
}

wstring convertForwardSlashesToBackslashes(const wstring& path)
{
    wstring converted = path;
    std::replace(converted.begin(), converted.end(), L'/', L'\\');
    return converted;
}

wstring getDllPath()
{
    auto path = getExecutableDir();
    path = convertBackslashesToSlashes(path);
    return path;
}

int printCurrentSettings()
{
    wstring npclientPath, freetrackPath;
    bool npclientExists = RegistryHelper::ReadStringValue(NPCLIENT_LOCATION_REG_KEY, PATH, npclientPath);
    bool freetrackExists = RegistryHelper::ReadStringValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH, freetrackPath);

    wcout << L"Current Settings:" << endl;
    if (npclientExists) {
        wcout << L"NPClient Path: " << npclientPath << endl;
        if(!isValidClientDllDirectory(npclientPath)) {
            warnInvalidDllDirectory(npclientPath);
        }
    } else {
        wcout << L"NPClient Path: Not set" << endl;
    }

    if (freetrackExists) {
        wcout << L"FreeTrackClient Path: " << freetrackPath << endl;
        if(!isValidClientDllDirectory(freetrackPath)) {
            warnInvalidDllDirectory(freetrackPath);
        }
    } else {
        wcout << L"FreeTrackClient Path: Not set" << endl;
    }

    int ret = 0;
    if(freetrackExists) ret += 1;
    if(npclientExists) ret += 2;

    return ret;
}

wstring appendTrainingSlashIfNedded(wstring path)
{
    if(path[path.length()-1] != L'/')
        return path + L'/';
    return path;
}

wstring validateAndPrepareDllPath(wstring path)
{
   wstring modifiedPath = appendTrainingSlashIfNedded(path);

    if (!isValidClientDllDirectory(path)) {
        warnInvalidDllDirectory(path);
    }

   return convertBackslashesToSlashes(modifiedPath);
}

void setNPClientPath(const wstring& path)
{
    wstring modifiedPath = validateAndPrepareDllPath(path);

    if (RegistryHelper::WriteStringValue(NPCLIENT_LOCATION_REG_KEY, PATH, modifiedPath)) {
       if(verbose) wcout << L"NPClient path set to: " << modifiedPath << endl;
    } else {
        wcerr << L"Failed to set NPClient path to: " << modifiedPath << endl;
    }
}

void setFreetrackClientPath(const wstring& path)
{
    wstring modifiedPath = validateAndPrepareDllPath(path);

    if (RegistryHelper::WriteStringValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH, modifiedPath)) {
        if(verbose) wcout << L"FreeTrackClient path set to: " << modifiedPath << endl;
    } else {
        wcerr << L"Failed to set FreeTrackClient path to: " << modifiedPath << endl;
    }
}

void clearFTClient()
{
    if (verbose) wcout << L"Clearing FreeTrackClient registry entry" << endl;

    if (!RegistryHelper::DeleteValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH)) {
        cerr << L"Failed to clear FreeTrackClient registry entry." << endl;
    }
}

void clearNPClient()
{
    if (verbose) wcout << L"Clearing NPClient registry entry" << endl;

    if (!RegistryHelper::DeleteValue(NPCLIENT_LOCATION_REG_KEY, PATH)) {
        cerr << L"Failed to clear NPClient registry entry." << endl;
    }
}

void clearBoth()
{
    clearFTClient();
    clearNPClient();
}

wstring getProgramName(const wstring& fullPath)
{
    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos != wstring::npos)
        return fullPath.substr(pos + 1); // includes trailing slash
    return fullPath;
}

bool isValidClientDllDirectory(const wstring& dirPath)
{
    namespace fs = std::filesystem;
    try {
        fs::path dir(convertForwardSlashesToBackslashes(dirPath));
        if (!fs::exists(dir) || !fs::is_directory(dir))
            return false;

        fs::path ftDll = dir / L"freetrackclient.dll";
        fs::path npDll = dir / L"npclient.dll";
        fs::path ft64Dll = dir / L"freetrackclient64.dll";
        fs::path np64Dll = dir / L"npclient64.dll";

        return fs::exists(ftDll) || fs::exists(npDll)
               || fs::exists(ft64Dll) || fs::exists(np64Dll);
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

void warnInvalidDllDirectory(const wstring& dirPath)
{
    wcerr << L"Warning: The directory '" << dirPath << L"' is invalid or does not contain any of the expected DLLs (freetrackclient.dll, npclient.dll, freetrackclient64.dll, npclient64.dll)." << endl;
}
