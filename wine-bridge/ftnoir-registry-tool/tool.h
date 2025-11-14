#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace ftnoir_registry_tool {

void setVerbose(bool v);
bool isVerbose();

/// Returns the directory where the executable is located.
std::string getExecutableDir();

/// Returns the current working directory.
std::string getCurrentDir();

/// Returns the directory path to use for the client DLLs.
std::string getDllPath();

// Gets the NPClient registry path, returns true if success/exists
bool getNPClientPath(std::string& npclientPath);

// Gets the FreeTrackClient registry path, returns true if success/exists
bool getFreetrackClientPath(std::string& ftclientPath);

/// Sets the NPClient registry path to the given directory.
void setNPClientPath(const std::string& path,std::ostream &out = std::cout,std::ostream &err = std::cerr);

/// Sets the FreeTrackClient registry path to the given directory.
void setFreetrackClientPath(const std::string& path,std::ostream &out = std::cout ,std::ostream &err = std::cerr);

/// Removes the FreeTrackClient registry entry.
void clearFTClient(std::ostream &out = std::cout,std::ostream &err = std::cerr);

/// Removes the NPClient registry entry.
void clearNPClient(std::ostream &out = std::cout ,std::ostream &err = std::cerr);

/// Removes both NPClient and FreeTrackClient registry entries.
void clearBothProtocols(std::ostream &out = std::cout ,std::ostream &err = std::cerr);

/// Prints the current registry settings for both protocols.
// return 0=none, 1=ft,2=np,3=both
int printCurrentSettings(std::ostream &out = std::cout);

/// Checks if the directory exists and contains at least one expected DLL.
bool isValidClientDllDirectory(const std::string& dirPath);

/// Prints a warning saying that the DLL directory is invalid or missing DLLs.
void warnInvalidDllDirectory(const std::string& dirPath,std::ostream& err = std::cerr);

}
