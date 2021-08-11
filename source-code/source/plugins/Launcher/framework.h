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

LPCWSTR GLOBAL_SECTION = L"global";
LPCWSTR PATCHES_SECTION = L"patches";
LPCWSTR GRAPHICS_SECTION = L"graphics";
LPCWSTR RESOLUTION_SECTION = L"resolution";
LPCWSTR LAUNCHER_SECTION = L"launcher";
LPCWSTR COMPONENTS_SECTION = L"components";
LPCWSTR PLAYERDATA_SECTION = L"playerdata";
LPCWSTR KEYCONFIG_SECTION = L"keyconfig";

int nSkipLauncher = GetPrivateProfileIntW(LAUNCHER_SECTION, L"skip", FALSE, CONFIG_FILE);
int nNoGPUDialog = GetPrivateProfileIntW(LAUNCHER_SECTION, L"no_gpu_dialog", FALSE, CONFIG_FILE);
int nLanguage = GetPrivateProfileIntW(LAUNCHER_SECTION, L"launcher_language", FALSE, CONFIG_FILE);

std::vector<LPCWSTR> languages = std::vector<LPCWSTR>({ L"Automatic", L"en", L"zh-Hans" });

void SetBackCol(Control^ elem, System::Drawing::Color color, System::Windows::Forms::FlatStyle cbxStyle)
{
	Type^ elemType = elem->GetType();

	if (elem->HasChildren)
	{
		for (int i = 0; i < elem->Controls->Count; i++)
		{
			SetBackCol(elem->Controls[i], color, cbxStyle);
		}
	}
	else if (elemType == ComboBox::typeid)
	{
		((ComboBox^)elem)->FlatStyle = cbxStyle;
		elem->BackColor = System::Drawing::Color::White;
	}
	else if (elemType == CheckBox::typeid || elemType == Button::typeid) elem->BackColor = color;
}

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


DropdownOption* DisplayModeDropdown = new DropdownOption(L"display", RESOLUTION_SECTION, CONFIG_FILE, L"DISPLAY_NAME", L"DISPLAY_HINT", 1, std::vector<LPCWSTR>({ L"Windowed", L"Fast", L"Exclusive", L"Safe" }));
ResolutionOption* DisplayResolutionOption = new ResolutionOption(L"width", L"height", RESOLUTION_SECTION, CONFIG_FILE, L"RESOLUTION_NAME", L"RESOLUTION_HINT", resolution(-1, -1), getScreenResolutionsVec(screenModes), true, RESOPT_INCLUDE_MATCH_SCREEN);

ConfigOptionBase* screenResolutionArray[] = {
	DisplayModeDropdown,
	DisplayResolutionOption,
	//new DropdownNumberOption(L"bitdepth", RESOLUTION_SECTION, CONFIG_FILE, L"Bit Depth:", L"Sets the display bit depth.", 32, getScreenDepthsVec(screenModes), true),
	new DropdownNumberOption(L"refreshrate", RESOLUTION_SECTION, CONFIG_FILE, L"REFRESHRATE_NAME", L"REFRESHRATE_HINT", 60, getScreenRatesVec(screenModes), true),
};

BooleanOption* InternalResolutionCheckbox = new BooleanOption(L"r.enable", RESOLUTION_SECTION, CONFIG_FILE, L"R.ENABLE_NAME", L"R.ENABLE_HINT", false, false);
ResolutionOption* InternalResolutionOption = new ResolutionOption(L"r.width", L"r.height", RESOLUTION_SECTION, CONFIG_FILE, L"R.RESOLUTION_NAME", L"R.RESOLUTION_HINT", resolution(1920, 1080), std::vector<resolution>({ resolution(1,1), resolution(320,240), resolution(426,240), resolution(640,480), resolution(854,480), resolution(960,540), resolution(1280,720),  resolution(1366,768), resolution(1600,900), resolution(1920,1080), resolution(2560,1440), resolution(3200,1800), resolution(3840,2160), resolution(5120,2880), resolution(7680,4320) }), true, RESOPT_INCLUDE_MATCH_WINDOW);

ConfigOptionBase* internalResolutionArray[] = {
	InternalResolutionCheckbox,
	InternalResolutionOption
};

ConfigOptionBase* graphicsArray[] = {
	new DropdownOption(L"model", L"GPU", CONFIG_FILE, L"MODEL_NAME", L"MODEL_HINT", -1, std::vector<LPCWSTR>({ L"Automatic", L"Kepler", L"Maxwell", L"Turing", L"Ampere" }), -1),
	new BooleanOption(L"TAA", GRAPHICS_SECTION, CONFIG_FILE, L"TAA_NAME", L"TAA_HINT", true, false),
	new BooleanOption(L"MLAA", GRAPHICS_SECTION, CONFIG_FILE, L"MLAA_NAME", L"MLAA_HINT", true, false),
	new DropdownOption(L"MAG", GRAPHICS_SECTION, CONFIG_FILE, L"MAG_NAME", L"MAG_HINT", 0, std::vector<LPCWSTR>({ L"Bilinear", L"Nearest-neighbour", L"Sharpen", L"Cone" })),
	new BooleanOption(L"DOF", GRAPHICS_SECTION, CONFIG_FILE, L"DOF_NAME", L"DOF_HINT", true, false),
	new BooleanOption(L"reflections", GRAPHICS_SECTION, CONFIG_FILE, L"REFLECTIONS_NAME", L"REFLECTIONS_HINT", true, false),
	new ResolutionOption(L"reflect_res_width", L"reflect_res_height", GRAPHICS_SECTION, CONFIG_FILE, L"REFLECT_RES_WIDTH_NAME", L"REFLECT_RES_WIDTH_HINT", resolution(512, 256), std::vector<resolution>({resolution(256,256), resolution(512,256), resolution(512,512), resolution(1024,1024), resolution(2048,2048), resolution(4096,4096)}), true, (ResolutionOptionOpts)0),
	new ResolutionOption(L"refract_res_width", L"refract_res_height", GRAPHICS_SECTION, CONFIG_FILE, L"REFRACT_RES_WIDTH_NAME", L"REFRACT_RES_WIDTH_HINT", resolution(512, 256), std::vector<resolution>({resolution(256,256), resolution(512,256), resolution(512,512), resolution(1024,1024), resolution(2048,2048), resolution(4096,4096)}), true, (ResolutionOptionOpts)0),
	new BooleanOption(L"shadows", GRAPHICS_SECTION, CONFIG_FILE, L"SHADOWS_NAME", L"SHADOWS_HINT", true, false),
	new BooleanOption(L"punchthrough", GRAPHICS_SECTION, CONFIG_FILE, L"PUNCHTHROUGH_NAME", L"PUNCHTHROUGH_HINT", true, false),
	new BooleanOption(L"glare", GRAPHICS_SECTION, CONFIG_FILE, L"GLARE_NAME", L"GLARE_HINT", true, false),
	new BooleanOption(L"shader", GRAPHICS_SECTION, CONFIG_FILE, L"SHADER_NAME", L"SHADER_HINT", true, false),
	new DropdownOption(L"NPR1", GRAPHICS_SECTION, CONFIG_FILE, L"NPR1_NAME", L"NPR1_HINT", 0, std::vector<LPCWSTR>({ L"Default", L"Force on", L"Force off" })),
	new BooleanOption(L"2D", GRAPHICS_SECTION, CONFIG_FILE, L"2D_NAME", L"2D_HINT", false, false),
};

ConfigOptionBase* optionsArray[] = {
	new OptionMetaSectionLabel(L"SECTION_LOADER"),
	new BooleanOption(L"builtin_patches", GLOBAL_SECTION, CONFIG_FILE, L"BUILTIN_PATCHES_NAME", L"BUILTIN_PATCHES_HINT", true, false),
	new BooleanOption(L"custom_patches", GLOBAL_SECTION, CONFIG_FILE, L"CUSTOM_PATCHES_NAME", L"CUSTOM_PATCHES_HINT", true, false),
	new BooleanOption(L"builtin_render", GLOBAL_SECTION, CONFIG_FILE, L"BUILTIN_RENDER_NAME", L"BUILTIN_RENDER_HINT", true, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_COMPATIBILITY"),
	new BooleanOption(L"no_movies", PATCHES_SECTION, CONFIG_FILE, L"NO_MOVIES_NAME", L"NO_MOVIES_HINT", false, false),
	new BooleanOption(L"stereo", PATCHES_SECTION, CONFIG_FILE, L"STEREO_NAME", L"STEREO_HINT", true, false),
	new BooleanOption(L"prevent_data_deletion", PATCHES_SECTION, CONFIG_FILE, L"PREVENT_DATA_DELETION_NAME", L"PREVENT_DATA_DELETION_HINT", false, false),
	new BooleanOption(L"no_opd", PATCHES_SECTION, CONFIG_FILE, L"NO_OPD_NAME", L"NO_OPD_HINT", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_FRAMERATE"),
	new BooleanOption(L"FPS.Limit.LightMode", GRAPHICS_SECTION, CONFIG_FILE, L"FPS.LIMIT.LIGHTMODE_NAME", L"FPS.LIMIT.LIGHTMODE_HINT", true, false),
	new NumericOption(L"FPS.Limit", GRAPHICS_SECTION, CONFIG_FILE, L"FPS.LIMIT_NAME", L"FPS.LIMIT_HINT", 60, -1, INT_MAX),
	new NumericOption(L"frm.motion.rate", GRAPHICS_SECTION, CONFIG_FILE, L"FRM.MOTION.RATE_NAME", L"FRM.MOTION.RATE_HINT", 300, 1, INT_MAX),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_CONTROLLER_OPTIONS"),
	new BooleanOption(L"rumble", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"RUMBLE_NAME", L"RUMBLE_HINT", true, true),
	new NumericOption(L"xinput_rumble_intensity_left", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"XINPUT_RUMBLE_INTENSITY_LEFT_NAME", L"XINPUT_RUMBLE_INTENSITY_LEFT_HINT", 8000, 0, USHRT_MAX),
	new NumericOption(L"xinput_rumble_intensity_right", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"XINPUT_RUMBLE_INTENSITY_RIGHT_NAME", L"XINPUT_RUMBLE_INTENSITY_RIGHT_HINT", 4000, 0, USHRT_MAX),
	new NumericOption(L"xinput_preferred", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"XINPUT_PREFERRED_NAME", L"XINPUT_PREFERRED_HINT", 0, 0, 3),
	new BooleanOption(L"hardware_slider", PATCHES_SECTION, CONFIG_FILE, L"HARDWARE_SLIDER_NAME", L"HARDWARE_SLIDER_HINT", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_LAUNCHER"),
	new DropdownOption(L"launcher_language",LAUNCHER_SECTION,CONFIG_FILE, L"LAUNCHER_LANGUAGE_NAME",L"LAUNCHER_LANGUAGE_HINT", 0, languages),
	new BooleanOption(L"dark_launcher", LAUNCHER_SECTION, CONFIG_FILE, L"DARK_LAUNCHER_NAME", L"DARK_LAUNCHER_HINT", false, false),
	//new BooleanOption(L"acrylic_blur", LAUNCHER_SECTION, CONFIG_FILE, L"ACRYLIC_BLUR_NAME", L"ACRYLIC_BLUR_HINT", false, false),
	new BooleanOption(L"no_gpu_dialog", LAUNCHER_SECTION, CONFIG_FILE, L"NO_GPU_DIALOG_NAME", L"NO_GPU_DIALOG_HINT", false, false),
	new BooleanOption(L"use_divahook_bat", LAUNCHER_SECTION, CONFIG_FILE, L"USE_DIVAHOOK_BAT_NAME", L"USE_DIVAHOOK_BAT_HINT", false, false),
	new StringOption(L"command_line", LAUNCHER_SECTION, CONFIG_FILE, L"COMMAND_LINE_NAME", L"COMMAND_LINE_HINT", L"", false),
};

ConfigOptionBase* options2Array[] = {
	new OptionMetaSectionLabel(L"SECTION_PV_SELECTOR"),
	new DropdownOption(L"quick_start", PATCHES_SECTION, CONFIG_FILE, L"QUICK_START_NAME", L"QUICK_START_HINT", 1, std::vector<LPCWSTR>({ L"Disabled", L"Guest", L"Guest + Normal" })),
	new BooleanOption(L"freeplay", PATCHES_SECTION, CONFIG_FILE, L"FREEPLAY_NAME", L"FREEPLAY_HINT", true, false),
	new BooleanOption(L"no_scrolling_sfx", PATCHES_SECTION, CONFIG_FILE, L"NO_SCROLLING_SFX_NAME", L"NO_SCROLLING_SFX_HINT", false, false),
	new BooleanOption(L"unlock_pseudo", PATCHES_SECTION, CONFIG_FILE, L"UNLOCK_PSEUDO_NAME", L"UNLOCK_PSEUDO_HINT", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_ESM"),
	new NumericOption(L"Enhanced_Stage_Manager", PATCHES_SECTION, CONFIG_FILE, L"ENHANCED_STAGE_MANAGER_NAME", L"ENHANCED_STAGE_MANAGER_HINT", 0, 0, INT_MAX),
	new BooleanOption(L"Enhanced_Stage_Manager_Encore", PATCHES_SECTION, CONFIG_FILE, L"ENHANCED_STAGE_MANAGER_ENCORE_NAME", L"ENHANCED_STAGE_MANAGER_ENCORE_HINT", true, false),
	new BooleanOption(L"sing_missed", PATCHES_SECTION, CONFIG_FILE, L"SING_MISSED_NAME", L"SING_MISSED_HINT", false, false),
	new BooleanOption(L"autopause", KEYCONFIG_SECTION, KEYCONFIG_FILE, L"AUTOPAUSE_NAME", L"AUTOPAUSE_HINT", true, true),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_UI_ELM"),
	new BooleanOption(L"cursor", PATCHES_SECTION, CONFIG_FILE, L"CURSOR_NAME", L"CURSOR_HINT", true, false),
	new BooleanOption(L"hide_volume", PATCHES_SECTION, CONFIG_FILE, L"HIDE_VOLUME_NAME", L"HIDE_VOLUME_HINT", false, false),
	new BooleanOption(L"no_pv_ui", PATCHES_SECTION, CONFIG_FILE, L"NO_PV_UI_NAME", L"NO_PV_UI_HINT", false, false),
	new BooleanOption(L"hide_pv_watermark", PATCHES_SECTION, CONFIG_FILE, L"HIDE_PV_WATERMARK_NAME", L"HIDE_PV_WATERMARK_HINT", false, false),
	new DropdownOption(L"status_icons", PATCHES_SECTION, CONFIG_FILE, L"STATUS_ICONS_NAME", L"STATUS_ICONS_HINT", 3, std::vector<LPCWSTR>({ L"Default", L"Hidden", L"Error", L"OK", L"Partial OK" })),
	new BooleanOption(L"no_lyrics", PATCHES_SECTION, CONFIG_FILE, L"NO_LYRICS_NAME", L"NO_LYRICS_HINT", false, false),
	new BooleanOption(L"no_error", PATCHES_SECTION, CONFIG_FILE, L"NO_ERROR_NAME", L"NO_ERROR_HINT", true, false),
	new BooleanOption(L"hide_freeplay", PATCHES_SECTION, CONFIG_FILE, L"HIDE_FREEPLAY_NAME", L"HIDE_FREEPLAY_HINT", false, false),
	new BooleanOption(L"pdloadertext", PATCHES_SECTION, CONFIG_FILE, L"PDLOADERTEXT_NAME", L"PDLOADERTEXT_HINT", true, false),
	new BooleanOption(L"no_timer", PATCHES_SECTION, CONFIG_FILE, L"NO_TIMER_NAME", L"NO_TIMER_HINT", true, false),
	new BooleanOption(L"no_timer_sprite", PATCHES_SECTION, CONFIG_FILE, L"NO_TIMER_SPRITE_NAME", L"NO_TIMER_SPRITE_HINT", true, false),
	new BooleanOption(L"no_message_bar", PATCHES_SECTION, CONFIG_FILE, L"NO_MESSAGE_BAR_NAME", L"NO_MESSAGE_BAR_HINT", false, false),
	new BooleanOption(L"no_stage_text", PATCHES_SECTION, CONFIG_FILE, L"NO_STAGE_TEXT_NAME", L"NO_STAGE_TEXT_HINT", false, false),
	new BooleanOption(L"card", PATCHES_SECTION, CONFIG_FILE, L"CARD_NAME", L"CARD_HINT", false, false),
	new BooleanOption(L"dwgui_scaling", PATCHES_SECTION, CONFIG_FILE, L"DWGUI_SCALING_NAME", L"DWGUI_SCALING_HINT", false, false),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),

	new OptionMetaSectionLabel(L"SECTION_PV_PATCHES"),
	new DropdownOption(L"force_mouth", PATCHES_SECTION, CONFIG_FILE, L"FORCE_MOUTH_NAME", L"FORCE_MOUTH_HINT", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new DropdownOption(L"force_expressions", PATCHES_SECTION, CONFIG_FILE, L"FORCE_EXPRESSIONS_NAME", L"FORCE_EXPRESSIONS_HINT", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new DropdownOption(L"force_look", PATCHES_SECTION, CONFIG_FILE, L"FORCE_LOOK_NAME", L"FORCE_LOOK_HINT", 0, std::vector<LPCWSTR>({ L"Default", L"Force PDA", L"Force FT" })),
	new BooleanOption(L"no_hand_scaling", PATCHES_SECTION, CONFIG_FILE, L"NO_HAND_SCALING_NAME", L"NO_HAND_SCALING_HINT", false, false),
	new NumericOption(L"default_hand_size", PATCHES_SECTION, CONFIG_FILE, L"DEFAULT_HAND_SIZE_NAME", L"DEFAULT_HAND_SIZE_HINT", -1, -1, INT_MAX),
	new OptionMetaSeparator(),
	new OptionMetaSpacer(8),
};

ConfigOptionBase* playerdataArray[] = {
	new StringOption(L"player_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"PLAYER_NAME_NAME", L"PLAYER_NAME_HINT", L"ＮＯ－ＮＡＭＥ", true),
	new StringOption(L"level_name", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"LEVEL_NAME_NAME", L"LEVEL_NAME_HINT", L"忘れないでね私の声を", true),

	new NumericOption(L"level_plate_id", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"LEVEL_PLATE_ID_NAME", L"LEVEL_PLATE_ID_HINT", 0, 0, INT_MAX),
	new NumericOption(L"level_plate_effect", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"LEVEL_PLATE_EFFECT_NAME", L"LEVEL_PLATE_EFFECT_HINT", -1, -1, 2),
	new NumericOption(L"skin_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"SKIN_EQUIP_NAME", L"SKIN_EQUIP_HINT", 0, 0, INT_MAX),

	new NumericOption(L"btn_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"BTN_SE_EQUIP_NAME", L"BTN_SE_EQUIP_HINT", -1, -1, INT_MAX),
	new NumericOption(L"slide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"SLIDE_SE_EQUIP_NAME", L"SLIDE_SE_EQUIP_HINT", -1, -1, INT_MAX),
	new NumericOption(L"chainslide_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"CHAINSLIDE_SE_EQUIP_NAME", L"CHAINSLIDE_SE_EQUIP_HINT", -1, -1, INT_MAX),
	new NumericOption(L"slidertouch_se_equip", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"SLIDERTOUCH_SE_EQUIP_NAME", L"SLIDERTOUCH_SE_EQUIP_HINT", -1, -1, INT_MAX),
	new BooleanOption(L"act_toggle", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"ACT_TOGGLE_NAME", L"ACT_TOGGLE_HINT", true, true),

	new BooleanOption(L"border_great", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"BORDER_GREAT_NAME", L"BORDER_GREAT_HINT", true, true),
	new BooleanOption(L"border_excellent", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"BORDER_EXCELLENT_NAME", L"BORDER_EXCELLENT_HINT", true, true),
	new BooleanOption(L"border_rival", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"BORDER_RIVAL_NAME", L"BORDER_RIVAL_HINT", false, true),

	new BooleanOption(L"use_card", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"USE_CARD_NAME", L"USE_CARD_HINT", false, true),
	new BooleanOption(L"module_card_workaround", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"MODULE_CARD_WORKAROUND_NAME", L"MODULE_CARD_WORKAROUND_HINT", true, true),
	new BooleanOption(L"use_pv_module_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"USE_PV_MODULE_EQUIP_NAME", L"USE_PV_MODULE_EQUIP_HINT", false, true),
	new BooleanOption(L"use_pv_skin_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"USE_PV_SKIN_EQUIP_NAME", L"USE_PV_SKIN_EQUIP_HINT", false, true),
	new BooleanOption(L"use_pv_sfx_equip ", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"USE_PV_SFX_EQUIP_NAME", L"USE_PV_SFX_EQUIP_HINT", false, true),

	new BooleanOption(L"gamemode_options", PLAYERDATA_SECTION, PLAYERDATA_FILE, L"GAMEMODE_OPTIONS_NAME", L"GAMEMODE_OPTIONS_HINT", true, true),
};


ConfigOptionBase* componentsArray[] = {
	new BooleanOption(L"input_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"INPUT_EMULATOR_NAME", L"INPUT_EMULATOR_HINT", false, true),
	new BooleanOption(L"touch_slider_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"TOUCH_SLIDER_EMULATOR_NAME", L"TOUCH_SLIDER_EMULATOR_HINT", false, true),
	new BooleanOption(L"touch_panel_emulator", COMPONENTS_SECTION, COMPONENTS_FILE, L"TOUCH_PANEL_EMULATOR_NAME", L"TOUCH_PANEL_EMULATOR_HINT", false, true),

	new BooleanOption(L"player_data_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"PLAYER_DATA_MANAGER_NAME", L"PLAYER_DATA_MANAGER_HINT", false, true),

	new BooleanOption(L"frame_rate_manager", COMPONENTS_SECTION, COMPONENTS_FILE, L"FRAME_RATE_MANAGER_NAME", L"FRAME_RATE_MANAGER_HINT", false, true),

	new BooleanOption(L"fast_loader", COMPONENTS_SECTION, COMPONENTS_FILE, L"FAST_LOADER_NAME", L"FAST_LOADER_HINT", false, true),

	new NumericOption(L"fast_loader_speed", COMPONENTS_SECTION, COMPONENTS_FILE, L"FAST_LOADER_SPEED_NAME", L"FAST_LOADER_SPEED_HINT", 39, 2, 1024),

	new BooleanOption(L"camera_controller", COMPONENTS_SECTION, COMPONENTS_FILE, L"CAMERA_CONTROLLER_NAME", L"CAMERA_CONTROLLER_HINT", false, true),

	new BooleanOption(L"scale_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"SCALE_COMPONENT_NAME", L"SCALE_COMPONENT_HINT", false, true),

	new BooleanOption(L"debug_component", COMPONENTS_SECTION, COMPONENTS_FILE, L"DEBUG_COMPONENT_NAME", L"DEBUG_COMPONENT_HINT", false, true),

	new BooleanOption(L"target_inspector", COMPONENTS_SECTION, COMPONENTS_FILE, L"TARGET_INSPECTOR_NAME", L"TARGET_INSPECTOR_HINT", false, true),

	new BooleanOption(L"score_saver", COMPONENTS_SECTION, COMPONENTS_FILE, L"SCORE_SAVER_NAME", L"SCORE_SAVER_HINT", false, true),

	new BooleanOption(L"pause", COMPONENTS_SECTION, COMPONENTS_FILE, L"PAUSE_NAME", L"PAUSE_HINT", false, true),
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
		PluginOption* opt = new PluginOption(pi.filename.c_str(), L"plugins", CONFIG_FILE, pi.name.c_str(), pi.description.c_str(), false, pi.configopts);
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
		PluginOption* opt = new PluginOption(pi.filename.c_str(), L"plugins", CONFIG_FILE, pi.name.c_str(), pi.description.c_str(), false, pi.configopts);
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