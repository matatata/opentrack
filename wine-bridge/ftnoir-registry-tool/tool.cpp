#include "tool.h"
#include "registry.h"
#include <windows.h>
#include <iostream>
#include <algorithm>
#include <filesystem>

namespace ftnoir_registry_tool {


#define NPCLIENT_LOCATION_REG_KEY "Software\\NaturalPoint\\NATURALPOINT\\NPClient Location"
#define FREETRACKCLIENT_LOCATION_REG_KEY "Software\\Freetrack\\FreeTrackClient"
#define PATH "Path"

using namespace std;

bool verbose = false;

void setVerbose(bool v) {
    verbose = v;
}

bool isVerbose() {
    return verbose;
}

string getExecutableDir()
{
    char path[MAX_PATH];
    DWORD len = GetModuleFileName(NULL, path, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        return "";
    string fullPath(path);
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != string::npos)
        return fullPath.substr(0, pos + 1); // includes trailing slash
    return "";
}

string getCurrentDir()
{
    char path[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, path);
    if (len != 0 && len < MAX_PATH)
        return string(path);
    return "";
}

string convertBackslashesToSlashes(const string& path)
{
    string converted = path;
    std::replace(converted.begin(), converted.end(), '\\', '/');
    return converted;
}

string convertForwardSlashesToBackslashes(const string& path)
{
    string converted = path;
    std::replace(converted.begin(), converted.end(), '/', '\\');
    return converted;
}

string getDllPath()
{
    auto path = getExecutableDir();
    path = convertBackslashesToSlashes(path);
    return path;
}




bool getNPClientPath(string& npclientPath){
    npclientPath="";
    return Registry::ReadStringValue(NPCLIENT_LOCATION_REG_KEY, PATH, npclientPath);
}

bool getFreetrackClientPath(string& ftclientPath){
    ftclientPath="";
    return Registry::ReadStringValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH, ftclientPath);
}

int printCurrentSettings(ostream &out)
{
    string npclientPath, freetrackPath;
    bool npclientExists = getNPClientPath(npclientPath);
    bool freetrackExists = getFreetrackClientPath(freetrackPath);

    out << "Current Settings:" << endl;
    if (npclientExists) {
        out << "NPClient Path: " << npclientPath << endl;
        if(!isValidClientDllDirectory(npclientPath)) {
            warnInvalidDllDirectory(npclientPath,out);
        }
    } else {
        out << "NPClient Path: Not set" << endl;
    }

    if (freetrackExists) {
        out << "FreeTrackClient Path: " << freetrackPath << endl;
        if(!isValidClientDllDirectory(freetrackPath)) {
            warnInvalidDllDirectory(freetrackPath,out);
        }
    } else {
        out << "FreeTrackClient Path: Not set" << endl;
    }

    int ret = 0;
    if(freetrackExists) ret += 1;
    if(npclientExists) ret += 2;

    return ret;
}

string appendTrainingSlashIfNedded(string path)
{
    if(path[path.length()-1] != '/')
        return path + '/';
    return path;
}

string validateAndPrepareDllPath(string path,ostream &err)
{
   string modifiedPath = appendTrainingSlashIfNedded(path);

    if (!isValidClientDllDirectory(path)) {
        warnInvalidDllDirectory(path,err);
    }

   return convertBackslashesToSlashes(modifiedPath);
}

void setNPClientPath(const string& path,ostream &out,ostream &err)
{
    string modifiedPath = validateAndPrepareDllPath(path,err);

    if (Registry::WriteStringValue(NPCLIENT_LOCATION_REG_KEY, PATH, modifiedPath)) {
       if(verbose) out << "NPClient path set to: " << modifiedPath << endl;
    } else {
        err << "Failed to set NPClient path to: " << modifiedPath << endl;
    }
}

void setFreetrackClientPath(const string& path,ostream &out,ostream &err)
{
    string modifiedPath = validateAndPrepareDllPath(path,err);

    if (Registry::WriteStringValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH, modifiedPath)) {
        if(verbose) out << "FreeTrackClient path set to: " << modifiedPath << endl;
    } else {
        err << "Failed to set FreeTrackClient path to: " << modifiedPath << endl;
    }
}

void clearFTClient(ostream &out,ostream &err)
{
    if (verbose) out << "Clearing FreeTrackClient registry entry" << endl;

    if (!Registry::DeleteValue(FREETRACKCLIENT_LOCATION_REG_KEY, PATH)) {
        err << "Failed to clear FreeTrackClient registry entry." << endl;
    }
}

void clearNPClient(ostream &out,ostream &err)
{
    if (verbose) out << "Clearing NPClient registry entry" << endl;

    if (!Registry::DeleteValue(NPCLIENT_LOCATION_REG_KEY, PATH)) {
        err << "Failed to clear NPClient registry entry." << endl;
    }
}

void clearBothProtocols(ostream &out,ostream &err)
{
    clearFTClient(out,err);
    clearNPClient(out,err);
}

bool isValidClientDllDirectory(const string& dirPath)
{
    namespace fs = std::filesystem;
    try {
        fs::path dir(convertForwardSlashesToBackslashes(dirPath));

        if (!fs::exists(dir) || !fs::is_directory(dir))
            return false;

        fs::path ftDll = dir / "freetrackclient.dll";
        fs::path npDll = dir / "npclient.dll";
        fs::path ft64Dll = dir / "freetrackclient64.dll";
        fs::path np64Dll = dir / "npclient64.dll";

        return fs::exists(ftDll) || fs::exists(npDll)
               || fs::exists(ft64Dll) || fs::exists(np64Dll);
    } catch (const fs::filesystem_error&) {
        return false;
    }
}

void warnInvalidDllDirectory(const string& dirPath,ostream& err)
{
    err << "Warning: The directory '" << dirPath << "' is invalid or does not contain any of the expected DLLs (freetrackclient.dll, npclient.dll, freetrackclient64.dll, npclient64.dll)." << endl;
}


} // namespace
