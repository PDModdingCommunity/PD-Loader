#pragma once
#include <windows.h>

enum AccentState {
	ACCENT_DISABLED = 0,
	ACCENT_ENABLE_GRADIENT = 1,
	ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
	ACCENT_ENABLE_BLURBEHIND = 3,
	ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
	ACCENT_ENABLE_HOSTBACKDROP = 5,
	ACCENT_INVALID_STATE = 6
};

enum WindowCompositionAttribute {
	WCA_ACCENT_POLICY = 19,
	WCA_USEDARKMODECOLORS = 26
};

struct AccentPolicy {
	AccentState accentState;
	int accentFlags;
	int gradientColor;
	int animationId;
};

struct WindowCompositionAttributeData {
	WindowCompositionAttribute attribute;
	PVOID pData;
	ULONG dataSize;
};

bool setBlur(HWND hWnd, AccentState accentState)
{
	HKEY CurrentVersion;
	auto result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &CurrentVersion);
	if (result == ERROR_SUCCESS)
	{
		DWORD DataSize = 32;
		WCHAR* DataPtr = (WCHAR*)malloc(DataSize);
		memset(DataPtr, 0, DataSize);
		bool queryBuild = RegQueryValueEx(CurrentVersion, L"CurrentBuild", NULL, NULL, (LPBYTE)DataPtr, &DataSize) == ERROR_SUCCESS;
		RegCloseKey(CurrentVersion);
		if (queryBuild)
		{
			int BuildNum = _wtoi(DataPtr);
			free(DataPtr);
			//MessageBox::Show(BuildNum.ToString());
			if (BuildNum < 19042) return 0;
		}
		else
		{
			free(DataPtr);
			return 0;
		}
	}
	else return 0;

	const HINSTANCE hModule_user32 = LoadLibrary(TEXT("user32.dll"));
	if (hModule_user32)
	{
		typedef BOOL(WINAPI* p_SetWindowCompositionAttribute)(HWND, WindowCompositionAttributeData*);

		const p_SetWindowCompositionAttribute SetWindowCompositionAttribute = (p_SetWindowCompositionAttribute)GetProcAddress(hModule_user32, "SetWindowCompositionAttribute");

		bool result = false;

		if (SetWindowCompositionAttribute)
		{
			AccentPolicy policy;
			policy.accentState = accentState;
			policy.accentFlags = 2;
			policy.gradientColor = 0x01000000;
			policy.animationId = 0;

			WindowCompositionAttributeData data;
			data.attribute = WCA_ACCENT_POLICY;
			data.pData = &policy;
			data.dataSize = sizeof(AccentPolicy);

			result = SetWindowCompositionAttribute(hWnd, &data);
		}
		FreeLibrary(hModule_user32);

		return result;
	}

	return 0;
}

void setDarkTheme(HWND hWnd, bool dark=true)
{
	const HINSTANCE hModule_dwmapi = LoadLibrary(TEXT("dwmapi.dll"));
	if (hModule_dwmapi)
	{
		typedef BOOL(WINAPI* p_DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
		const p_DwmSetWindowAttribute DwmSetWindowAttribute = (p_DwmSetWindowAttribute)GetProcAddress(hModule_dwmapi, "DwmSetWindowAttribute");

		if (DwmSetWindowAttribute)
		{
			LONG useDarkModeColors = dark;
			if(DwmSetWindowAttribute(hWnd, 20, &useDarkModeColors, sizeof(useDarkModeColors)))
				DwmSetWindowAttribute(hWnd, 19, &useDarkModeColors, sizeof(useDarkModeColors));
		}
		FreeLibrary(hModule_dwmapi);
	}
}
