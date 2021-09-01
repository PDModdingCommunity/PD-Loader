#pragma once
#include "windows.h"
#include <string>
#include <vector>

std::string TrimString(const std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);

	if (strBegin == std::string::npos)
		return "";

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::vector<std::string> SplitString(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos)
			pos = str.length();

		std::string token = str.substr(prev, pos - prev);

		if (!token.empty())
			tokens.push_back(token);

		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());

	return tokens;
}

void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

void InjectInt(void* address, const int data)
{
	const size_t byteCount = sizeof(int);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(int*)address = data;
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

void InjectLong(void* address, const long data)
{
	const size_t byteCount = sizeof(long);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	*(long*)address = data;
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

const LPCWSTR CONFIG_FILE = L"plugins\\config.ini";
const LPCWSTR CONFIG_FILE_NAME = L".\\config.ini";

auto nCursor = GetPrivateProfileIntW(L"patches", L"cursor", TRUE, CONFIG_FILE);
auto nHideFreeplay = GetPrivateProfileIntW(L"patches", L"hide_freeplay", FALSE, CONFIG_FILE);
auto nFreeplay = GetPrivateProfileIntW(L"patches", L"freeplay", TRUE, CONFIG_FILE);
auto nPDLoaderText = GetPrivateProfileIntW(L"patches", L"pdloadertext", TRUE, CONFIG_FILE);
auto nStatusIcons = GetPrivateProfileIntW(L"patches", L"status_icons", 0, CONFIG_FILE);
auto nHidePVWatermark = GetPrivateProfileIntW(L"patches", L"hide_pv_watermark", FALSE, CONFIG_FILE);
auto nNoPVUi = GetPrivateProfileIntW(L"patches", L"no_pv_ui", FALSE, CONFIG_FILE);
auto nHideVolCtrl = GetPrivateProfileIntW(L"patches", L"hide_volume", FALSE, CONFIG_FILE);
auto nNoLyrics = GetPrivateProfileIntW(L"patches", L"no_lyrics", FALSE, CONFIG_FILE);
auto nNoMovies = GetPrivateProfileIntW(L"patches", L"no_movies", FALSE, CONFIG_FILE);
auto nNoError = GetPrivateProfileIntW(L"patches", L"no_error", FALSE, CONFIG_FILE);
auto nNoTimer = GetPrivateProfileIntW(L"patches", L"no_timer", TRUE, CONFIG_FILE);
auto nNoTimerSprite = GetPrivateProfileIntW(L"patches", L"no_timer_sprite", TRUE, CONFIG_FILE);
auto nEStageManager = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager", 0, CONFIG_FILE);
auto nEStageManagerEncore = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager_encore", TRUE, CONFIG_FILE);
auto nUnlockPseudo = GetPrivateProfileIntW(L"patches", L"unlock_pseudo", FALSE, CONFIG_FILE);
auto nCard = GetPrivateProfileIntW(L"patches", L"card", TRUE, CONFIG_FILE);
auto nHardwareSlider = GetPrivateProfileIntW(L"patches", L"hardware_slider", FALSE, CONFIG_FILE);
//auto nOGLPatchA = GetPrivateProfileIntW(L"patches", L"opengl_patch_a", FALSE, CONFIG_FILE);
//auto nOGLPatchB = GetPrivateProfileIntW(L"patches", L"opengl_patch_b", FALSE, CONFIG_FILE);
auto nTAA = GetPrivateProfileIntW(L"graphics", L"taa", TRUE, CONFIG_FILE);
auto nMLAA = GetPrivateProfileIntW(L"graphics", L"mlaa", TRUE, CONFIG_FILE);
auto nDoF = GetPrivateProfileIntW(L"graphics", L"dof", TRUE, CONFIG_FILE);
auto nMAG = GetPrivateProfileIntW(L"graphics", L"mag", 0, CONFIG_FILE);
auto nStereo = GetPrivateProfileIntW(L"patches", L"stereo", TRUE, CONFIG_FILE);
auto nBuiltinPatches = GetPrivateProfileIntW(L"global", L"builtin_patches", TRUE, CONFIG_FILE);
auto nCustomPatches = GetPrivateProfileIntW(L"global", L"custom_patches", TRUE, CONFIG_FILE);
auto nQuickStart = GetPrivateProfileIntW(L"patches", L"quick_start", 1, CONFIG_FILE);
auto nNoScrollingSfx = GetPrivateProfileIntW(L"patches", L"no_scrolling_sfx", FALSE, CONFIG_FILE);
auto nNoHandScaling = GetPrivateProfileIntW(L"patches", L"no_hand_scaling", FALSE, CONFIG_FILE);
auto nDefaultHandSize = GetPrivateProfileIntW(L"patches", L"default_hand_size", -1, CONFIG_FILE);
auto nSingMissed = GetPrivateProfileIntW(L"patches", L"sing_missed", FALSE, CONFIG_FILE);
auto nForceMouth = GetPrivateProfileIntW(L"patches", L"force_mouth", 0, CONFIG_FILE);
auto nForceExpressions = GetPrivateProfileIntW(L"patches", L"force_expressions", 0, CONFIG_FILE);
auto nForceLook = GetPrivateProfileIntW(L"patches", L"force_look", 0, CONFIG_FILE);
auto nNpr1 = GetPrivateProfileIntW(L"graphics", L"npr1", 0, CONFIG_FILE);
auto nReflections = GetPrivateProfileIntW(L"graphics", L"reflections", TRUE, CONFIG_FILE);
auto nGamma = GetPrivateProfileIntW(L"graphics", L"gamma", 100, CONFIG_FILE);
auto nShadows = GetPrivateProfileIntW(L"graphics", L"shadows", TRUE, CONFIG_FILE);
auto nPunchthrough = GetPrivateProfileIntW(L"graphics", L"punchthrough", TRUE, CONFIG_FILE);
auto nGlare = GetPrivateProfileIntW(L"graphics", L"glare", TRUE, CONFIG_FILE);
auto nShader = GetPrivateProfileIntW(L"graphics", L"shader", TRUE, CONFIG_FILE);
auto n2D = GetPrivateProfileIntW(L"graphics", L"2D", FALSE, CONFIG_FILE);
auto nNoDelete = GetPrivateProfileIntW(L"patches", L"prevent_data_deletion", FALSE, CONFIG_FILE);
auto nReflectResWidth = GetPrivateProfileIntW(L"graphics", L"reflect_res_width", 512, CONFIG_FILE);
auto nReflectResHeight = GetPrivateProfileIntW(L"graphics", L"reflect_res_height", 256, CONFIG_FILE);
auto nRefractResWidth = GetPrivateProfileIntW(L"graphics", L"refract_res_width", 512, CONFIG_FILE);
auto nRefractResHeight = GetPrivateProfileIntW(L"graphics", L"refract_res_height", 256, CONFIG_FILE);
auto nLagCompensation = GetPrivateProfileIntW(L"graphics", L"lag_compensation", 0, CONFIG_FILE);
auto nNoMessageBar = GetPrivateProfileIntW(L"patches", L"no_message_bar", FALSE, CONFIG_FILE);
auto nNoStageText = GetPrivateProfileIntW(L"patches", L"no_stage_text", FALSE, CONFIG_FILE);
auto nNoOpd = GetPrivateProfileIntW(L"patches", L"no_opd", FALSE, CONFIG_FILE);
auto nDwguiScaling = GetPrivateProfileIntW(L"patches", L"dwgui_scaling", FALSE, CONFIG_FILE);
