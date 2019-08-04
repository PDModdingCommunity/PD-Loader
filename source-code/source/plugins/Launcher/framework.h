#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include "ConfigOption.h"

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

wstring PLAYERDATA_FILE_STRING = DirPath() + L"\\plugins\\playerdata.ini";
LPCWSTR PLAYERDATA_FILE = PLAYERDATA_FILE_STRING.c_str();

LPCWSTR PATCHES_SECTION = L"patches";
LPCWSTR GRAPHICS_SECTION = L"graphics";
LPCWSTR RESOLUTION_SECTION = L"resolution";
LPCWSTR LAUNCHER_SECTION = L"launcher";
LPCWSTR COMPONENTS_SECTION = L"components";
LPCWSTR PLAYERDATA_SECTION = L"playerdata";

int nSkipLauncher = GetPrivateProfileIntW(L"launcher", L"skip", FALSE, CONFIG_FILE);


std::vector<DEVMODEW> getScreenModes() {
	static std::vector<DEVMODEW> outVec = std::vector<DEVMODEW>();

	DEVMODEW dm = { 0 };
	dm.dmSize = sizeof(dm);
	for (int iModeNum = 0; EnumDisplaySettingsW(NULL, iModeNum, &dm) != 0; iModeNum++)
	{
		outVec.push_back(dm);
	}

	return outVec;
}

std::vector<resolution> getScreenResolutionsVec(std::vector<DEVMODEW> &screenModes) {
	static std::vector<resolution> outVec = std::vector<resolution>();

	for (DEVMODEW &dm : screenModes)
	{
		resolution res = resolution(dm.dmPelsWidth, dm.dmPelsHeight);

		if (std::find(outVec.begin(), outVec.end(), res) == outVec.end()) {
			outVec.push_back(res);
		}
	}

	std::sort(outVec.begin(), outVec.end());

	return outVec;
}

static std::vector<int> getScreenDepthsVec(std::vector<DEVMODEW> &screenModes) {
	static std::vector<int> outVec = std::vector<int>();

	for (DEVMODEW &dm : screenModes)
	{
		int depth = dm.dmBitsPerPel;

		if (depth < 24)
			continue;

		if (std::find(outVec.begin(), outVec.end(), depth) == outVec.end()) {
			outVec.push_back(depth);
		}
	}

	std::sort(outVec.begin(), outVec.end());

	return outVec;
}

static std::vector<int> getScreenRatesVec(std::vector<DEVMODEW> &screenModes) {
	static std::vector<int> outVec = std::vector<int>();

	for (DEVMODEW &dm : screenModes)
	{
		int rate = dm.dmDisplayFrequency;

		if (std::find(outVec.begin(), outVec.end(), rate) == outVec.end()) {
			outVec.push_back(rate);
		}
	}

	std::sort(outVec.begin(), outVec.end());

	return outVec;
}

std::vector<DEVMODEW> screenModes = getScreenModes();


DropdownOption* DisplayModeDropdown = new DropdownOption(L"display", RESOLUTION_SECTION, CONFIG_FILE, L"Display:", L"Sets the window/screen mode.", 0, std::vector<LPCWSTR>({ L"Windowed", L"Borderless", L"Fullscreen" }));
ResolutionOption* DisplayResolutionOption = new ResolutionOption(L"width", L"height", RESOLUTION_SECTION, CONFIG_FILE, L"Resolution:", L"Sets the display resolution.", resolution(1280, 720), getScreenResolutionsVec(screenModes));

ConfigOptionBase* screenResolutionArray[] = {
	DisplayModeDropdown,
	DisplayResolutionOption,
	new EditableDropdownNumberOption(L"bitdepth", RESOLUTION_SECTION, CONFIG_FILE, L"Bit Depth:", L"Sets the display bit depth.", 32, getScreenDepthsVec(screenModes)),
	new EditableDropdownNumberOption(L"refreshrate", RESOLUTION_SECTION, CONFIG_FILE, L"Refresh Rate:", L"Sets the display refresh rate.", 60, getScreenRatesVec(screenModes)),
};

BooleanOption* InternalResolutionCheckbox = new BooleanOption(L"r.enable", RESOLUTION_SECTION, CONFIG_FILE, L"Enable", L"Enable or disable custom internal resolution.", false, false);
ResolutionOption* InternalResolutionOption = new ResolutionOption(L"r.width", L"r.height", RESOLUTION_SECTION, CONFIG_FILE, L"Resolution:", L"Sets the internal resolution.", resolution(1280, 720), std::vector<resolution>({ resolution(640,480), resolution(800,600), resolution(960,720), resolution(1280,720), resolution(1920,1080), resolution(2560,1440), resolution(3840,2160), resolution(5120,2880), resolution(7680,4320) }));

ConfigOptionBase* internalResolutionArray[] = {
	InternalResolutionCheckbox,
	InternalResolutionOption
};

ConfigOptionBase* optionsArray[] = {
	new BooleanOption(L"cursor", PATCHES_SECTION, CONFIG_FILE, L"Cursor", L"Enable or disable the mouse cursor.", true, false),

	new BooleanOption(L"TAA", GRAPHICS_SECTION, CONFIG_FILE, L"TAA", L"Temporal Anti-Aliasing", true, false),
	new BooleanOption(L"MLAA", GRAPHICS_SECTION, CONFIG_FILE, L"MLAA", L"Morphological Anti-Aliasing", true, false),

	new BooleanOption(L"hide_freeplay", PATCHES_SECTION, CONFIG_FILE, L"Hide Freeplay", L"Hide Freeplay text.", false, false),
	new BooleanOption(L"hide_volume", PATCHES_SECTION, CONFIG_FILE, L"Hide Volume Buttons", L"Hide the volume and SE control buttons.", false, false),
	new BooleanOption(L"no_movies", PATCHES_SECTION, CONFIG_FILE, L"Disable Movies", L"Disables movies (enable this if the game hangs when loading certain PVs).", false, false),
	new BooleanOption(L"no_pv_ui", PATCHES_SECTION, CONFIG_FILE, L"Disable PV UI", L"Removes the photo controls during PV playback.", false, false),
	new BooleanOption(L"no_lyrics", PATCHES_SECTION, CONFIG_FILE, L"Disable Lyrics", L"Disables showing lyrics.", false, false),
	new BooleanOption(L"hide_pv_watermark", PATCHES_SECTION, CONFIG_FILE, L"Hide PV Watermark", L"Hides the watermark that's usually shown in PV viewing mode.", false, false),
	new BooleanOption(L"no_error", PATCHES_SECTION, CONFIG_FILE, L"Disable Error Banner", L"Disables the error banner on the attract screen.", true, false),

	new BooleanOption(L"skip", LAUNCHER_SECTION, CONFIG_FILE, L"Skip Launcher", L"Forces the launcher to be skipped, you can also use the --launch parameter instead of this.", false, false),

	new DropdownOption(L"status_icons", PATCHES_SECTION, CONFIG_FILE, L"Status Icons:", L"Set the state of card reader and network status icons.", 3, std::vector<LPCWSTR>({ L"Default", L"Hidden", L"Error", L"OK", L"Partial OK" })),
	
	new NumericOption(L"FPS.Limit", GRAPHICS_SECTION, CONFIG_FILE, L"FPS Limit:", L"Using the FPS limit requires the \"FPS Limiter\" component to be enabled.", 60, 0, INT_MAX),

	new StringOption(L"command_line", LAUNCHER_SECTION, CONFIG_FILE, L"Command Line:", L"Allows setting command line parameters for the game when using the launcher.\nDisabling the launcher will bypass this.", L"", false),
};

ConfigOptionBase* playerdataArray[] = {
	new StringOption(L"player_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Player Name:", L"Player name shown in game.", L"ＮＯ－ＮＡＭＥ", true),
	new StringOption(L"level_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Level Name:", L"Level (plate) name shown in game.", L"忘れないでね私の声を", true),

	new NumericOption(L"level_plate_id", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Level Plate:", L"Sets the level background image (plate).", 0, 0, INT_MAX),
	new NumericOption(L"skin_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Skin:", L"Sets the gameplay UI skin.", 0, 0, INT_MAX),

	new NumericOption(L"btn_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Button Sound:", L"Sets the sound effect for buttons.\n-1 = song default", -1, -1, INT_MAX),
	new NumericOption(L"slide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Slide Sound:", L"Sets the sound effect for slides.\n-1 = song default", -1, -1, INT_MAX),
	new NumericOption(L"chainslide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Chainslide Sound:", L"Sets the sound effect for chain slides.\n-1 = song default", -1, -1, INT_MAX),

	new BooleanOption(L"border_great", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Clear Border (Great)", L"Shows the clear border for a great rating on the progress bar.", true, true),
	new BooleanOption(L"border_excellent", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Clear Border (Excellent)", L"Shows the clear border for an excellent rating on the progress bar.", true, true),

	new BooleanOption(L"use_card", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Use Card", L"Enables IC card. This allows module selection.", false, true),
	new BooleanOption(L"module_card_workaround", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Module Selection Workaround", L"Allows module selection without card and tries to improve menu performance.\n(BETA)", true, true),

	new BooleanOption(L"gamemode_options", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Game Modifiers", L"Allows use of game mode modifiers (hi-speed, hidden, and sudden).", true, true),
};


ConfigOptionBase* componentsArray[] = {
	new BooleanOption(L"input_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Input Emulator", L"Emulates input through keyboard and/or mouse.", false, true),
	new BooleanOption(L"touch_slider_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Slider Emulator", L"Emulates slider through keyboard and/or mouse.", false, true),
	new BooleanOption(L"touch_panel_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Touch Panel Emulator", L"Emulates touch panel through mouse.", false, true),

	new BooleanOption(L"sys_timer", COMPONENTS_SECTION, COMPONENTS_FILE, L"Timer Freeze", L"Freezes the PV select timer at 39 seconds.", false, true),

	new BooleanOption(L"player_data_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"Player Data Manager", L"Loads user-defined values into the PlayerData struct.\nRequired for modules and game mode modifiers.", false, true),

	new BooleanOption(L"frame_rate_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"Frame Rate Manager", L"Adjusts animations to the correct speed at different frame rates.\nOnly needed when FPS isn't locket at 60.", false, true),

	new BooleanOption(L"stage_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"Stage Manager", L"Allows for playing unlimited songs per session.", false, true),

	new BooleanOption(L"fast_loader", COMPONENTS_SECTION, COMPONENTS_FILE, L"Fast Loader", L"Skip or speed up unnecessary loading steps.", false, true),

	new BooleanOption(L"camera_controller", COMPONENTS_SECTION, COMPONENTS_FILE, L"Camera Controller", L"Enables freecam (toggled using F3).\nWASD to move, SPACE/CTRL for up/down, Q/R to rotate, R/F for zoom.\nHolding SHIFT/ALT changes control speed.", false, true),

	new BooleanOption(L"scale_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"Scale Component", L"Scales the graphics output framebuffer to fill the screen/window.", false, true),

	new BooleanOption(L"debug_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"Debug Component", L"Allows for changing game state (F4-F8 keys), using dev GUI and tests, and speeding up 2d animations/menus (hold SHIFT+TAB).", false, true),

	new BooleanOption(L"fps_limiter", COMPONENTS_SECTION, COMPONENTS_FILE, L"FPS Limiter", L"Lets you set a framerate cap. The value of the limit is in the options tab.", false, true),

	new BooleanOption(L"target_inspector", COMPONENTS_SECTION, COMPONENTS_FILE, L"Target Inspector", L"Enables hold transfers.", false, true),
};

bool IsLineInFile(LPCSTR searchLine, LPCWSTR fileName)
{
	bool result = false;

	std::ifstream fileStream(fileName);

	if (!fileStream.is_open())
		return false;

	std::string line;

	while (std::getline(fileStream, line))
	{
		if (line.compare(searchLine) == 0)
		{
			result = true;
			break;
		}
	}

	fileStream.close();
	return result;
}

void PrependFile(LPCSTR newStr, LPCWSTR fileName)
{
	std::fstream fileStream(fileName);

	if (!fileStream.is_open())
		return;


	std::string origStr;

	// this is apparently more efficient than just going straight into the string
	fileStream.seekg(0, std::ios::end);
	origStr.reserve(fileStream.tellg());
	fileStream.seekg(0, std::ios::beg);
	origStr.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

	std::string outStr = newStr;
	outStr += origStr;

	fileStream.seekg(0, std::ios::beg);
	fileStream << outStr;
	fileStream.close();
}


// used to trick Optimus into switching to the NVIDIA GPU
HMODULE nvcudaModule = LoadLibraryW(L"nvcuda.dll");
// cuInit actually returns a CUresult, but we don't really care about it
void(WINAPI * cuInit)(unsigned int flags) = (void(WINAPI*)(unsigned int flags))GetProcAddress(nvcudaModule, "cuInit");

// needed to close the OpenGL window (freeglut only)
HMODULE glutModule = LoadLibraryW(L"glut32.dll");
void(__stdcall * glutMainLoopEventDynamic)() = (void(__stdcall*)())GetProcAddress(glutModule, "glutMainLoopEvent");