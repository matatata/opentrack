#include "registry.h"
#include <iostream>
#include <vector>

bool Registry::WriteStringValue(const std::string& key, const std::string& valueName, const std::string& value) {
    HKEY hKey;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error creating registry key: " << result << std::endl;
        return false;
    }

    result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), (DWORD)(value.length() * sizeof(char) + sizeof(char)));
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error setting registry value: " << result << std::endl;
        return false;
    }

    return true;
}

bool Registry::ReadStringValue(const std::string& key, const std::string& valueName, std::string& value) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hKey);
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
    result = RegQueryValueEx(hKey, valueName.c_str(), NULL, &dataType, NULL, &dataSize);
    if(result == ERROR_FILE_NOT_FOUND){
        // value does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS || dataType != REG_SZ) {
        RegCloseKey(hKey);
        std::cerr << "Error getting registry value size: " << result << std::endl;
        return false;
    }

    std::vector<char> buffer(dataSize / sizeof(char) + 1);
    result = RegQueryValueEx(hKey, valueName.c_str(), NULL, &dataType, reinterpret_cast<BYTE*>(buffer.data()), &dataSize);
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error reading registry value: " << result << std::endl;
        return false;
    }

    value = std::string(buffer.data());
    return true;
}

bool Registry::WriteDwordValue(const std::string& key, const std::string& valueName, DWORD value) {
    HKEY hKey;
    LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error creating registry key: " << result << std::endl;
        return false;
    }

    result = RegSetValueEx(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(DWORD));
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        std::cerr << "Error setting registry value: " << result << std::endl;
        return false;
    }

    return true;
}

bool Registry::ReadDwordValue(const std::string& key, const std::string& valueName, DWORD& value) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hKey);

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
    result = RegQueryValueEx(hKey, valueName.c_str(), NULL, &dataType, reinterpret_cast<BYTE*>(&value), &dataSize);
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

bool Registry::DeleteValue(const std::string& key, const std::string& valueName) {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, KEY_SET_VALUE, &hKey);
    if(result == ERROR_FILE_NOT_FOUND){
        // key does not exist is not an error
        return false;
    }
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key for deletion: " << result << std::endl;
        return false;
    }

    result = RegDeleteValue(hKey, valueName.c_str());
    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        std::cerr << "Could not delete registry value: " << result << std::endl;
        return false;
    }

    // Treat ERROR_FILE_NOT_FOUND as success (value already deleted)
    return true;
}
