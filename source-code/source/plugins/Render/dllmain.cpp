#include "framework.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")
#include <GL\freeglut.h>
#include <GL\GL.h>
#include <wingdi.h>
#include <windows.h>
#include <iostream>

int hookedCreateWindow(const char* title, void(__cdecl* exit_function)(int))
{
	// trick Optimus into switching to the NVIDIA GPU
	HMODULE nvcudaModule = LoadLibraryW(L"nvcuda.dll");
	// cuInit actually returns a CUresult, but we don't really care about it
	void(WINAPI * cuInit)(unsigned int flags) = (void(WINAPI*)(unsigned int flags))GetProcAddress(nvcudaModule, "cuInit");
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
			glutCreateWindow(title);
		}
	}
	else // windowed or borderless
	{
		*fullScreenFlag = 0;

		// looks like GLUT_SCREEN_WIDTH and GLUT_SCREEN_HEIGHT can have issues after SetProcessDPIAware()
		// not sure if calling SetProcessDPIAware later will fix this, but I'll try it
		// if not, using GetSystemMetrics should work
		int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
		int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
		//int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		//int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// window position for centered window
		int wndX = (screenWidth - nWidth) / 2;
		int wndY = (screenHeight - nHeight) / 2;


		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

		glutInitWindowSize(nWidth, nHeight);
		glutInitWindowPosition(wndX, wndY);
		glutCreateWindow(title);

		if (nDisplay == 1) // borderless mode
		{
			// support borderless mode even with original copy of glut
			HDC hDev = wglGetCurrentDC(); // get handle to current opengl device context
			HWND hWnd = WindowFromDC(hDev); // convert it to a window handle

			SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP + WS_VISIBLE); // set popup style (no border)
			SetWindowPos(hWnd, HWND_TOP, wndX, wndY, nWidth, nHeight, 0); // adjust position to apply new style

			printf("[Render Manager] Borderless mode.\n");
		}
		else
		{
			printf("[Render Manager] Windowed mode.\n");
		}
	}

	SetProcessDPIAware();
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
	}
	return TRUE;
}

