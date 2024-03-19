#include "framework.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")
#include <GL\freeglut.h>
#include <GL\GL.h>
#include <wingdi.h>
#include <windows.h>
#include <timeapi.h>
#include <iostream>
#include <thread>

#include <chrono>

#include "PluginConfigApi.h"

using namespace std::chrono;

unsigned short game_version = 710;

void exitGameClean(int)
{
	switch (game_version)
	{
	case 600:
		*(bool*)0x0000000140E8DF20 = true; // 6.00
		break;
	default:
		*(bool*)0x0000000140EDA6B0 = true; // 7.10
	}
}

int hookedCreateWindow(const char* title, void(__cdecl* exit_function)(int))
{
	SetProcessDPIAware();

	// trick Optimus into switching to the NVIDIA GPU
	if (cuInit != NULL) cuInit(0);
	
	if (nDisplay == 2) // fullscreen
	{
		char GameModeString[24];
		sprintf_s(GameModeString, sizeof(GameModeString), "%dx%d:%d@%d", nWidth, nHeight, nBitDepth, nRefreshRate);
		glutGameModeString(GameModeString);
		
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
			printf("[Render Manager] Game mode (exclusive fullscreen) enabled.\n");
			printf(GameModeString);
			printf("\n");
			glutEnterGameMode();
		}
		else {
			printf("[Render Manager] Requested display mode not supported. Using non-exclusive fullscreen instead.\n");
			printf(GameModeString);
			printf("\n");
			switch (game_version)
			{
			case 600:
				*fullScreenFlag_600 = 1;
				break;
			default:
				*fullScreenFlag_710 = 1;
			}
			__glutCreateWindowWithExit(title, exitGameClean);
		}
	}
	else // windowed or borderless
	{
		switch (game_version)
		{
		case 600:
			*fullScreenFlag_600 = 0;
			break;
		default:
			*fullScreenFlag_710 = 0;
		}

		RECT xy;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &xy, 0);
		int screenWidth = xy.right - xy.left;
		int screenHeight = xy.bottom - xy.top;
		if (nWidth > screenWidth)
		{
			screenWidth = GetSystemMetrics(SM_CXSCREEN);
			xy.left = 0;
		}

		if (nHeight > screenHeight)
		{
			screenHeight = GetSystemMetrics(SM_CYSCREEN);
			xy.top = 0;
		}
		int wndX = xy.left + (screenWidth - nWidth) / 2;
		int wndY = xy.top + (screenHeight - nHeight) / 2;

		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

		glutInitWindowSize(nWidth, nHeight);
		glutInitWindowPosition(wndX, wndY);
		__glutCreateWindowWithExit(title, exitGameClean);

		if (nDisplay == 1 || nDisplay == 3) // borderless mode
		{
			// support borderless mode even with original copy of glut
			HDC hDev = wglGetCurrentDC(); // get handle to current opengl device context
			HWND hWnd = WindowFromDC(hDev); // convert it to a window handle

			LONG_PTR wndStyle = WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;  // set no border (visible to ensure something is shown immediately)

			if (nDisplay == 1)
			{
				wndStyle |= WS_POPUP;
				printf("[Render Manager] Popup enabled.\n");
			}

			SetWindowLongPtr(hWnd, GWL_STYLE, wndStyle);
			SetWindowPos(hWnd, HWND_TOP, wndX, wndY, nWidth, nHeight, SWP_FRAMECHANGED); // adjust position to apply new style

			printf("[Render Manager] Borderless mode.\n");
		}
		else
		{
			printf("[Render Manager] Windowed mode.\n");
		}
	}

	return true;
}

__int64 hookedParseParameters(int a1, __int64* a2)
{
	// Force -wqhd if Custom Internal Resolution is enabled
	if (nIntRes)
		switch (game_version)
		{
		case 600:
			*resolutionType_600 = 15;
			break;
		//default:
		//	*resolutionType_710 = 15;
		}
	// Return to the original function
	switch (game_version)
	{
	case 600:
		return divaParseParameters_600(a1, a2);
		break;
	//default:
	//	return divaParseParameters_710(a1, a2);
	}
}

time_point<high_resolution_clock> sleepUntil = high_resolution_clock::now();
time_point<high_resolution_clock> nextUpdate = high_resolution_clock::now();
nanoseconds expectedFrameDuration;
nanoseconds sleepWindow(2500000);

__int64 __fastcall limiterFuncNormal(__int64 a1)
{
	auto timeNow = high_resolution_clock::now();

	if (timeNow < nextUpdate)
		return 0;

	timeNow = high_resolution_clock::now();

	// increment nextUpdate by fixed interval to unsure timing accuracy between frames
	// (ignore time spent outside the update)
	nextUpdate += expectedFrameDuration;

	// if dropping frames, run as fast as possible
	// and also change nextUpdate's timing to match current time
	// (so frame n+2 will be at least 1/nFpsLimit seconds from frame n+1)
	if (nextUpdate < timeNow)
		nextUpdate = timeNow;

	switch (game_version)
	{
	case 600:
		return divaEngineUpdate_600(a1);
	default:
		return divaEngineUpdate_710(a1);
	}
}

__int64 __fastcall limiterFuncLight(__int64 a1)
{
	auto timeNow = high_resolution_clock::now();

	// sleep until the approximate correct time to save CPU usage,
	// then change to instantly returning when timing is more critical
	if (timeNow < sleepUntil)
	{
		std::this_thread::sleep_until(sleepUntil);

		if (nVerboseLimiter)
		{
			timeNow = high_resolution_clock::now();
			nanoseconds timeDifference = (timeNow - sleepUntil);
			if (timeDifference > sleepWindow) // no need for abs because it doesn't matter if this is early
			{
				printf("FPS LIMITER SLEEP TIME OUTSIDE ALLOWED WINDOW\n");
				printf(" Target sleep until time: %lld, Actual time: %lld\n", sleepUntil.time_since_epoch().count(), timeNow.time_since_epoch().count());
				printf(" (Difference: %lld, Window: %lld)\n", timeDifference.count(), sleepWindow.count());
			}
		}
	}

	if (timeNow < nextUpdate)
		return 0;

	timeNow = high_resolution_clock::now();

	// increment nextUpdate by fixed interval to unsure timing accuracy between frames
	// (ignore time spent outside the update)
	nextUpdate += expectedFrameDuration;

	// if dropping frames, run as fast as possible
	// and also change nextUpdate's timing to match current time
	// (so frame n+2 will be at least 1/nFpsLimit seconds from frame n+1)
	if (nextUpdate < timeNow)
		nextUpdate = timeNow;

	sleepUntil = nextUpdate - sleepWindow;

	switch (game_version)
	{
	case 600:
		return divaEngineUpdate_600(a1);
	default:
		return divaEngineUpdate_710(a1);
	}
}

#define WRITE_MEMORY_INT(address, data) \
{ \
DWORD oldProtect, bck; \
VirtualProtect((BYTE*)(address), 4, PAGE_EXECUTE_READWRITE, &oldProtect); \
*((int*)(address)) = (data); \
VirtualProtect((BYTE*)(address), 4, oldProtect, &bck); \
}

void __fastcall hookedInitRender(int ssaa, int hd_res, int ss_alpha_mask, __int8 npr) {
	if (nIntRes)
	{
		const __int64 widthAddress = 0x0000000140EDA8B0 + 0x0C;
		const __int64 heightAddress = 0x0000000140EDA8B0 + 0x10;
		const __int64 intWidthAddress = 0x0000000140EDA8D8 + 0x0C;
		const __int64 intHeightAddress = 0x0000000140EDA8D8 + 0x10;

		int originalWidth = *(int*)widthAddress;
		int originalHeight = *(int*)heightAddress;
		int originalIntWidth = *(int*)intWidthAddress;
		int originalIntHeight = *(int*)intHeightAddress;
		WRITE_MEMORY_INT(widthAddress, nIntResWidth);
		WRITE_MEMORY_INT(heightAddress, nIntResHeight);
		WRITE_MEMORY_INT(intWidthAddress, nIntResWidth);
		WRITE_MEMORY_INT(intHeightAddress, nIntResHeight);
		divaInitRender_710(ssaa, hd_res, ss_alpha_mask, npr);
		WRITE_MEMORY_INT(widthAddress, originalWidth);
		WRITE_MEMORY_INT(heightAddress, originalHeight);
		WRITE_MEMORY_INT(intWidthAddress, originalIntWidth);
		WRITE_MEMORY_INT(intHeightAddress, originalIntHeight);
	}
	else
		divaInitRender_710(ssaa, hd_res, ss_alpha_mask, npr);
}

__int64 __fastcall hookedEngineUpdate(__int64 a1)
{
	if (nFpsLimit < 1)
		switch (game_version)
		{
		case 600:
			return divaEngineUpdate_600(a1);
		default:
			return divaEngineUpdate_710(a1);
		}
	else if (nUseLightLimiter)
		return limiterFuncLight(a1);
	else
		return limiterFuncNormal(a1);
}

extern "C" __declspec(dllexport) int getFramerateLimit(void)
{
	return nFpsLimit;
}

extern "C" __declspec(dllexport) void setFramerateLimit(int framerate)
{
	nFpsLimit = framerate;
	if (nFpsLimit < 1)
		expectedFrameDuration = nanoseconds(0);
	else
		expectedFrameDuration = nanoseconds(1000000000 / nFpsLimit);
	return;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		if (*(char*)0x140A570F0 == '6') game_version = 600;

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		switch (game_version)
		{
		case 600:
			DetourAttach(&(PVOID&)divaCreateWindow_600, hookedCreateWindow);
			break;
		default:
			DetourAttach(&(PVOID&)divaCreateWindow_710, hookedCreateWindow);
		}
		DetourTransactionCommit();

		switch (game_version)
		{
		case 600:
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)divaParseParameters_600, hookedParseParameters);
			DetourTransactionCommit();
			break;
		//default:
		//	DetourAttach(&(PVOID&)divaParseParameters_710, hookedParseParameters);
		}


		// set sleep time resolution to 2ms or device minimum (whichever's lower)
		TIMECAPS tc;

		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) == MMSYSERR_NOERROR)
		{
			UINT wTimerRes = min(max(tc.wPeriodMin, 2), tc.wPeriodMax);
			timeBeginPeriod(wTimerRes);
		}

		setFramerateLimit(nFpsLimit);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		switch (game_version)
		{
		case 600:
			DetourAttach(&(PVOID&)divaEngineUpdate_600, hookedEngineUpdate);
			break;
		default:
			DetourAttach(&(PVOID&)divaEngineUpdate_710, hookedEngineUpdate);
		}
		DetourTransactionCommit();

		// if window is set to screen res, process that now so it can be used for auto internal res
		if (nWidth == -1 || nHeight == -1)
		{
			nWidth = GetSystemMetrics(SM_CXSCREEN);
			nHeight = GetSystemMetrics(SM_CYSCREEN);
		}

		if (nIntRes)
		{
			// if -1 use window resolution
			if (nIntResWidth == -1 || nIntResHeight == -1)
			{
				nIntResWidth = nWidth;
				nIntResHeight = nHeight;
			}

			printf("[Render] Custom internal resolution enabled\n");
			printf("[Render] X: %d Y: %d\n", nIntResWidth, nIntResHeight);
			switch (game_version)
			{
			case 600:
				WRITE_MEMORY_INT(0x0000000140980954, nIntResWidth);
				WRITE_MEMORY_INT(0x0000000140980958, nIntResWidth);
				break;
			default:
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());
				DetourAttach(&(PVOID&)divaInitRender_710, hookedInitRender);
				DetourTransactionCommit();
			}
		}
	}
	return TRUE;
}


using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Render";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Applies window mode/size and FPS limiting.";
}
