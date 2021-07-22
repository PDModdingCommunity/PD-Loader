#pragma once
#include <windows.h>
#include <string>

void setFullscreenOptimizations(bool enable)
{
    HKEY Layers;

    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, KEY_ALL_ACCESS, &Layers);

    if (!result == ERROR_SUCCESS) return;

    WCHAR value[MAX_PATH];
    GetModuleFileNameW(NULL, value, MAX_PATH);

    if (!enable)
    {
        const LPCTSTR data = L"~ DISABLEDXMAXIMIZEDWINDOWEDMODE\0\0";
        result = RegSetValueExW(Layers, value, 0, REG_SZ, (LPBYTE)data, 2*(wcslen(data) + 1));
    }
    else result = RegDeleteValueW(Layers, value);

    if (!result == ERROR_SUCCESS) return;

    result = RegCloseKey(Layers);

    if (!result == ERROR_SUCCESS) return;
}

bool getFullscreenOptimizations()
{
    HKEY Layers;

    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, KEY_ALL_ACCESS, &Layers);

    if (!result == ERROR_SUCCESS) return 0;

    WCHAR value[MAX_PATH];
    GetModuleFileNameW(NULL, value, MAX_PATH);

    //const LPCTSTR data = L"~ DISABLEDXMAXIMIZEDWINDOWEDMODE";
    DWORD DataSize = 2048;
    WCHAR* DataPtr = (WCHAR*)malloc(DataSize);
    memset(DataPtr, 0, DataSize);
    result = RegQueryValueExW(Layers, value, NULL, NULL, (LPBYTE)DataPtr, &DataSize);

    if (!result == ERROR_SUCCESS) return 1;

    result = RegCloseKey(Layers);

    if (!result == ERROR_SUCCESS) return 1;

    std::wstring regstr(DataPtr);
    bool val = regstr.find(L"DISABLEDXMAXIMIZEDWINDOWEDMODE") == -1;
       

    //bool val = wcscmp(data, DataPtr);

    free(DataPtr);
    return val;
}
