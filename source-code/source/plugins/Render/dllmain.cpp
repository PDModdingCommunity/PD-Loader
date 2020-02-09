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

void exitGameClean(int)
{
	*(bool*)0x0000000140EDA6B0 = true;
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
			*fullScreenFlag = 1;
			__glutCreateWindowWithExit(title, exitGameClean);
		}
	}
	else // windowed or borderless
	{
		*fullScreenFlag = 0;

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
		*resolutionType = 15;
	// Return to the original function
	return divaParseParameters(a1, a2);
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

	const auto result = divaEngineUpdate(a1);

	timeNow = high_resolution_clock::now();

	// increment nextUpdate by fixed interval to unsure timing accuracy between frames
	// (ignore time spent outside the update)
	nextUpdate += expectedFrameDuration;

	// if dropping frames, run as fast as possible
	// and also change nextUpdate's timing to match current time
	// (so frame n+2 will be at least 1/nFpsLimit seconds from frame n+1)
	if (nextUpdate < timeNow)
		nextUpdate = timeNow;

	return result;
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

	const auto result = divaEngineUpdate(a1);

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

	return result;
}

__int64 __fastcall hookedEngineUpdate(__int64 a1)
{
	if (nFpsLimit < 1)
		return divaEngineUpdate(a1);
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
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaCreateWindow, hookedCreateWindow);
		DetourTransactionCommit();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaParseParameters, hookedParseParameters);
		DetourTransactionCommit();


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
		DetourAttach(&(PVOID&)divaEngineUpdate, hookedEngineUpdate);
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
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001409B8B68, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((int*)0x00000001409B8B68) = nIntResWidth;
				VirtualProtect((BYTE*)0x00000001409B8B68, 6, oldProtect, &bck);
			}
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001409B8B6C, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((int*)0x00000001409B8B6C) = nIntResHeight;
				VirtualProtect((BYTE*)0x00000001409B8B6C, 6, oldProtect, &bck);
			}

			//*((int*)0x00000001409B8B6C) = maxHeight;
			//*((int*)0x00000001409B8B14) = maxWidth;
			//*((int*)0x00000001409B8B18) = maxHeight;

			//*((int*)0x00000001409B8B1C) = maxWidth; // No parameters width?
			//*((int*)0x00000001409B8B20) = maxHeight; // No parameters height?
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