#include "framework.h"
#include "PluginConfigApi.h"
#include <detours.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <thread>

#define HEAD_ACCESSORY 0
#define HAIR 1
#define UNKNOWN_1 2
#define UNKNOWN_2 3
#define FACE_ACCESSORY 4
#define UNKNOWN_3 5
#define FACE_TEXTURES 6
#define UNKNOWN_4 7
#define CHEST_ACCESSORY 8
#define UNKNOWN_5 9
#define BODY 10
#define UNKNOWN_6 11
#define UNKNOWN_7 12
#define UNKNOWN_8 13
#define HANDS 14,
#define UNKNOWN_9 15
#define BACK_ACCESSORY 16
#define UNKNOWN_10 17
#define UNKNOWN_11 18
#define UNKNOWN_12 19
#define UNKNOWN_13 20
#define UNKNOWN_14 21
#define UNKNOWN_15 22
#define UNKNOWN_16 23
#define HEAD 24

using namespace std;

void(__cdecl* originalWig)(void* cls, uint64_t unk, uint64_t unk2) = (void(__cdecl*)(void* cls, uint64_t unk, uint64_t unk2))0x14052AD90;
void(__cdecl* pvTimerUpdate)(__int64 a1) = (void(__cdecl*)(__int64 a1))0x1405BDF90;

const string Strings[25] = { "Head accessory", "Hair", "Unknown 1", "Unknown 2", "Face accessory", "Unknown 3", "Face textures", "Unknown 4", "Chest accessory", "Unknown 5", "Body", "Unknown 6", "Unknown 7", "Unknown 8", "Hands", "Unknown 9", "Back accessory", "Unknown 10", "Unknown 11", "Unknown 12", "Unknown 13", "Unknown 14", "Unknown 15", "Unknown 16", "Head" };
bool only_hair = false, only_head_accessory = false, only_face_accessory = false, only_face_textures = false, head = false, nothing = false;

void loadConfig()
{
	// some_option = GetPrivateProfileIntW(L"general", L"some_option", 0, CONFIG_FILE) > 0 ? true : false;

	return;
}

void inputLoop(__int64 a1)
{
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('0') < 0)
		{
			only_hair = false;
			only_head_accessory = false;
			only_face_accessory = false;
			only_face_textures = false;
			head = false;
			nothing = false;
			Beep(330, 300);
			cout << "[DivaWig] Update all module parts!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('1') < 0)
		{
			only_hair = true;
			only_head_accessory = false;
			only_face_accessory = false;
			only_face_textures = false;
			head = false;
			nothing = false;
			Beep(440, 300);
			cout << "[DivaWig] Only update the hair!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('2') < 0)
		{
			only_hair = false;
			only_head_accessory = true;
			only_face_accessory = false;
			only_face_textures = false;
			head = false;
			nothing = false;
			Beep(550, 300);
			cout << "[DivaWig] Only update the head accessory!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('3') < 0)
		{
			only_hair = false;
			only_head_accessory = false;
			only_face_accessory = true;
			only_face_textures = false;
			head = false;
			nothing = false;
			Beep(660, 300);
			cout << "[DivaWig] Only update the face accessory!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('4') < 0)
		{
			only_hair = false;
			only_head_accessory = false;
			only_face_accessory = false;
			only_face_textures = true;
			head = false;
			nothing = false;
			Beep(770, 300);
			cout << "[DivaWig] Only update the face textures!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('5') < 0)
		{
			only_hair = false;
			only_head_accessory = false;
			only_face_accessory = false;
			only_face_textures = false;
			head = true;
			nothing = false;
			Beep(880, 300);
			cout << "[DivaWig] Only update the head!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}
		if ((GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) && GetKeyState('9') < 0)
		{
			only_hair = false;
			only_head_accessory = false;
			only_face_accessory = false;
			only_face_textures = false;
			head = false;
			nothing = true;
			Beep(220, 300);
			cout << "[DivaWig] Update nothing!" << endl;
			//this_thread::sleep_for(chrono::seconds(1));
		}

		pvTimerUpdate(a1);
}

__int64 hookedWig(__int64 classp, int module_part, int part_id)
{
	cout << "[DivaWig] Performer: " << (classp-5387286232)/33040+1 << "; Module part: " << module_part << " (" << Strings[module_part] << ")" << "; Default part: " << part_id << "." << endl;

	if (!(only_hair || only_head_accessory || only_face_accessory || only_face_textures || head || nothing) || (only_hair && module_part == HAIR) || (only_head_accessory && module_part == HEAD_ACCESSORY) || (only_face_accessory && module_part == FACE_ACCESSORY) || (only_face_textures && module_part == FACE_TEXTURES) || (head && module_part == HEAD))
		*(int*)(classp + 4i64 * module_part + 8) = part_id;

	return module_part;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		cout << "[DivaWig] Hooking function..." << endl;
		DetourAttach(&(PVOID&)originalWig, (PVOID)hookedWig);
		cout << "[DivaWig] Function hooked." << endl;
		DetourTransactionCommit();

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)pvTimerUpdate, (PVOID)inputLoop);
		DetourTransactionCommit();

		//thread* inputThread = new thread(inputLoop);
	}

	return TRUE;
}

/*PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"some_option", L"general", CONFIG_FILE, L"TODO", L"Add options.", false } },
};*/

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DivaWig";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"DivaWig Plugin by nas\n\nDivaWig lets you mix some module parts.";
}

/*extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}*/