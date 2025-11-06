#pragma once

#include <string>
#include <vector>

extern bool verbose;

/// Case-insensitive comparison of two wide strings.
bool iequals(const std::wstring& a, const std::wstring& b);

/// Extracts the program name from a full path.
std::wstring getProgramName(const std::wstring& fullPath);

/// Returns the directory where the executable is located.
std::wstring getExecutableDir();

/// Returns the current working directory.
std::wstring getCurrentDir();

/// Returns the directory path to use for the client DLLs.
std::wstring getDllPath();

/// Sets the NPClient registry path to the given directory.
void setNPClientPath(const std::wstring& path);

/// Sets the FreeTrackClient registry path to the given directory.
void setFreetrackClientPath(const std::wstring& path);

/// Removes the FreeTrackClient registry entry.
void clearFTClient();

/// Removes the NPClient registry entry.
void clearNPClient();

/// Removes both NPClient and FreeTrackClient registry entries.
void clearBoth();

/// Converts all backslashes in a path to forward slashes.
std::wstring convertBackslashesToSlashes(const std::wstring& path);

/// Converts all forward slashes in a path to backslashes.
std::wstring convertForwardSlashesToBackslashes(const std::wstring& path);

/// Appends a trailing slash to the path if needed.
std::wstring appendTrainingSlashIfNedded(std::wstring path);



/// Prints the current registry settings for both protocols.
// return 0=none, 1=ft,2=np,3=both
int printCurrentSettings();

/// Prints usage information for the command-line tool.
void printUsage(const std::wstring& progname);

/// Prints help header for the command-line tool.
void printHelpHeader(const std::wstring& progname);

/// Prints help information for the command-line tool.
void printHelp(const std::wstring& progname);

/// Prints usage information for the interactive console mode.
void printInteractiveUsage();

/// Checks if the directory exists and contains at least one expected DLL.
bool isValidClientDllDirectory(const std::wstring& dirPath);

/// Prints a warning saying that the DLL directory is invalid or missing DLLs.
void warnInvalidDllDirectory(const std::wstring& dirPath);

