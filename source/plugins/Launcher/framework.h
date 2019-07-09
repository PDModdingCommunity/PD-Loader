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

wstring COMPONENTS_FILE_STRING = DirPath() + L"\\plugins\\components.ini";
LPCWSTR COMPONENTS_FILE = COMPONENTS_FILE_STRING.c_str();

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


// Custom function. Works like GetPrivateProfileIntW but returns bool. Can detect a numeric value or string.
bool GetPrivateProfileBoolW(LPCWSTR lpAppName, LPCWSTR lpKeyName, bool default, LPCWSTR lpFileName)
{
	wchar_t buffer[8];
	GetPrivateProfileStringW(lpAppName, lpKeyName, L"", buffer, 8, lpFileName);
	//MessageBoxW(NULL, buffer, NULL, 0);

	for (wchar_t& chr : buffer)
		chr = towlower(chr);

	bool out;
	if ((lstrcmpW(buffer, L"true") == 0) || (lstrcmpW(buffer, L"1") == 0))
		out = true;
	else if ((lstrcmpW(buffer, L"false") == 0) || (lstrcmpW(buffer, L"0") == 0))
		out = false;
	else
		out = default;

	return out;
}

struct componentInfo { const LPCWSTR name; LPCWSTR friendlyName; LPCWSTR description; System::IntPtr cb; };
componentInfo componentsArray[] = {
	{ L"input_emulator", L"Input Emulator", L"Emulates input through keyboard and/or mouse.", System::IntPtr::Zero },
	{ L"touch_slider_emulator", L"Slider Emulator", L"Emulates slider through keyboard and/or mouse.", System::IntPtr::Zero },
	{ L"touch_panel_emulator", L"Touch Panel Emulator", L"Emulates touch panel through mouse.", System::IntPtr::Zero },

	{ L"sys_timer", L"Timer Freeze", L"Freezes the PV select timer at 39 seconds.", System::IntPtr::Zero },

	{ L"player_data_manager", L"Player Data Manager", L"Loads user-defined values into the PlayerData struct.\nRequired for modules and game mode modifiers.", System::IntPtr::Zero },

	{ L"frame_rate_manager", L"Frame Rate Manager", L"Adjusts animations to the correct speed at different frame rates.\nOnly needed when FPS isn't locket at 60.", System::IntPtr::Zero },

	{ L"stage_manager", L"Stage Manager", L"Allows for playing unlimited songs per session.", System::IntPtr::Zero },

	{ L"fast_loader", L"Fast Loader", L"Skip or speed up unnecessary loading steps.", System::IntPtr::Zero },

	{ L"camera_controller", L"Camera Controller", L"Enables freecam (toggled using F3).\nWASD to move, SPACE/CTRL for up/down, Q/R to rotate, R/F for zoom.\nHolding SHIFT/ALT changes control speed.", System::IntPtr::Zero },

	{ L"scale_component", L"Scale Component", L"Scales the graphics output framebuffer to fill the screen/window.", System::IntPtr::Zero },

	{ L"debug_component", L"Debug Component", L"Allows for changing game state (F4-F8 keys), using dev GUI and tests, and speeding up 2d animations/menus (hold SHIFT+TAB).", System::IntPtr::Zero },

	{ L"fps_limiter", L"FPS Limiter", L"Lets you set a framerate cap. The value of the limit is in the options tab.", System::IntPtr::Zero },

	{ L"target_inspector", L"Target Inspector", L"Enables hold transfers.", System::IntPtr::Zero },
};