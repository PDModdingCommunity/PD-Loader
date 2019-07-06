#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>

int (__cdecl* divaMain)(int argc, const char** argv, const char** envp) = (int(__cdecl*)(int argc, const char** argv, const char** envp))0x140194D90;

using namespace std;

string arg;

wstring ExePath() {
	WCHAR buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return wstring(buffer);
}

wstring DirPath() {
	wstring exepath = ExePath();
	wstring::size_type pos = exepath.find_last_of(L"\\/");
	return exepath.substr(0, pos);
}

wstring DIVA_EXECUTABLE_STRING = ExePath();
LPCWSTR DIVA_EXECUTABLE = DIVA_EXECUTABLE_STRING.c_str();

wstring DIVA_EXECUTABLE_LAUNCH_STRING = DIVA_EXECUTABLE_STRING + L" --launch";
LPWSTR DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING.c_str());

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

int nCursor = GetPrivateProfileIntW(L"patches", L"cursor", TRUE, CONFIG_FILE);
int nHideFreeplay = GetPrivateProfileIntW(L"patches", L"hide_freeplay", FALSE, CONFIG_FILE);
int nStatusIcons = GetPrivateProfileIntW(L"patches", L"status_icons", 0, CONFIG_FILE);
int nHidePVWatermark = GetPrivateProfileIntW(L"patches", L"hide_pv_watermark", FALSE, CONFIG_FILE);
int nNoPVUi = GetPrivateProfileIntW(L"patches", L"no_pv_ui", FALSE, CONFIG_FILE);
int nHideVolCtrl = GetPrivateProfileIntW(L"patches", L"hide_volume", FALSE, CONFIG_FILE);
int nNoLyrics = GetPrivateProfileIntW(L"patches", L"no_lyrics", FALSE, CONFIG_FILE);
int nNoMovies = GetPrivateProfileIntW(L"patches", L"no_movies", FALSE, CONFIG_FILE);
int nNoError = GetPrivateProfileIntW(L"patches", L"no_error", TRUE, CONFIG_FILE);

int nTAA = GetPrivateProfileIntW(L"graphics", L"TAA", TRUE, CONFIG_FILE);
int nMLAA = GetPrivateProfileIntW(L"graphics", L"MLAA", TRUE, CONFIG_FILE);
int nFPSLimit = GetPrivateProfileIntW(L"graphics", L"FPS.Limit", 0, CONFIG_FILE);

int nSkipLauncher = GetPrivateProfileIntW(L"launcher", L"skip", FALSE, CONFIG_FILE);
