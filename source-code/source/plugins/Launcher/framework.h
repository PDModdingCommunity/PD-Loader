#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include "ConfigOption.h"
#include "PluginConfig.h"
#include <iostream>

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

wstring DIVA_DIRPATH_STRING = DirPath();
LPCWSTR DIVA_DIRPATH = DIVA_DIRPATH_STRING.c_str();

wstring DIVA_EXECUTABLE_LAUNCH_STRING = DIVA_EXECUTABLE_STRING + L" --launch";
LPWSTR DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING.c_str());

wstring PLUGINS_DIR = DirPath() + L"\\plugins";
wstring PATCHES_DIR = DirPath() + L"\\patches";

wstring CONFIG_FILE_STRING = PLUGINS_DIR + L"\\config.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

wstring COMPONENTS_FILE_STRING = PLUGINS_DIR + L"\\components.ini";
LPCWSTR COMPONENTS_FILE = COMPONENTS_FILE_STRING.c_str();

wstring PLAYERDATA_FILE_STRING = PLUGINS_DIR + L"\\playerdata.ini";
LPCWSTR PLAYERDATA_FILE = PLAYERDATA_FILE_STRING.c_str();

wstring KEYCONFIG_FILE_STRING = PLUGINS_DIR + L"\\keyconfig.ini";
LPCWSTR KEYCONFIG_FILE = KEYCONFIG_FILE_STRING.c_str();

LPCWSTR PATCHES_SECTION = L"patches";
LPCWSTR GRAPHICS_SECTION = L"graphics";
LPCWSTR RESOLUTION_SECTION = L"resolution";
LPCWSTR LAUNCHER_SECTION = L"launcher";
LPCWSTR COMPONENTS_SECTION = L"components";
LPCWSTR PLAYERDATA_SECTION = L"playerdata";
LPCWSTR KEYCONFIG_SECTION = L"keyconfig";

int nSkipLauncher = GetPrivateProfileIntW(LAUNCHER_SECTION, L"skip", FALSE, CONFIG_FILE);
int nNoGPUDialog = GetPrivateProfileIntW(LAUNCHER_SECTION, L"no_gpu_dialog", FALSE, CONFIG_FILE);


resolution getCurrentScreenResolution() {
	return resolution(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

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

	const resolution defaultres(1280, 720);
	if (std::find(outVec.begin(), outVec.end(), defaultres) == outVec.end()) {
		outVec.push_back(defaultres);
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

const std::vector<resolution> internalResOptions = { resolution(320,240), resolution(426,240), resolution(640,480), resolution(854,480), resolution(960,540), resolution(1280,720),  resolution(1366,768), resolution(1600,900), resolution(1920,1080), resolution(2560,1440), resolution(3200,1800), resolution(3840,2160), resolution(5120,2880), resolution(7680,4320) };

DropdownOption* DisplayModeDropdown = new DropdownOption(L"display", RESOLUTION_SECTION, CONFIG_FILE, L"Display:", L"Sets the window/screen mode.", 1, std::vector<LPCWSTR>({ L"Windowed", L"Fast", L"Exclusive", L"Safe" }));
ResolutionOption* DisplayResolutionOption = new ResolutionOption(L"width", L"height", RESOLUTION_SECTION, CONFIG_FILE, L"Resolution:", L"Sets the display resolution.", resolution(-1, -1), getScreenResolutionsVec(screenModes), true, RESOPT_INCLUDE_MATCH_SCREEN);
DropdownNumberOption* RefreshRateOption = new DropdownNumberOption(L"refreshrate", RESOLUTION_SECTION, CONFIG_FILE, L"Refresh Rate:", L"Sets the display refresh rate.", 60, getScreenRatesVec(screenModes), true);

ConfigOptionBase* screenResolutionArray[] = {
	DisplayModeDropdown,
	DisplayResolutionOption,
	//new DropdownNumberOption(L"bitdepth", RESOLUTION_SECTION, CONFIG_FILE, L"Bit Depth:", L"Sets the display bit depth.", 32, getScreenDepthsVec(screenModes), true),
	RefreshRateOption,
};

BooleanOption* InternalResolutionCheckbox = new BooleanOption(L"r.enable", RESOLUTION_SECTION, CONFIG_FILE, L"Enable", L"Enable or disable custom internal resolution.", false, false);
ResolutionOption* InternalResolutionOption = new ResolutionOption(L"r.width", L"r.height", RESOLUTION_SECTION, CONFIG_FILE, L"Resolution:", L"Sets the internal resolution (instead of 1280x720).", resolution(1920, 1080), internalResOptions, true, RESOPT_INCLUDE_MATCH_WINDOW);

ConfigOptionBase* internalResolutionArray[] = {
	InternalResolutionCheckbox,
	InternalResolutionOption
};

ConfigOptionBase* graphicsArray[] = {
	new DropdownOption(L"model", L"GPU", CONFIG_FILE, L"NVIDIA GPU:", L"Select your NVIDIA GPU's architecture to apply the necessary workarounds.\n\nNOTE: Automatic detection does not currently work on GNU/Linux.", -1, std::vector<LPCWSTR>({ L"Automatic", L"Kepler", L"Maxwell/Pascal", L"Turing", L"Ampere" }), -1),
	new BooleanOption(L"TAA", GRAPHICS_SECTION, CONFIG_FILE, L"TAA", L"Temporal Anti-Aliasing", true, false),
	new BooleanOption(L"MLAA", GRAPHICS_SECTION, CONFIG_FILE, L"MLAA", L"Morphological Anti-Aliasing", true, false),
	new DropdownOption(L"MAG", GRAPHICS_SECTION, CONFIG_FILE, L"Filter:", L"Image filter.\n\nBilinear: default filter\nNearest-neighbour: sharpest, but blocky\nSharpen: sharp filter\nCone: smooth filter", 0, std::vector<LPCWSTR>({ L"Bilinear", L"Nearest-neighbour", L"Sharpen", L"Cone" })),
	new BooleanOption(L"DOF", GRAPHICS_SECTION, CONFIG_FILE, L"Depth of Field", L"Blurs the background. Disable for better performance.", true, false),
	new BooleanOption(L"reflections", GRAPHICS_SECTION, CONFIG_FILE, L"Reflections", L"Enable or disable reflections.", true, false),
	new ResolutionOption(L"reflect_res_width", L"reflect_res_height", GRAPHICS_SECTION, CONFIG_FILE, L"Reflection Res:", L"Sets the reflection buffer resolution (instead of 512x256).", resolution(-1, -1), std::vector<resolution>({resolution(256,256), resolution(512,256), resolution(512,512), resolution(1024,1024), resolution(2048,2048), resolution(4096,4096)}), true, RESOPT_INCLUDE_DEFAULT),
	new ResolutionOption(L"refract_res_width", L"refract_res_height", GRAPHICS_SECTION, CONFIG_FILE, L"Refraction Res:", L"Sets the refraction buffer resolution (instead of 512x256).", resolution(-1, -1), std::vector<resolution>({resolution(256,256), resolution(512,256), resolution(512,512), resolution(1024,1024), resolution(2048,2048), resolution(4096,4096)}), true, RESOPT_INCLUDE_DEFAULT),
	new NumericOption(L"gamma", GRAPHICS_SECTION, CONFIG_FILE, L"Gamma:", L"Increase to darken shadows.\nSet to 125 for a console-like experience.\n\nDefault: 0 or 100", 100, 0, 200),
	new BooleanOption(L"shadows", GRAPHICS_SECTION, CONFIG_FILE, L"Shadows", L"Enable or disable shadows.", true, false),
	new BooleanOption(L"punchthrough", GRAPHICS_SECTION, CONFIG_FILE, L"Transparent Meshes", L"Show transparent meshes.", true, false),
	new BooleanOption(L"glare", GRAPHICS_SECTION, CONFIG_FILE, L"Glare", L"Enable or disable glare.", true, false),
	new BooleanOption(L"shader", GRAPHICS_SECTION, CONFIG_FILE, L"Shader", L"Enable or disable high-quality shaders.", true, false),
	new DropdownOption(L"NPR1", GRAPHICS_SECTION, CONFIG_FILE, L"Toon (F9):", L"NPR1 shader\n\nPress F9 to toggle.", 0, std::vector<LPCWSTR>({ L"Default", L"Force on", L"Force off" })),
	new BooleanOption(L"2D", GRAPHICS_SECTION, CONFIG_FILE, L"Disable 3D rendering", L"Disable all 3D passes.\n\nWARNING: The extended data will be deleted unless Prevent Data Deletion is enabled.", false, false),

};

ConfigOptionBase* optionsArray[] = {
	new OptionMetaSectionLabel(L"Compatibility"),
	new BooleanOption(L"no_movies", PATCHES_SECTION, CONFIG_FILE, L"Disable Movies", L"Disable movies (enable this if the game hangs when loading certain PVs).", false, false),
	new BooleanOption(L"mp4_movies", PATCHES_SECTION, CONFIG_FILE, L"Custom MP4 Adv Movies", L"Enable MP4 (instead of WMV) advertise/attract movies.", false, false),
	new BooleanOption(L"cursor", PATCHES_SECTION, CONFIG_FILE, L"Cursor", L"Enable or disable the mouse cursor.", true, false),
	new BooleanOption(L"stereo", PATCHES_SECTION, CONFIG_FILE, L"Stereo", L"Use 2 channels instead of 4 (when not using DivaSound).", true, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Controller Options"),
	new BooleanOption(L"rumble", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"XInput Rumble", L"Enables rumble during chainslides.", true, true),
	new NumericOption(L"xinput_rumble_intensity_left", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"Left motor:", L"Left rumble motor intensity (XInput).\n\nDefault: 8000", 8000, 0, USHRT_MAX),
	new NumericOption(L"xinput_rumble_intensity_right", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"Right motor:", L"Right rumble motor intensity (XInput).\n\nDefault: 4000", 4000, 0, USHRT_MAX),
	new NumericOption(L"xinput_preferred", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"XInput Controller Num:", L"Sets the preferred XInput controller.\nIf unavailable, the next connected controller is used.", 0, 0, 3),
	new BooleanOption(L"hardware_slider", PATCHES_SECTION, CONFIG_FILE, L"Use Hardware Slider", L"Enable this if using a real arcade slider.\n(set the slider to port COM11)", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"PV Selector"),
	new DropdownOption(L"quick_start", PATCHES_SECTION, CONFIG_FILE, L"Quick Start:", L"Skip one or more menus.", 1, std::vector<LPCWSTR>({ L"Disabled", L"Guest", L"Guest + Normal" })),
	new BooleanOption(L"no_scrolling_sfx", PATCHES_SECTION, CONFIG_FILE, L"Disable Scrolling SFX", L"Disable the scrolling sound effect.", false, false),
	new BooleanOption(L"unlock_pseudo", PATCHES_SECTION, CONFIG_FILE, L"Unlock PSEUDO modules (incomplete)", L"Lets you play any PV with any performer.\n(incomplete, recommended modules will default to Miku)", false, false),
	new BooleanOption(L"card", PATCHES_SECTION, CONFIG_FILE, L"Unlock card menu (incomplete)", L"Enables the card menu.\n(incomplete, it doesn't bypass the card prompt)", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Stages/Songs"),
	new NumericOption(L"Enhanced_Stage_Manager", PATCHES_SECTION, CONFIG_FILE, L"Number of Stages:", L"Set the number of stages (0 = default).", 0, 0, INT_MAX),
	new BooleanOption(L"Enhanced_Stage_Manager_Encore", PATCHES_SECTION, CONFIG_FILE, L"Encore", L"Enable encore stages.", true, false),
	new BooleanOption(L"sing_missed", PATCHES_SECTION, CONFIG_FILE, L"Sing Missed", L"Sing missed notes.", false, false),
	new BooleanOption(L"autopause", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"Pause Automatically", L"Pause when focus is lost.", true, true),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"PV Customization"),
	new BooleanOption(L"enable_pv_customization", PATCHES_SECTION, CONFIG_FILE, L"Enable PV Customization", L"Enable the options below.", false, false),
	new DropdownOption(L"force_mouth", PATCHES_SECTION, CONFIG_FILE, L"Mouth Type:", L"Change the mouth animations.", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new DropdownOption(L"force_expressions", PATCHES_SECTION, CONFIG_FILE, L"Expression Type:", L"Change the expressions.", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new DropdownOption(L"force_look", PATCHES_SECTION, CONFIG_FILE, L"Look Type:", L"Change the look animations.", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new BooleanOption(L"no_hand_scaling", PATCHES_SECTION, CONFIG_FILE, L"No Hand Scaling", L"Disable hand scaling.", false, false),
	new NumericOption(L"default_hand_size_uint", PATCHES_SECTION, CONFIG_FILE, L"Default Hand Size:", L"0-10000: default\n12200: PDA\n1000000: maximum", 0, 0, 1000000),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"UI Elements"),
	new BooleanOption(L"hide_volume", PATCHES_SECTION, CONFIG_FILE, L"Hide Volume Buttons", L"Hide the volume and SE control buttons.", false, false),
	new BooleanOption(L"no_pv_ui", PATCHES_SECTION, CONFIG_FILE, L"Disable PV UI", L"Remove the photo controls during PV playback.", false, false),
	new BooleanOption(L"hide_pv_watermark", PATCHES_SECTION, CONFIG_FILE, L"Hide PV Watermark", L"Hide the watermark that's usually shown in PV viewing mode.", false, false),
	new DropdownOption(L"status_icons", PATCHES_SECTION, CONFIG_FILE, L"Top-Right Corner Icons:", L"Set the state of card reader and network status icons.", 3, std::vector<LPCWSTR>({ L"Default", L"Hidden", L"Error", L"OK", L"Partial OK" })),
	new BooleanOption(L"no_lyrics", PATCHES_SECTION, CONFIG_FILE, L"Disable Lyrics", L"Disable showing lyrics.", false, false),
	new BooleanOption(L"no_error", PATCHES_SECTION, CONFIG_FILE, L"Disable Error Banner", L"Disable the error banner on the attract screen.", true, false),
	new BooleanOption(L"hide_freeplay", PATCHES_SECTION, CONFIG_FILE, L"Hide \"FREE PLAY\"/\"CREDIT(S)\"", L"Hide the \"FREE PLAY\"/\"CREDIT(S)\" text.", false, false),
	new BooleanOption(L"freeplay", PATCHES_SECTION, CONFIG_FILE, L"FREE PLAY", L"Show \"FREE PLAY\" instead of \"CREDIT(S)\" and don't require credits.", true, false),
	new BooleanOption(L"pdloadertext", PATCHES_SECTION, CONFIG_FILE, L"PD Loader FREE PLAY", L"Show \"PD Loader\" instead of \"FREE PLAY\".", true, false),
	new BooleanOption(L"no_timer", PATCHES_SECTION, CONFIG_FILE, L"Freeze Timer", L"Disable the timer.", true, false),
	new BooleanOption(L"no_timer_sprite", PATCHES_SECTION, CONFIG_FILE, L"Disable Timer Sprite", L"Disable the timer sprite.", true, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Framerate"),
	new BooleanOption(L"FPS.Limit.LightMode", GRAPHICS_SECTION, CONFIG_FILE, L"Use Lightweight Limiter", L"Makes the FPS limiter use less CPU.\nMay have less consistent performance.", true, false),
	new NumericOption(L"FPS.Limit", GRAPHICS_SECTION, CONFIG_FILE, L"FPS Limit:", L"Allows you to set a frame rate cap. Set to -1 to unlock the frame rate.", 60, -1, INT_MAX),
	new NumericOption(L"frm.motion.rate", GRAPHICS_SECTION, CONFIG_FILE, L"FRM Motion Rate:", L"Sets the motion rate (fps) for the Frame Rate Manager component.\nLarger values should be smoother, but more CPU intensive and possibly buggier.", 300, 1, INT_MAX),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Advanced Compatibility"),
	new BooleanOption(L"opengl_patch_a", LAUNCHER_SECTION, CONFIG_FILE, L"OpenGL Patch A", L"Ignores some OpenGL-related errors. Don't use both patches at the same time unless you're know what you're doing.", false, false),
	new BooleanOption(L"opengl_patch_b", LAUNCHER_SECTION, CONFIG_FILE, L"OpenGL Patch B", L"Ignores some OpenGL-related errors. Don't use both patches at the same time unless you're know what you're doing.", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Loader"),
	new BooleanOption(L"custom_patches", PATCHES_SECTION, CONFIG_FILE, L"Enable Custom Patches", L"Enables all custom patches.", true, false),
	new BooleanOption(L"prevent_data_deletion", PATCHES_SECTION, CONFIG_FILE, L"Prevent Data Deletion", L"Prevents the game from deleting files.", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"Launcher"),
	new BooleanOption(L"dark_launcher", LAUNCHER_SECTION, CONFIG_FILE, L"Dark Launcher", L"Sets the dark colour scheme in the launcher.", false, false),
	new BooleanOption(L"acrylic_blur", LAUNCHER_SECTION, CONFIG_FILE, L"Acrylic Blur", L"Enables acrylic blur in the launcher on Windows 10 20H2 or later.", false, false),
	new BooleanOption(L"no_gpu_dialog", LAUNCHER_SECTION, CONFIG_FILE, L"Disable GPU Warning", L"Disables the warning dialog for unsupported GPUs.", false, false),
	//new BooleanOption(L"ignore_exe_checksum", PATCHES_SECTION, CONFIG_FILE, L"Ignore exe checksum", L"Use at your own risk.", false, false),
	new StringOption(L"command_line", LAUNCHER_SECTION, CONFIG_FILE, L"Command Line:", L"Allows setting command line parameters for the game when using the launcher.\nDisabling the launcher will bypass this.", L"", false),
	new BooleanOption(L"use_divahook_bat", LAUNCHER_SECTION, CONFIG_FILE, L"Use divahook.bat/start.bat", L"Launches divahook.bat/start.bat intead of diva.exe.", false, false),
};

ConfigOptionBase* playerdataArray[] = {
	new StringOption(L"player_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Player Name:", L"Player name shown in game.", L"ＮＯ－ＮＡＭＥ", true),
	new StringOption(L"level_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Level Name:", L"Level (plate) name shown in game.", L"忘れないでね私の声を", true),

	new NumericOption(L"level_plate_id", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Level Plate:", L"Sets the level background image (plate).", 0, 0, INT_MAX),
	new NumericOption(L"level_plate_effect", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Level Plate Effect:", L"Sets the effect on the level background image (plate).", -1, -1, 2),
	new NumericOption(L"skin_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Skin:", L"Sets the gameplay UI skin.", 0, 0, INT_MAX),

	new NumericOption(L"btn_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Button Sound:", L"Sets the sound effect for buttons.\n-1 = song default", -1, -1, INT_MAX),
	new NumericOption(L"slide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Slide Sound:", L"Sets the sound effect for slides.\n-1 = song default", -1, -1, INT_MAX),
	new NumericOption(L"chainslide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Chainslide Sound:", L"Sets the sound effect for chain slides.\n-1 = song default", -1, -1, INT_MAX),
	new NumericOption(L"slidertouch_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Slider Touch Sound:", L"Sets the sound effect for touching the slider with no note.\n-1 = song default", -1, -1, INT_MAX),
	new BooleanOption(L"act_toggle", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Button SE", L"Enables button/slider sounds.", true, true),

	new BooleanOption(L"border_great", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Clear Border (Great)", L"Shows the clear border for a great rating on the progress bar.", true, true),
	new BooleanOption(L"border_excellent", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Clear Border (Excellent)", L"Shows the clear border for an excellent rating on the progress bar.", true, true),
	new BooleanOption(L"border_rival", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Clear Border (Rival)", L"Shows the clear border for beating your rival on the progress bar.", false, true),

	new BooleanOption(L"use_card", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Use Card", L"Enables IC card. This allows module selection.", false, true),
	new BooleanOption(L"module_card_workaround", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Module Selection Workaround", L"Allows module selection without card and tries to improve menu performance.\n(BETA)", true, true),
	new BooleanOption(L"use_pv_module_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Song Specific Modules", L"Allows song-specific module selection.", false, true),
	new BooleanOption(L"use_pv_skin_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Song Specific Skins", L"Allows song-specific skin settings.\nValues stored in skins.ini", false, true),
	new BooleanOption(L"use_pv_sfx_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Song Specific Sound Effects", L"Allows song-specific sound effect settings.\nValues stored in sfx.ini", false, true),

	new BooleanOption(L"gamemode_options", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"Game Modifiers", L"Allows use of game mode modifiers (hi-speed, hidden, and sudden).", true, true),
};


ConfigOptionBase* componentsArray[] = {
	new BooleanOption(L"input_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Input Emulator", L"Emulates input through keyboard and/or mouse.", false, true),
	new BooleanOption(L"touch_slider_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Slider Emulator", L"Emulates slider through keyboard and/or mouse.", false, true),
	new BooleanOption(L"touch_panel_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"Touch Panel Emulator", L"Emulates touch panel through mouse.", false, true),

	new BooleanOption(L"player_data_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"Player Data Manager", L"Loads user-defined values into the PlayerData struct.\nRequired for modules and game mode modifiers.", false, true),

	new BooleanOption(L"frame_rate_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"Frame Rate Manager", L"Adjusts animations to the correct speed at different frame rates.\nOnly needed when FPS isn't locked at 60.", false, true),

	new BooleanOption(L"fast_loader", COMPONENTS_SECTION, COMPONENTS_FILE, L"Fast Loader", L"Skip or speed up unnecessary loading steps.", false, true),

	new NumericOption(L"fast_loader_speed", COMPONENTS_SECTION, COMPONENTS_FILE, L"Fast Loader Speed", L"Set this to 4 or less when playing online.\nDefault: 39", 39, 2, 1024),

	new BooleanOption(L"camera_controller", COMPONENTS_SECTION, COMPONENTS_FILE, L"Camera Controller", L"Enables freecam (toggled using F3).\nWASD to move, SPACE/CTRL for up/down, Q/R to rotate, R/F for zoom.\nHolding SHIFT/ALT changes control speed.", false, true),

	new BooleanOption(L"scale_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"Scale Component", L"Scales the graphics output framebuffer to fill the screen/window.", false, true),

	new BooleanOption(L"debug_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"Debug Component", L"Allows for changing game state (F4-F8 keys), using dev GUI and tests, and speeding up 2d animations/menus (hold SHIFT+TAB).", false, true),

	new BooleanOption(L"target_inspector", COMPONENTS_SECTION, COMPONENTS_FILE, L"Target Inspector", L"Enables hold transfers.", false, true),

	new BooleanOption(L"score_saver", COMPONENTS_SECTION, COMPONENTS_FILE, L"Score Saver", L"Saves high scores to plugins/scores.ini.", false, true),

	new BooleanOption(L"pause", COMPONENTS_SECTION, COMPONENTS_FILE, L"Pause", L"Adds a pause menu.", false, true),
};

bool IsLineInFile(LPCSTR searchLine, LPCWSTR fileName)
{
	bool result = false;

	std::ifstream fileStream(fileName);

	if (!fileStream.is_open())
		return false;

	std::string line;

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

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

	// check for BOM
	std::string BOMcheckLine;
	std::getline(fileStream, BOMcheckLine);
	if (BOMcheckLine.size() >= 3 && BOMcheckLine.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	origStr.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());


	std::string outStr = newStr;
	outStr += origStr;

	fileStream.seekg(0, std::ios::beg);
	fileStream << outStr;
	fileStream.close();
}


struct PluginInfo {
	HMODULE handle;
	std::wstring filename;
	std::wstring name;
	std::wstring description;
	std::wstring builddate;
	std::vector<ConfigOptionBase*> configopts;
};
std::vector<PluginInfo> LoadPlugins()
{
	std::vector<PluginInfo> outvec;

	HANDLE hFind;
	WIN32_FIND_DATAW ffd;

	hFind = FindFirstFileW((PLUGINS_DIR + L"\\*.dva").c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				auto pos = wcslen(ffd.cFileName);

				if (ffd.cFileName[pos - 4] == '.' &&
					(ffd.cFileName[pos - 3] == 'd' || ffd.cFileName[pos - 3] == 'D') &&
					(ffd.cFileName[pos - 2] == 'v' || ffd.cFileName[pos - 2] == 'V') &&
					(ffd.cFileName[pos - 1] == 'a' || ffd.cFileName[pos - 1] == 'A'))
				{
					PluginInfo thisplugin;
					thisplugin.handle = LoadLibraryW((PLUGINS_DIR + L"\\" + ffd.cFileName).c_str());
					thisplugin.filename = ffd.cFileName;

					if (thisplugin.handle == NULL)
						continue;

					auto nameFunc = (LPCWSTR(*)())GetProcAddress(thisplugin.handle, "GetPluginName");
					auto descFunc = (LPCWSTR(*)())GetProcAddress(thisplugin.handle, "GetPluginDescription");
					auto dateFunc = (LPCWSTR(*)())GetProcAddress(thisplugin.handle, "GetBuildDate");
					auto optsFunc = (PluginConfig::PluginConfigArray(*)())GetProcAddress(thisplugin.handle, "GetPluginOptions");

					if (nameFunc != NULL)
						thisplugin.name = nameFunc();
					else
						thisplugin.name = thisplugin.filename;

					if (descFunc != NULL)
						thisplugin.description = descFunc();
					else
						thisplugin.description = (thisplugin.filename + L" Plugin").c_str();

					if (dateFunc != NULL)
						thisplugin.builddate = dateFunc();
					else
						thisplugin.builddate = L"Unknown";

					thisplugin.description += L"\n\nVersion: " + thisplugin.builddate;

					if (optsFunc != NULL)
						thisplugin.configopts = PluginConfig::GetConfigOptionVec(optsFunc());

					// sometimes this might screw with custom button config, so let's just not free stuff
					// FreeLibrary(thisplugin.handle);

					outvec.push_back(thisplugin);
				}
			}
		} while (FindNextFileW(hFind, &ffd));
		FindClose(hFind);
	}

	return outvec;
}
// don't load until actually needed to avoid loading disabled plugins
std::vector<PluginInfo> AllPlugins; // = LoadPlugins();

std::vector<PluginOption*> GetPluginOptions(std::vector<PluginInfo>* plugins)
{
	std::vector<PluginOption*> outvec;

	for (PluginInfo &pi : *plugins)
	{
		PluginOption* opt = new PluginOption(pi.filename.c_str(), L"plugins", CONFIG_FILE, pi.name.c_str(), pi.description.c_str(), true, pi.configopts);
		outvec.push_back(opt);
	}

	return outvec;
}
// don't load until actually needed to avoid loading disabled plugins
std::vector<PluginOption*> AllPluginOpts; // = GetPluginOptions(&AllPlugins);

std::vector<PluginInfo> LoadCustom()
{
	std::vector<PluginInfo> outvec;

	HANDLE hFind;
	WIN32_FIND_DATAW ffd;

	hFind = FindFirstFileW((PATCHES_DIR + L"\\*.p?").c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				auto pos = wcslen(ffd.cFileName);

				if ((ffd.cFileName[pos - 2] == '.' && (ffd.cFileName[pos - 1] == 'p' || ffd.cFileName[pos - 1] == 'P')) ||
					(ffd.cFileName[pos - 3] == '.' && (ffd.cFileName[pos - 2] == 'p' || ffd.cFileName[pos - 2] == 'P') && (ffd.cFileName[pos - 1] == '2')))
				{
					PluginInfo thisplugin;
					thisplugin.filename = ffd.cFileName;

					thisplugin.name = thisplugin.filename;
					if (thisplugin.filename[pos - 1] == '2')
						thisplugin.name.resize(thisplugin.name.size() - 3);
					else
						thisplugin.name.resize(thisplugin.name.size() - 2);
					
					thisplugin.description = L"Custom patch";

					outvec.push_back(thisplugin);
				}
			}
		} while (FindNextFileW(hFind, &ffd));
		FindClose(hFind);
	}

	return outvec;
}
std::vector<PluginInfo> AllCustomPatches; // = LoadCustom();

std::vector<PluginOption*> GetCustomOptions(std::vector<PluginInfo>* patches)
{
	std::vector<PluginOption*> outvec;

	for (PluginInfo& pi : *patches)
	{
		PluginOption* opt = new PluginOption(pi.filename.c_str(), L"plugins", CONFIG_FILE, pi.name.c_str(), pi.description.c_str(), true, pi.configopts);
		outvec.push_back(opt);
	}

	return outvec;
}
// don't load until actually needed to avoid loading disabled plugins
std::vector<PluginOption*> AllCustomOpts; // = GetCustomOptions(&AllCustomPatches);

// used to trick Optimus into switching to the NVIDIA GPU
HMODULE nvcudaModule = LoadLibraryW(L"nvcuda.dll");
// cuInit actually returns a CUresult, but we don't really care about it
void(WINAPI * cuInit)(unsigned int flags) = (void(WINAPI*)(unsigned int flags))GetProcAddress(nvcudaModule, "cuInit");

// needed to close the OpenGL window (freeglut only)
HMODULE glutModule = LoadLibraryW(L"glut32.dll");
void(__stdcall * glutMainLoopEventDynamic)() = (void(__stdcall*)())GetProcAddress(glutModule, "glutMainLoopEvent");


using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Launcher";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"PD Launcher is the window you currently have open.\nYou can disable it if you want, but you'll need to edit config.ini if you ever want to re-enable it.";
}