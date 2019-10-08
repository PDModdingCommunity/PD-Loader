#include "PluginConfigApi.h"

using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"TLAC";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Provides keyboard/mouse/controller input, player data management, bug fixes for running on more PCs, etc.\nThis plugin is required.";
}