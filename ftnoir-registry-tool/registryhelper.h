#pragma once

#include <string>
#include <windows.h>

class RegistryHelper {
public:
    /**
     * Writes a string value to the registry.
     * @param key The registry key path.
     * @param valueName The name of the registry value.
     * @param value The string value to write.
     * @return True if the operation was successful, false otherwise.
     */
    static bool WriteStringValue(const std::wstring& key, const std::wstring& valueName, const std::wstring& value);

    /**
     * Reads a string value from the registry.
     * @param key The registry key path.
     * @param valueName The name of the registry value.
     * @param value The read string value.
     * @return True if the operation was successful, false otherwise.
     */
    static bool ReadStringValue(const std::wstring& key, const std::wstring& valueName, std::wstring& value);

    /**
     * Writes a DWORD (32-bit integer) value to the registry.
     * @param key The registry key path.
     * @param valueName The name of the registry value.
     * @param value The DWORD value to write.
     * @return True if the operation was successful, false otherwise.
     */
    static bool WriteDwordValue(const std::wstring& key, const std::wstring& valueName, DWORD value);

    /**
     * Reads a DWORD (32-bit integer) value from the registry.
     * @param key The registry key path.
     * @param valueName The name of the registry value.
     * @param value The read DWORD value.
     * @return True if the operation was successful, false otherwise.
     */
    static bool ReadDwordValue(const std::wstring& key, const std::wstring& valueName, DWORD& value);

    /**
     * Deletes a value from the registry.
     * @param key The registry key path.
     * @param valueName The name of the registry value to delete.
     * @return True if the operation was successful, false otherwise.
     */
    static bool DeleteValue(const std::wstring& key, const std::wstring& valueName);
};
