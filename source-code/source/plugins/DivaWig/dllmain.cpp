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
#define ALL -1
#define NONE -2

using namespace std;

void(__cdecl* originalWig)(void* cls, uint64_t unk, uint64_t unk2) = (void(__cdecl*)(void* cls, uint64_t unk, uint64_t unk2))0x14052AD90;
void(__cdecl* pvTimerUpdate)(__int64 a1) = (void(__cdecl*)(__int64 a1))0x1405BDF90;

const string Strings[25] = { "Head accessory", "Hair", "Unknown 1", "Unknown 2", "Face accessory", "Unknown 3", "Face textures", "Unknown 4", "Chest accessory", "Unknown 5", "Body", "Unknown 6", "Unknown 7", "Unknown 8", "Hands", "Unknown 9", "Back accessory", "Unknown 10", "Unknown 11", "Unknown 12", "Unknown 13", "Unknown 14", "Unknown 15", "Unknown 16", "Head" };
char part_to_update = -1;

bool dobeep = true;

void loadConfig()
{
	dobeep = GetPrivateProfileIntW(L"general", L"beep", 1, CONFIG_FILE) > 0 ? true : false;

	return;
}

void setPartToUpdate(char part, string message, unsigned short beepfreq)
{
	part_to_update = part;
	cout << "[DivaWig] " << message << endl;
	if (dobeep) Beep(beepfreq, 300);

	//this_thread::sleep_for(chrono::seconds(1));
	Sleep(100);
}

void inputLoop(__int64 a1)
{
	if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)
	{
		if (GetKeyState('0') < 0) setPartToUpdate(ALL, "Update all module parts!", 330);
		if (GetKeyState('1') < 0) setPartToUpdate(HAIR, "Only update the hair!", 440);
		if (GetKeyState('2') < 0) setPartToUpdate(HEAD_ACCESSORY, "Only update the head accessory!", 550);
		if (GetKeyState('3') < 0) setPartToUpdate(FACE_ACCESSORY, "Only update the face accessory!", 660);
		if (GetKeyState('4') < 0) setPartToUpdate(FACE_TEXTURES, "Only update the face textures!", 770);
		if (GetKeyState('5') < 0) setPartToUpdate(HEAD, "Only update the head!", 880);
		if (GetKeyState('6') < 0) setPartToUpdate(CHEST_ACCESSORY, "Only update the chest accessory!", 990);
		if (GetKeyState('7') < 0) setPartToUpdate(BODY, "Only update the body!", 1100);
		if (GetKeyState('8') < 0) setPartToUpdate(BACK_ACCESSORY, "Only update the back accessory!", 1210);
		if (GetKeyState('9') < 0) setPartToUpdate(NONE, "Update nothing!", 220);
	}

		pvTimerUpdate(a1);
}

__int64 hookedWig(__int64 classp, int module_part, int part_id)
{
	cout << "[DivaWig] Performer: " << (classp-5387286232)/33040+1 << "; Module part: " << module_part << " (" << Strings[module_part] << ")" << "; Default part: " << part_id << "." << endl;
	
	if (part_to_update == ALL || part_to_update == module_part)
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

PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"beep", L"general", CONFIG_FILE, L"Beep", L"Beep when selecting the part that should be updated.", true } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DivaWig";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"DivaWig Plugin by nas\n\nDivaWig lets you mix some module parts.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}
