#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>

static int(__cdecl* divaCreateWindow)(const char* title, void(__cdecl* exitfunc)(int)) = (int(__cdecl*)(const char* title, void(__cdecl * exitfunc)(int)))0x140194D00;
__int64 (__fastcall* divaParseParameters)(int a1, __int64* a2) = (__int64(__fastcall*)(int a1, __int64* a2))0x140193630;
__int64(__fastcall* divaEngineUpdate)(__int64 a1) = (__int64(__fastcall*)(__int64 a1))0x140194CD0;

uint8_t* fullScreenFlag = (uint8_t*)0x140EDA5D1;
DWORD* resolutionType = (DWORD*)0x140EDA5D4;

using namespace std;

wstring DirPath() {
	WCHAR buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	wstring::size_type pos = wstring(buffer).find_last_of(L"\\/");
	return wstring(buffer).substr(0, pos);
}

wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\config.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

int nDisplay = GetPrivateProfileIntW(L"resolution", L"display", 0, CONFIG_FILE);
int nWidth = GetPrivateProfileIntW(L"resolution", L"width", 1280, CONFIG_FILE);
int nHeight = GetPrivateProfileIntW(L"resolution", L"height", 720, CONFIG_FILE);

int nIntRes = GetPrivateProfileIntW(L"resolution", L"r.enable", FALSE, CONFIG_FILE);
int nIntResWidth = GetPrivateProfileIntW(L"resolution", L"r.width", 1280, CONFIG_FILE);
int nIntResHeight = GetPrivateProfileIntW(L"resolution", L"r.height", 720, CONFIG_FILE);

int nBitDepth = GetPrivateProfileIntW(L"resolution", L"bitdepth", 32, CONFIG_FILE);
int nRefreshRate = GetPrivateProfileIntW(L"resolution", L"refreshrate", 60, CONFIG_FILE);

int nFpsLimit = GetPrivateProfileIntW(L"graphics", L"fps.limit", 60, CONFIG_FILE);
int nUseLightLimiter = GetPrivateProfileIntW(L"graphics", L"fps.limit.lightmode", TRUE, CONFIG_FILE);
int nVerboseLimiter = GetPrivateProfileIntW(L"graphics", L"fps.limit.verbose", FALSE, CONFIG_FILE);

// used to trick Optimus into switching to the NVIDIA GPU
HMODULE nvcudaModule = LoadLibraryW(L"nvcuda.dll");
// cuInit actually returns a CUresult, but we don't really care about it
void(WINAPI * cuInit)(unsigned int flags) = (void(WINAPI*)(unsigned int flags))GetProcAddress(nvcudaModule, "cuInit");
