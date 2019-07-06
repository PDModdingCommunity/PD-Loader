#include "framework.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")
#include <GL\freeglut.h>
#include <GL\GL.h>
#include <windows.h>
#include <iostream>

int hookedCreateWindow(const char* title, void(__cdecl* exit_function)(int))
{
	if (nDisplay == 1)
	{
		*fullScreenFlag = 0;
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_BORDERLESS);
		glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
		glutInitWindowPosition(0, 0);
		glutCreateWindow(title);
	printf("[Render Manager] Borderless mode.\n");
	}
	else if (nDisplay == 2)
	{
		// Crashes, no idea why yet
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
			char GameModeString[24];
			sprintf_s(GameModeString, sizeof(GameModeString),"%dx%d:%d@%d", nWidth, nHeight, nBitDepth ,nRefreshRate);
			printf("[Render Manager] Game Mode supported and enabled in replacement of Fullscreen.\n");
			printf(GameModeString);
			printf("\n");
			glutGameModeString(GameModeString);
			glutEnterGameMode();
		}
		else {
		*fullScreenFlag = 1;
		glutCreateWindow(title);
		printf("[Render Manager] Fullscreen enabled.\n");
		}
	}
	else
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

