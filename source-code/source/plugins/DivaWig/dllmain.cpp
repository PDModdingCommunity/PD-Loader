#include "framework.h"
#include "PluginConfigApi.h"
#include <detours.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <thread>

static const enum module_part
{
	HEAD_ACCESSORY,
	HAIR,
	UNKNOWN_1,
	UNKNOWN_2,
	FACE_ACCESSORY,
	UNKNOWN_3,
	FACE_TEXTURES,
	UNKNOWN_4,
	CHEST_ACCESSORY,
	UNKNOWN_5,
	BODY,
	UNKNOWN_6,
	UNKNOWN_7,
	UNKNOWN_8,
	HANDS,
	UNKNOWN_9,
	BACK_ACCESSORY,
	UNKNOWN_10,
	UNKNOWN_11,
	UNKNOWN_12,
	UNKNOWN_13,
	UNKNOWN_14,
	UNKNOWN_15,
	UNKNOWN_16,
	HEAD,
	ALL = -1,
	NONE = -2
};

using namespace std;

void(__cdecl* originalWig)(void* cls, uint64_t unk, uint64_t unk2) = (void(__cdecl*)(void* cls, uint64_t unk, uint64_t unk2))0x14052AD90;
void(__cdecl* originalSec)(unsigned int* a1, unsigned int module_part) = (void(__cdecl*)(unsigned int* a1, unsigned int module_part))0x14052ADF0;
void(__cdecl* originalThi)(unsigned int* a1, unsigned int module_part) = (void(__cdecl*)(unsigned int* a1, unsigned int module_part))0x140521FA0;
void(__cdecl* pvTimerUpdate)(__int64 a1) = (void(__cdecl*)(__int64 a1))0x1405BDF90;
void(__cdecl* unloadFunc)(uint64_t* classp, unsigned int unk, __int64 performer) = (void(__cdecl*)(uint64_t * classp, unsigned int unk, __int64 performer))0x14052F720;
void(__cdecl* loadFunc)(uint64_t* classp, unsigned int unk, __int64 performer) = (void(__cdecl*)(uint64_t * classp, unsigned int unk, __int64 performer))0x14052FC00;

const string Strings[25] = { "Head accessory", "Hair", "Unknown 1", "Unknown 2", "Face accessory", "Unknown 3", "Face textures", "Unknown 4", "Chest accessory", "Unknown 5", "Body", "Unknown 6", "Unknown 7", "Unknown 8", "Hands", "Unknown 9", "Back accessory", "Unknown 10", "Unknown 11", "Unknown 12", "Unknown 13", "Unknown 14", "Unknown 15", "Unknown 16", "Head" };
char part_to_update = -1;

/*uint64_t *last_unload_a1 = 0, *last_reload_a1 = 0;
unsigned int last_unload_a2 = 0, last_reload_a2 = 0;
__int64 last_unload_a3 = 0, last_reload_a3 = 0;*/

bool dobeep = true, allowReloading = true, debug = false;

void loadConfig()
{
	dobeep = GetPrivateProfileIntW(L"general", L"beep", 1, CONFIG_FILE) > 0 ? true : false;
	allowReloading = GetPrivateProfileIntW(L"general", L"reloading", 0, CONFIG_FILE) > 0 ? true : false;
	debug = GetPrivateProfileIntW(L"general", L"debug", 0, CONFIG_FILE) > 0 ? true : false;

	return;
}

void setPartToUpdate(char part, string message, unsigned short beepfreq)
{
	part_to_update = part;
	cout << "[DivaWig] " << message << endl;
	if (dobeep) Beep(beepfreq, 300);

	this_thread::sleep_for(chrono::seconds(1));
}

void unloadAll()
{
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387286240);
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387319280);
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387352320);
	cout << "[DivaWig] All unloaded!" << endl;
	if (dobeep) Beep(500, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void unload(__int64 performer_address)
{
	unloadFunc((uint64_t*)0x1411B7060, 0, performer_address);
	cout << "[DivaWig] Performer " << performer_address/33040 + 1 << " unloaded!" << endl;
	if (dobeep) Beep(500, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void loadAll()
{
	loadFunc((uint64_t*)0x1411B7060, 0, 5387286240);
	loadFunc((uint64_t*)0x1411B7060, 0, 5387319280);
	loadFunc((uint64_t*)0x1411B7060, 0, 5387352320);
	cout << "[DivaWig] All loaded!" << endl;
	if (dobeep) Beep(600, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void load(__int64 performer_address)
{
	loadFunc((uint64_t*)0x1411B7060, 0, performer_address);
	cout << "[DivaWig] Performer " << performer_address / 33040 + 1 << " loaded!" << endl;
	if (dobeep) Beep(600, 100);
	this_thread::sleep_for(chrono::seconds(1));
}

/*void hookedUnload(uint64_t* a1, unsigned int a2, __int64 a3)
{
	cout << "[DivaWig] hookedUnload a1: " << a1 << "; a2: " << a2 << "; a3: " << a3 << endl;
	last_unload_a1 = a1;
	last_unload_a2 = a2;
	last_unload_a3 = a3;
	unloadFunc(a1, a2, a3);
}

void hookedLoad(uint64_t* a1, unsigned int a2, __int64 a3)
{
	cout << "[DivaWig] hookedReload a1: " << a1 << "; a2: " << a2 << "; a3: " << a3 << endl;
	last_reload_a1 = a1;
	last_reload_a2 = a2;
	last_reload_a3 = a3;
	loadFunc(a1, a2, a3);
}*/

void inputLoop(__int64 a1)
{
	//if (GetKeyState(VK_CONTROL) < 0)
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
	if (allowReloading)
	{
		if (GetKeyState('U') < 0)
		{
			if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) unloadAll();
			if (GetKeyState('1') < 0) unload(5387286240);
			if (GetKeyState('2') < 0) unload(5387319280);
			if (GetKeyState('3') < 0) unload(5387352320);
		}
		if (GetKeyState('L') < 0)
		{
			if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) loadAll();
			if (GetKeyState('1') < 0) load(5387286240);
			if (GetKeyState('2') < 0) load(5387319280);
			if (GetKeyState('3') < 0) load(5387352320);
		}
	}

		pvTimerUpdate(a1);
}

__int64 hookedWig(__int64 classp, int module_part, int part_id)
{
	if (debug) cout << "[DivaWig] Performer: " << (classp-5387286232)/33040+1 << "; Module part: " << module_part << " (" << Strings[module_part] << ")" << "; Default part: " << part_id << "." << endl;
	
	if (part_to_update == ALL || part_to_update == module_part)
		*(int*)(classp + 4i64 * module_part + 8) = part_id;

	return module_part;
}

void hookedSec(unsigned int* a1, unsigned int module_part)
{
	if (part_to_update == ALL || part_to_update == module_part)
		originalSec(a1, module_part);
}

void hookedThi(unsigned int* a1, unsigned int module_part)
{
	if (part_to_update == ALL || part_to_update == module_part)
		originalSec(a1, module_part);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		cout << "[DivaWig] Hooking functions..." << endl;
		DetourAttach(&(PVOID&)originalWig, (PVOID)hookedWig);
		DetourAttach(&(PVOID&)originalSec, (PVOID)hookedSec);
		DetourAttach(&(PVOID&)originalThi, (PVOID)hookedThi);
		DetourAttach(&(PVOID&)pvTimerUpdate, (PVOID)inputLoop);
		cout << "[DivaWig] Functions hooked." << endl;
		DetourTransactionCommit();

		/*if (allowReloading)
		{
			DisableThreadLibraryCalls(hModule);
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)unloadFunc, (PVOID)hookedUnload);
			DetourTransactionCommit();

			DisableThreadLibraryCalls(hModule);
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)loadFunc, (PVOID)hookedLoad);
			DetourTransactionCommit();
		}*/

		//thread* inputThread = new thread(inputLoop);
	}

	return TRUE;
}

PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"beep", L"general", CONFIG_FILE, L"Beep", L"Beep when selecting the part that should be updated.", true } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"reloading", L"general", CONFIG_FILE, L"Unloading and loading (experimental)", L"Allow unloading (CTRL+U or U+number) and loading (CTRL+L or L+number).", false } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"debug", L"general", CONFIG_FILE, L"Debug", L"Print extra information.", false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DivaWig";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"DivaWig Plugin by nas\n\nDivaWig lets you mix some module parts by pressing CTRL+0~9.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}
