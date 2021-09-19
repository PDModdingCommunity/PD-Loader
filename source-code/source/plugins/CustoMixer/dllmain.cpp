#include "PluginConfigApi.h"
#include "framework.h"

using namespace std;

int nNoHandScaling, nDefaultHandSize, nForceMouth, nForceExpressions, nForceLook;

void loadConfig()
{    
    nNoHandScaling = GetPrivateProfileIntW(L"patches", L"no_hand_scaling", FALSE, CONFIG_FILE) > 0 ? true : false;
    nDefaultHandSize = GetPrivateProfileIntW(L"patches", L"default_hand_size", -1, CONFIG_FILE);
    nForceMouth = GetPrivateProfileIntW(L"patches", L"force_mouth", 0, CONFIG_FILE);
    nForceExpressions = GetPrivateProfileIntW(L"patches", L"force_expressions", 0, CONFIG_FILE);
    nForceLook = GetPrivateProfileIntW(L"patches", L"force_look", 0, CONFIG_FILE);

	return;
}

void pvPatches()
{
    puts("[CustoMixer] Applying PV patches...");

    // Force mouth animations
    {
        if (nForceMouth == 1) // PDA
        {
            printf("[CustoMixer] Forcing PDA mouth...\n");
            int* mouth_table = (int*)(0x1409A1DC0);
            DWORD oldProtect;
            VirtualProtect(mouth_table, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 11; i++)
            {
                mouth_table[i]++;
            }
            VirtualProtect(mouth_table, 44, oldProtect, nullptr);
            printf("[CustoMixer] PDA mouth forced\n");
        }
        else if (nForceMouth == 2) // FT
        {
            printf("[CustoMixer] Forcing FT mouth...\n");
            int* mouth_table_oldid = (int*)(0x1409A1E1C);
            DWORD oldProtect;
            VirtualProtect(mouth_table_oldid, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 11; i++)
            {
                mouth_table_oldid[i]--;
            }
            VirtualProtect(mouth_table_oldid, 44, oldProtect, nullptr);
            printf("[CustoMixer] FT mouth forced\n");
        }
    }

    // Force expressions
    {
        if (nForceExpressions == 1) // PDA
        {
            printf("[CustoMixer] Forcing PDA expressions...\n");
            int* exp_table = (int*)(0x140A21900);
            DWORD oldProtect;
            VirtualProtect(exp_table, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 26; i++)
            {
                exp_table[i] += 2;
            }
            VirtualProtect(exp_table, 104, oldProtect, nullptr);
            printf("[CustoMixer] PDA expressions forced\n");
        }
        else if (nForceExpressions == 2) // FT
        {
            printf("[CustoMixer] Forcing FT expressions...\n");
            int* exp_table_oldid = (int*)(0x140A219D0);
            DWORD oldProtect;
            VirtualProtect(exp_table_oldid, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 26; i++)
            {
                exp_table_oldid[i] -= 2;
            }
            VirtualProtect(exp_table_oldid, 104, oldProtect, nullptr);
            printf("[CustoMixer] FT expressions forced\n");
        }
    }

    // Force look animations
    {
        if (nForceLook == 1) // PDA
        {
            printf("[CustoMixer] Forcing PDA look...\n");
            int* look_table = (int*)(0x1409A1D70);
            DWORD oldProtect;
            VirtualProtect(look_table, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 9; i++)
            {
                look_table[i]++;
            }
            VirtualProtect(look_table, 36, oldProtect, nullptr);
            printf("[CustoMixer] PDA look forced\n");
        }
        else if (nForceLook == 2) // FT
        {
            printf("[CustoMixer] Forcing FT look...\n");
            int* look_table_oldid = (int*)(0x1409A1D9C);
            DWORD oldProtect;
            VirtualProtect(look_table_oldid, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
            for (int i = 0; i < 9; i++)
            {
                look_table_oldid[i]--;
            }
            VirtualProtect(look_table_oldid, 36, oldProtect, nullptr);
            printf("[CustoMixer] FT look forced\n");
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
        MessageBoxW(nullptr, L"The default hand size is lower than 1 and higher than -1 (default). Hands may disappear.\n\nIf this is not intentional, set it back to -1.", L"DivaWig", MB_ICONWARNING);

        printf("[CustoMixer] Changing default hand size...\n");
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
        printf("[CustoMixer] New default hand size: %f\n", num);
    }

    puts("[CustoMixer] PV patches applied.");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();
        pvPatches();
	}

	return TRUE;
}

const std::vector<LPCWSTR> typeList = { L"Default", L"PDA", L"PDAFT/PDFT/PDM39" };

PluginConfig::PluginConfigOption config[] = {
    { PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"no_hand_scaling", L"patches", CONFIG_FILE, L"No Hand Scaling", L"Disable scripted hand scaling.", false } },
    { PluginConfig::CONFIG_NUMERIC, new PluginConfig::PluginConfigNumericData{ L"default_hand_size", L"patches", CONFIG_FILE, L"Default Hand Size:", L"-1: default\n12200: PDA", -1, -1, INT_MAX } },
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_mouth", L"patches", CONFIG_FILE, L"Mouth Animations:", L"Change the mouth animations.", 0, typeList}},
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_expressions", L"patches", CONFIG_FILE, L"Expr. Animations:", L"Change the expressions.", 0, typeList}},
    { PluginConfig::CONFIG_DROPDOWN_INDEX, new PluginConfig::PluginConfigDropdownIndexData{ L"force_look", L"patches", CONFIG_FILE, L"Look Animations:", L"Change the look animations.", 0, typeList}},
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"CustoMixer";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"CustoMixer Plugin by nas\n\DivaWig lets you change some PV settings.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}
