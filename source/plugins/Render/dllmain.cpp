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
	if (nDisplay == 1) // borderless fullscreen
	{
		*fullScreenFlag = 0;
		int nWidth = glutGet(GLUT_SCREEN_WIDTH);
		int nHeight = glutGet(GLUT_SCREEN_HEIGHT);
		int nX = 0;
		int nY = 0;

		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_BORDERLESS);
		glutInitWindowSize(nWidth, nHeight);
		glutInitWindowPosition(nX, nY);
		glutCreateWindow(title);

		if (glutGet(GLUT_WINDOW_X) != nX || glutGet(GLUT_WINDOW_Y) != nY) // if not in borderless mode (top left of client area isn't top left of window position)
		{
			// support borderless mode even with original copy of glut
			HDC hDev = wglGetCurrentDC(); // get handle to current opengl device context
			HWND hWnd = WindowFromDC(hDev); // convert it to a window handle
			SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP); // set popup style (no border)
			SetWindowPos(hWnd, HWND_TOP, 0, 0, nWidth, nHeight, 0); // adjust position to apply new style
		}

		printf("[Render Manager] Borderless mode.\n");
	}
	else if (nDisplay == 2) // fullscreen
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
	else // windowed
	{
		*fullScreenFlag = 0;
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(nWidth, nHeight);
		glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - nWidth) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - nHeight) / 2); // Center to the middle of the screen when windowed
		glutCreateWindow(title);
		printf("[Render Manager] Windowed mode.\n");
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

