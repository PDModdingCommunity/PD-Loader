#include "PluginConfigApi.h"
#include "framework.h"
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
int nNoHandScaling, nDefaultHandSize, nSingMissed, nForceMouth, nForceExpressions, nForceLook;

void loadConfig()
{    
    dobeep = GetPrivateProfileIntW(L"wig", L"beep", 1, CONFIG_FILE) > 0 ? true : false;
	allowReloading = GetPrivateProfileIntW(L"wig", L"reloading", 0, CONFIG_FILE) > 0 ? true : false;
	debug = GetPrivateProfileIntW(L"general", L"debug", 0, CONFIG_FILE) > 0 ? true : false;

    nSingMissed = GetPrivateProfileIntW(L"patches", L"sing_missed", FALSE, CONFIG_FILE) > 0 ? true : false;
    nNoHandScaling = GetPrivateProfileIntW(L"patches", L"no_hand_scaling", FALSE, CONFIG_FILE) > 0 ? true : false;
    nDefaultHandSize = GetPrivateProfileIntW(L"patches", L"default_hand_size", -1, CONFIG_FILE);
    nForceMouth = GetPrivateProfileIntW(L"patches", L"force_mouth", 0, CONFIG_FILE);
    nForceExpressions = GetPrivateProfileIntW(L"patches", L"force_expressions", 0, CONFIG_FILE);
    nForceLook = GetPrivateProfileIntW(L"patches", L"force_look", 0, CONFIG_FILE);

	return;
}

void pvPatches()
{
    cout << "[CustoMixer] Applying PV patches..." << endl;

    // Sing missed notes
    if (nSingMissed)
    {
        InjectCode((void*)0x140109044, { 0xEB });
        // Breaks border:
        // InjectCode((void*)0x140109096, { 0xEB });
    }

    // Force mouth animations
    {
        if (nForceMouth == 1) // PDA
        {
            printf("[Patches] Forcing PDA mouth...\n");
            int* mouth_table = (int*)(0x1409A1DC0);
            DWORD oldProtect;
            VirtualProtect(mouth_table, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 11; i++)
            {
                mouth_table[i]++;
            }
            VirtualProtect(mouth_table, 44, oldProtect, nullptr);
            printf("[Patches] PDA mouth forced\n");
        }
        else if (nForceMouth == 2) // FT
        {
            printf("[Patches] Forcing FT mouth...\n");
            int* mouth_table_oldid = (int*)(0x1409A1E1C);
            DWORD oldProtect;
            VirtualProtect(mouth_table_oldid, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 11; i++)
            {
                mouth_table_oldid[i]--;
            }
            VirtualProtect(mouth_table_oldid, 44, oldProtect, nullptr);
            printf("[Patches] FT mouth forced\n");
        }
    }

    // Force expressions
    {
        if (nForceExpressions == 1) // PDA
        {
            printf("[Patches] Forcing PDA expressions...\n");
            int* exp_table = (int*)(0x140A21900);
            DWORD oldProtect;
            VirtualProtect(exp_table, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 26; i++)
            {
                exp_table[i] += 2;
            }
            VirtualProtect(exp_table, 104, oldProtect, nullptr);
            printf("[Patches] PDA expressions forced\n");
        }
        else if (nForceExpressions == 2) // FT
        {
            printf("[Patches] Forcing FT expressions...\n");
            int* exp_table_oldid = (int*)(0x140A219D0);
            DWORD oldProtect;
            VirtualProtect(exp_table_oldid, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 26; i++)
            {
                exp_table_oldid[i] -= 2;
            }
            VirtualProtect(exp_table_oldid, 104, oldProtect, nullptr);
            printf("[Patches] FT expressions forced\n");
        }
    }

    // Force look animations
    {
        if (nForceLook == 1) // PDA
        {
            printf("[Patches] Forcing PDA look...\n");
            int* look_table = (int*)(0x1409A1D70);
            DWORD oldProtect;
            VirtualProtect(look_table, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 9; i++)
            {
                look_table[i]++;
            }
            VirtualProtect(look_table, 36, oldProtect, nullptr);
            printf("[Patches] PDA look forced\n");
        }
        else if (nForceLook == 2) // FT
        {
            printf("[Patches] Forcing FT look...\n");
            int* look_table_oldid = (int*)(0x1409A1D9C);
            DWORD oldProtect;
            VirtualProtect(look_table_oldid, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 9; i++)
            {
                look_table_oldid[i]--;
            }
            VirtualProtect(look_table_oldid, 36, oldProtect, nullptr);
            printf("[Patches] FT look forced\n");
        }
    }

    // Disable hand scaling
    if (nNoHandScaling)
    {
        InjectCode((void*)0x0000000140120709, { 0xE9, 0x82, 0x0A, 0x00 });
    }

    // Default hand size
    if (nDefaultHandSize != -1 && nDefaultHandSize < 1)
    {
        MessageBoxW(nullptr, L"The default hand size is lower than 1 and higher than -1 (default). Hands may disappear.\n\nIf this is not intentional, set it back to -1.", L"CustoMixer", MB_ICONWARNING);

        printf("[Patches] Changing default hand size...\n");
        const float num = (float)nDefaultHandSize / 10000.0;
        DWORD oldProtect;
        float* addr1 = (float*)(0x140506B59);
        float* addr2 = (float*)(0x140506B60);
        /*float* addr3 = (float*)(0x140506B67);
        float* addr4 = (float*)(0x140506B71);*/
        VirtualProtect(addr1, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        VirtualProtect(addr2, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        /*VirtualProtect(addr3, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
        VirtualProtect(addr4, 4, PAGE_EXECUTE_READWRITE, &oldProtect);*/
        *addr1 = *addr2 /*= *addr3 = *addr4*/ = num;
        VirtualProtect(addr1, 4, oldProtect, nullptr);
        VirtualProtect(addr2, 4, oldProtect, nullptr);
        /*VirtualProtect(addr3, 4, oldProtect, nullptr);
        VirtualProtect(addr4, 4, oldProtect, nullptr);*/
        printf("[Patches] New default hand size: %f\n", num);
    }

    cout << "[CustoMixer] PV patches applied." << endl;
}

void setPartToUpdate(char part, string message, unsigned short beepfreq)
{
	part_to_update = part;
	cout << "[CustoMixer] " << message << endl;
	if (dobeep) Beep(beepfreq, 300);

	this_thread::sleep_for(chrono::seconds(1));
}

void unloadAll()
{
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387286240);
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387319280);
	unloadFunc((uint64_t*)0x1411B7060, 0, 5387352320);
	cout << "[CustoMixer] All unloaded!" << endl;
	if (dobeep) Beep(500, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void unload(__int64 performer_address)
{
	unloadFunc((uint64_t*)0x1411B7060, 0, performer_address);
	cout << "[CustoMixer] Performer " << performer_address/33040 + 1 << " unloaded!" << endl;
	if (dobeep) Beep(500, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void loadAll()
{
	loadFunc((uint64_t*)0x1411B7060, 0, 5387286240);
	loadFunc((uint64_t*)0x1411B7060, 0, 5387319280);
	loadFunc((uint64_t*)0x1411B7060, 0, 5387352320);
	cout << "[CustoMixer] All loaded!" << endl;
	if (dobeep) Beep(600, 100);
	this_thread::sleep_for(chrono::seconds(1));;
}

void load(__int64 performer_address)
{
	loadFunc((uint64_t*)0x1411B7060, 0, performer_address);
	cout << "[CustoMixer] Performer " << performer_address / 33040 + 1 << " loaded!" << endl;
	if (dobeep) Beep(600, 100);
	this_thread::sleep_for(chrono::seconds(1));
}

/*void hookedUnload(uint64_t* a1, unsigned int a2, __int64 a3)
{
	cout << "[CustoMixer] hookedUnload a1: " << a1 << "; a2: " << a2 << "; a3: " << a3 << endl;
	last_unload_a1 = a1;
	last_unload_a2 = a2;
	last_unload_a3 = a3;
	unloadFunc(a1, a2, a3);
}

void hookedLoad(uint64_t* a1, unsigned int a2, __int64 a3)
{
	cout << "[CustoMixer] hookedReload a1: " << a1 << "; a2: " << a2 << "; a3: " << a3 << endl;
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
	if (debug) cout << "[CustoMixer] Performer: " << (classp-5387286232)/33040+1 << "; Module part: " << module_part << " (" << Strings[module_part] << ")" << "; Default part: " << part_id << "." << endl;
	
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
        pvPatches();
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		cout << "[CustoMixer] Hooking functions..." << endl;
		DetourAttach(&(PVOID&)originalWig, (PVOID)hookedWig);
		DetourAttach(&(PVOID&)originalSec, (PVOID)hookedSec);
		DetourAttach(&(PVOID&)originalThi, (PVOID)hookedThi);
		DetourAttach(&(PVOID&)pvTimerUpdate, (PVOID)inputLoop);
		cout << "[CustoMixer] Functions hooked." << endl;
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

const std::vector<LPCWSTR> typeList = { L"Default", L"PDA", L"PDAFT/PDFT/PDM39" };

PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"beep", L"wig", CONFIG_FILE, L"Beep", L"Beep when selecting the part that should be updated.", true } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"reloading", L"wig", CONFIG_FILE, L"Unloading and loading (experimental)", L"Allow unloading (CTRL+U or U+number) and loading (CTRL+L or L+number).", false } },
    { PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"sing_missed", L"patches", CONFIG_FILE, L"Sing Missed Notes", L"Sing Missed Notes.", false } },
    { PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"no_hand_scaling", L"patches", CONFIG_FILE, L"No Hand Scaling", L"Disable scripted hand scaling.", false } },
    { PluginConfig::CONFIG_NUMERIC, new PluginConfig::PluginConfigNumericData{ L"default_hand_size", L"patches", CONFIG_FILE, L"Default Hand Size:", L"-1: default\n12200: PDA", -1, -1, INT_MAX } },
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_mouth", L"patches", CONFIG_FILE, L"Mouth Animations:", L"Change the mouth animations.", 0, typeList}},
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_expressions", L"patches", CONFIG_FILE, L"Expr. Animations:", L"Change the expressions.", 0, typeList}},
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_look", L"patches", CONFIG_FILE, L"Look Animations:", L"Change the look animations.", 0, typeList}},
    { PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"debug", L"general", CONFIG_FILE, L"Debug", L"Print extra information.", false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"CustoMixer";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"CustoMixer Plugin by nas\n\nCustoMixer lets you change some PV settings, and mix module parts by pressing CTRL+0~9.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}
