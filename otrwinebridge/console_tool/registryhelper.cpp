#include "registryhelper.h"
#include <iostream>
#include <vector>

bool RegistryHelper::WriteStringValue(const std::wstring& key, const std::wstring& valueName, const std::wstring& value) {
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error creating registry key: " << result << std::endl;
        return false;
    }

    result = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), (DWORD)(value.length() * sizeof(wchar_t) + sizeof(wchar_t)));
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error setting registry value: " << result << std::endl;
        return false;
    }

    return true;
}

bool RegistryHelper::ReadStringValue(const std::wstring& key, const std::wstring& valueName, std::wstring& value) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hKey);
    if(result == ERROR_FILE_NOT_FOUND){
        // value does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key: " << result << std::endl;
        return false;
    }

    DWORD dataType;
    DWORD dataSize = 0;
    result = RegQueryValueExW(hKey, valueName.c_str(), NULL, &dataType, NULL, &dataSize);
    if(result == ERROR_FILE_NOT_FOUND){
        // value does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS || dataType != REG_SZ) {
        RegCloseKey(hKey);
        std::cerr << "Error getting registry value size: " << result << std::endl;
        return false;
    }

    std::vector<wchar_t> buffer(dataSize / sizeof(wchar_t) + 1);
    result = RegQueryValueExW(hKey, valueName.c_str(), NULL, &dataType, reinterpret_cast<BYTE*>(buffer.data()), &dataSize);
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error reading registry value: " << result << std::endl;
        return false;
    }

    value = std::wstring(buffer.data());
    return true;
}

bool RegistryHelper::WriteDwordValue(const std::wstring& key, const std::wstring& valueName, DWORD value) {
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error creating registry key: " << result << std::endl;
        return false;
    }

    result = RegSetValueExW(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(DWORD));
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error setting registry value: " << result << std::endl;
        return false;
    }

    return true;
}

bool RegistryHelper::ReadDwordValue(const std::wstring& key, const std::wstring& valueName, DWORD& value) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hKey);

    if(result == ERROR_FILE_NOT_FOUND){
        // key does not exist is not an error
        return false;
    }

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key: " << result << std::endl;
        return false;
    }

    DWORD dataType;
    DWORD dataSize = sizeof(DWORD);
    result = RegQueryValueExW(hKey, valueName.c_str(), NULL, &dataType, reinterpret_cast<BYTE*>(&value), &dataSize);
    RegCloseKey(hKey);

    if(result == ERROR_FILE_NOT_FOUND){
        // value does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS || dataType != REG_DWORD) {
        std::cerr << "Error reading registry value: " << result << std::endl;
        return false;
    }

    return true;
}

bool RegistryHelper::DeleteValue(const std::wstring& key, const std::wstring& valueName) {
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, key.c_str(), 0, KEY_SET_VALUE, &hKey);
    if(result == ERROR_FILE_NOT_FOUND){
        // key does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key for deletion: " << result << std::endl;
        return false;
    }

    result = RegDeleteValueW(hKey, valueName.c_str());
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        std::cerr << "Could not delete registry value: " << result << std::endl;
        return false;
    }

    // Treat ERROR_FILE_NOT_FOUND as success (value already deleted)
    return true;
}
