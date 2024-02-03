#include <iostream>
#include <vector>
#include <Windows.h>
#include <Dbt.h>
#include <stdio.h>
#include "Constants.h"
#include "framework.h"
#include "pluginconfig.h"
#include "Input/Mouse/Mouse.h"
#include "Input/Xinput/Xinput.h"
#include "Input/Keyboard/Keyboard.h"
#include "Input/DirectInput/DirectInput.h"
#include "Input/DirectInput/Ds4/DualShock4.h"
#include "Input/DirectInput/GenericUsb/GenericUsbInput.h"
#include "Input/Divaller/Divaller.h"
#include "Components/ComponentsManager.h"
#include <tchar.h>
#include <GL/freeglut.h>

#include <detours.h>
#include <fstream>
#pragma comment(lib, "detours.lib")

void(__cdecl* divaEngineUpdate)() = (void(__cdecl*)())0x14018CC40;
void(__cdecl* divaEngineStageDispShadow)() = (void(__cdecl*)())ENGINE_STAGE_DISP_SHADOW_ADDRESS;

LRESULT CALLBACK MessageWindowProcessCallback(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI WindowMessageDispatcher(LPVOID);
VOID RegisterMessageWindowClass();

struct
{
	DWORD ID = NULL;
	HANDLE Handle = NULL;
} MessageThread;

const wchar_t* MessageWindowClassName = TEXT("MessageWindowClass");
const wchar_t* MessageWindowName = TEXT("MessageWindowTitle");

namespace TLAC
{
	Components::ComponentsManager ComponentsManager;
	bool DeviceConnected = true;
	bool FirstUpdateTick = true;
	bool HasWindowFocus, HadWindowFocus;

	void InitializeTick()
	{
		RegisterMessageWindowClass();
		if ((MessageThread.Handle = CreateThread(0, 0, WindowMessageDispatcher, 0, 0, 0)) == NULL)
			printf("[TLAC] InitializeTick(): CreateThread() Error: %d\n", GetLastError());

		framework::DivaWindowHandle = WindowFromDC(wglGetCurrentDC());

		HRESULT diInitResult = Input::InitializeDirectInput(framework::Module);
		if (FAILED(diInitResult))
			printf("[TLAC] InitializeTick(): Failed to initialize DirectInput. Error: 0x%08X\n", diInitResult);

		ComponentsManager.Initialize();
	}

	void UpdateTick()
	{
		if (FirstUpdateTick)
		{
			FirstUpdateTick = false;
			InitializeTick();
		}

		if (DeviceConnected)
		{
			DeviceConnected = false;

			if (!Input::DualShock4::InstanceInitialized())
			{
				if (Input::DualShock4::TryInitializeInstance())
					printf("[TLAC] UpdateTick(): DualShock4 connected and initialized\n");
			}

			if (!Input::GenericUsbInput::InstanceInitialized())
			{
				if (Input::GenericUsbInput::TryInitializeInstance())
					printf("[TLAC] UpdateTick(): GenericUsbInput connected and initialized\n");
			}

			if (!Input::Divaller::InstanceInitialized())
			{
				if (Input::Divaller::TryInitializeInstance())
					printf("[TLAC] UpdateTick(): Divaller connected and initialized\n");
			}
		}

		ComponentsManager.Update();

		HadWindowFocus = HasWindowFocus;
		HasWindowFocus = framework::DivaWindowHandle == NULL || GetForegroundWindow() == framework::DivaWindowHandle;

		if ((HasWindowFocus) && (!framework::inputDisable))
		{
			Input::Keyboard::GetInstance()->PollInput();
			Input::Mouse::GetInstance()->PollInput();
			Input::Xinput::GetInstance()->PollInput();

			if (Input::DualShock4::GetInstance() != nullptr)
			{
				if (!Input::DualShock4::GetInstance()->PollInput())
				{
					Input::DualShock4::DeleteInstance();
					printf("[TLAC] UpdateTick(): DualShock4 connection lost\n");
				}
			}

			if (Input::GenericUsbInput::GetInstance() != nullptr)
			{
				if (!Input::GenericUsbInput::GetInstance()->PollInput())
				{
					Input::GenericUsbInput::DeleteInstance();
					printf("[TLAC] UpdateTick(): GenericUsbInput connection lost\n");
				}
			}

			if (Input::Divaller::GetInstance() != nullptr)
			{
				if (!Input::Divaller::GetInstance()->PollInput())
				{
					Input::Divaller::DeleteInstance();
					printf("[TLAC] UpdateTick(): Divaller connection lost\n");
				}
			}

			ComponentsManager.UpdateInput();
		}

		ComponentsManager.UpdatePostInput();

		if ((framework::inputDisable))
		{
			Input::Keyboard::GetInstance()->PollInput();
			Input::Mouse::GetInstance()->PollInput();
			Input::Xinput::GetInstance()->PollInput();

			if (Input::DualShock4::GetInstance() != nullptr)
			{
				if (!Input::DualShock4::GetInstance()->PollInput())
				{
					Input::DualShock4::DeleteInstance();
					printf("[TLAC] UpdateTick(): DualShock4 connection lost\n");
				}
			}

			if (Input::GenericUsbInput::GetInstance() != nullptr)
			{
				if (!Input::GenericUsbInput::GetInstance()->PollInput())
				{
					Input::GenericUsbInput::DeleteInstance();
					printf("[TLAC] UpdateTick(): GenericUsbInput connection lost\n");
				}
			}

			if (Input::Divaller::GetInstance() != nullptr)
			{
				if (!Input::Divaller::GetInstance()->PollInput())
				{
					Input::Divaller::DeleteInstance();
					printf("[TLAC] UpdateTick(): Divaller connection lost\n");
				}
			}
		}

		if (HasWindowFocus && !HadWindowFocus)
			ComponentsManager.OnFocusGain();

		if (!HasWindowFocus && HadWindowFocus)
			ComponentsManager.OnFocusLost();
	}

	void UpdateDraw2D()
	{
		ComponentsManager.UpdateDraw2D();
	}

	/*void InitializeExtraSettings()
	{
		const LPCTSTR RESOLUTION_CONFIG_FILE_NAME = _T(".\\config.ini");
		auto nTAA = GetPrivateProfileIntW(L"graphics", L"taa", TRUE, RESOLUTION_CONFIG_FILE_NAME);
		auto nMLAA = GetPrivateProfileIntW(L"graphics", L"mlaa", TRUE, RESOLUTION_CONFIG_FILE_NAME);

		
		if (!nTAA)
		{
			{
				// set TAA var (shouldn't be needed but whatever)
				*(byte*)0x00000001411AB67C = 0;
			}
			{
				// make constructor/init not set TAA
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001404AB11D, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00000001404AB11D + 0) = 0x90;
				*((byte*)0x00000001404AB11D + 1) = 0x90;
				*((byte*)0x00000001404AB11D + 2) = 0x90;
				VirtualProtect((BYTE*)0x00000001404AB11D, 3, oldProtect, &bck);
			}
			{
				// not sure, but it's somewhere in TaskPvGame init
				// just make it set TAA to 0 instead of 1 to avoid possible issues
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001401063CE, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00000001401063CE + 0) = 0x00;
				VirtualProtect((BYTE*)0x00000001401063CE, 1, oldProtect, &bck);
			}
			{
				// prevent re-enabling after taking photos
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x000000014048FBA9, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x000000014048FBA9 + 0) = 0x00;
				VirtualProtect((BYTE*)0x000000014048FBA9, 1, oldProtect, &bck);
			}
			printf("[TLAC] TAA disabled\n");
		}
		if (!nMLAA)
		{
			{
				// set MLAA var (shouldn't be needed but whatever)
				*(byte*)0x00000001411AB680 = 0;
			}
			{
				// make constructor/init not set MLAA
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001404AB11A, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00000001404AB11A + 0) = 0x90;
				*((byte*)0x00000001404AB11A + 1) = 0x90;
				*((byte*)0x00000001404AB11A + 2) = 0x90;
				VirtualProtect((BYTE*)0x00000001404AB11A, 3, oldProtect, &bck);
			}

			printf("[TLAC] MLAA disabled\n");
		}
		/*if (nMagFilter > -1)
		{
			{
				// set MAG filter var
				*(byte*)0x00000001411AC518 = nMagFilter;
			}
			{
				// make constructor/init not set  MAG filter (1)
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001404AB13C, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00000001404AB13C + 0) = 0x90;
				*((byte*)0x00000001404AB13C + 1) = 0x90;
				*((byte*)0x00000001404AB13C + 2) = 0x90;
				*((byte*)0x00000001404AB13C + 3) = 0x90;
				*((byte*)0x00000001404AB13C + 4) = 0x90;
				*((byte*)0x00000001404AB13C + 5) = 0x90;
				VirtualProtect((BYTE*)0x00000001404AB13C, 6, oldProtect, &bck);
			}
			{
				// make constructor/init not set  MAG filter (2)
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001404A863F, 10, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((byte*)0x00000001404A863F + 0) = 0x90;
				*((byte*)0x00000001404A863F + 1) = 0x90;
				*((byte*)0x00000001404A863F + 2) = 0x90;
				*((byte*)0x00000001404A863F + 3) = 0x90;
				*((byte*)0x00000001404A863F + 4) = 0x90;
				*((byte*)0x00000001404A863F + 5) = 0x90;
				*((byte*)0x00000001404A863F + 6) = 0x90;
				*((byte*)0x00000001404A863F + 7) = 0x90;
				*((byte*)0x00000001404A863F + 8) = 0x90;
				*((byte*)0x00000001404A863F + 9) = 0x90;
				VirtualProtect((BYTE*)0x00000001404A863F, 10, oldProtect, &bck);
			}

			printf("[TLAC] MAG Filter set to %d\n", nMagFilter);
		}*
	}*/

	void Dispose()
	{
		ComponentsManager.Dispose();

		delete Input::Keyboard::GetInstance();
		delete Input::Mouse::GetInstance();
		delete Input::DualShock4::GetInstance();
		delete Input::GenericUsbInput::GetInstance();

		Input::DisposeDirectInput();

		PostThreadMessage(MessageThread.ID, WM_QUIT, 0, 0);
	}
}

DWORD WINAPI WindowMessageDispatcher(LPVOID lpParam)
{
	HWND windowHandle = CreateWindowW(
		MessageWindowClassName,
		MessageWindowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL,
		TLAC::framework::Module,
		NULL);

	if (!windowHandle)
	{
		printf("[TLAC] WindowMessageDispatcher(): CreateWindowW() Error: %d\n", GetLastError());
		return 1;
	}

	MessageThread.ID = GetCurrentThreadId();

	MSG message;
	DWORD returnValue;

	printf("[TLAC] WindowMessageDispatcher(): Entering message loop...\n");

	while (1)
	{
		returnValue = GetMessage(&message, NULL, 0, 0);
		if (returnValue != -1)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			printf("[TLAC] WindowMessageDispatcher(): GetMessage() Error: %d\n", returnValue);
		}
	}

	DestroyWindow(windowHandle);
	return 0;
}

BOOL RegisterDeviceInterface(HWND hWnd, HDEVNOTIFY* hDeviceNotify)
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {};

	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	*hDeviceNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);

	return *hDeviceNotify != NULL;
}

LRESULT CALLBACK MessageWindowProcessCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HDEVNOTIFY hDevNotify = NULL;

		if (!RegisterDeviceInterface(hWnd, &hDevNotify))
			printf("[TLAC] MessageWindowProcessCallback(): RegisterDeviceInterface() Error: %d\n", GetLastError());

		break;
	}

	case WM_DEVICECHANGE:
	{
		switch (wParam)
		{
		case DBT_DEVICEARRIVAL:
			TLAC::DeviceConnected = true;
			break;

		default:
			break;
		}
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

VOID RegisterMessageWindowClass()
{
	WNDCLASS windowClass = { };

	windowClass.lpfnWndProc = MessageWindowProcessCallback;
	windowClass.hInstance = TLAC::framework::Module;
	windowClass.lpszClassName = MessageWindowClassName;

	RegisterClass(&windowClass);
}

void hookedEngineUpdate()
{
	TLAC::UpdateTick();
	//divaEngineUpdate();
}

void hookedEngineStageDispShadow()
{
	divaEngineStageDispShadow();
	TLAC::UpdateDraw2D();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	HWND consoleHandle = GetConsoleWindow();

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		// Are you a dev and want to debug? Want to always be able to watch the console upon game initialization? Comment this.
		ShowWindow(consoleHandle, SW_HIDE);

		printf("[TLAC] DllMain(): Installing hooks...\n");


		//InstallHook((void*)ENGINE_UPDATE_HOOK_TARGET_ADDRESS, (void*)UpdateTick, 0xE);
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaEngineUpdate, hookedEngineUpdate);
		DetourTransactionCommit();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaEngineStageDispShadow, hookedEngineStageDispShadow);
		DetourTransactionCommit();

		//TLAC::InitializeExtraSettings();

		TLAC::framework::Module = hModule;
		break;

	case DLL_PROCESS_DETACH:
		TLAC::Dispose();
		break;
	}

	return TRUE;
}