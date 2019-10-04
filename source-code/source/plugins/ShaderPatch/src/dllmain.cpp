#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "framework.h"
//#include "PluginConfigApi.h"
#include "detours.h"
#pragma comment(lib, "detours.lib")

#include <vector>
#include <regex>

void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

void NopBytes(void* address, unsigned int num)
{
	std::vector<uint8_t> newbytes = {};

	for (unsigned int i = 0; i < num; ++i) newbytes.push_back(0x90);

	InjectCode(address, newbytes);
}


void loadGpuName()
{
	NvAPI_QueryInterface = (void*(*)(unsigned int offset))GetProcAddress(LoadLibrary("nvapi64.dll"), "nvapi_QueryInterface");

	if (NvAPI_QueryInterface == nullptr)
	{
		gpuName = "Other";
		return;
	}

	NvAPI_Initialize = (int(*)())NvAPI_QueryInterface(0x0150E828);
	NvAPI_Unload = (int(*)()) NvAPI_QueryInterface(0xD22BDD7E);
	NvAPI_EnumPhysicalGPUs = (int(*)(int64_t** handle, int* count))NvAPI_QueryInterface(0xE5AC921F);
	NvAPI_GPU_GetShortName = (int(*)(int64_t* handle, char* name))NvAPI_QueryInterface(0xD988F0F3);

	if (NvAPI_Initialize == nullptr ||
		NvAPI_Unload == nullptr ||
		NvAPI_EnumPhysicalGPUs == nullptr ||
		NvAPI_GPU_GetShortName == nullptr)
	{
		gpuName = "Unknown";
		return;
	}

	int64_t* hdlGpu[64] = { 0 };
	int nGpu = 0;

	NvAPI_Initialize();
	NvAPI_EnumPhysicalGPUs(hdlGpu, &nGpu);

	//printf("[ShaderPatch] nGpu: %d\n", nGpu);
	if (nGpu < 1)
	{
		gpuName = "Unknown";
		NvAPI_Unload();
		return;
	}

	char nameBuf[64];
	NvAPI_GPU_GetShortName(hdlGpu[0], nameBuf);
	NvAPI_Unload();

	gpuName = nameBuf;
	return;
}


void LoadConfig()
{
	std::ifstream fileStream(CONFIG_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;
	std::string section = "patches";

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		if (line.size() <= 0 || line[0] == '#' || (line.size() >= 2 && line.rfind("//", 0) == 0))
			continue;

		if (line[0] == '[') // section name
		{
			size_t endIdx = line.find(']');
			section = line.substr(1, endIdx - 1);
			std::transform(section.begin(), section.end(), section.begin(), ::tolower);
			continue;
		}

		std::vector<std::string> equalSplit = SplitString(line, "=");
		if (equalSplit.size() < 2) continue;

		if (section == "patches")
		{
			ShaderPatchInfo patch = ShaderPatchInfo();

			patch.fileRegex = equalSplit[0];

			std::vector<std::string> rules = SplitString(equalSplit[1], "||");
			if (rules.size() < 1) continue; // probably will never trigger but whatever

			for (std::string &rule : rules)
			{
				if (rule.size() >= 5 && rule.rfind("arch:", 0) == 0)
				{
					rule.erase(0, 5);
					patch.archs = SplitString(rule, ",");
				}
				else if (rule.size() >= 4 && rule.rfind("cfg:", 0) == 0)
				{
					rule.erase(0, 4);
					// force cfg key to lower because ini shouldn't be case sensitive
					std::transform(rule.begin(), rule.end(), rule.begin(), ::tolower);
					patch.cfg = rule;
				}
				else if (rule.size() >= 5 && rule.rfind("from:", 0) == 0)
				{
					rule.erase(0, 5);
					patch.dataRegex = rule;
				}
				else if (rule.size() >= 3 && rule.rfind("to:", 0) == 0)
				{
					rule.erase(0, 3);
					patch.dataReplace = rule;
				}
			}

			patchesVec.push_back(patch);
		}
		else if (section == "config")
		{
			equalSplit[1] = TrimString(equalSplit[1], " \t");
			if (equalSplit[1] != "0") // ignore when == 0 to allow disabling via checkbox
			{
				// force cfg key to lower because ini shouldn't be case sensitive
				std::transform(equalSplit[0].begin(), equalSplit[0].end(), equalSplit[0].begin(), ::tolower);
				configMap.insert(std::pair<std::string, std::string>(equalSplit[0], equalSplit[1]));
			}
		}
	}

	fileStream.close();
}


void hookedLoadFromFarcThunk(FArchivedFile** ppFile)
{
	loadFromFarcThunk(ppFile);

	if (ppFile == nullptr || *ppFile == nullptr)
		return;

	FArchivedFile &file = **ppFile;

	if (file.notLoadedFlag != 0)
		return;

	std::string modifiedStr = "";

	for (ShaderPatchInfo &patch : patchesVec)
	{
		bool archMatches = false;
		if (patch.archs.size() > 0)
		{
			for (std::string &arch : patch.archs)
			{
				if (gpuName.size() >= arch.size() && gpuName.rfind(arch, 0) == 0)
				{
					archMatches = true;
					break;
				}
			}
		}
		else
		{
			archMatches = true;
		}

		bool cfgMatches = false;
		if (patch.cfg.length() == 0 || configMap.find(patch.cfg) != configMap.end())
		{
			cfgMatches = true;
		}

		if (archMatches && cfgMatches && std::regex_match(file.fileName.GetCharBuf(), patch.fileRegex))
		{
			if (modifiedStr.length() == 0)
				modifiedStr = std::string(file.data, file.dataSize);

			modifiedStr = std::regex_replace(modifiedStr, patch.dataRegex, patch.dataReplace);

			if (patch.cfg.length() > 0)
			{
				modifiedStr = StringReplace(modifiedStr, "<cfg>", configMap[patch.cfg]);
			}
		}
	}

	if (modifiedStr.length() > 0)
	{
		free(file.data);
		file.data = _strdup(modifiedStr.c_str());
		file.dataSize = modifiedStr.size();
		printf("[ShaderPatch] Patched %s\n", file.fileName.GetCharBuf());
	}
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		// install a thunk to hook
		InjectCode((void*)0x1405e5991, { 0x48, 0x83, 0xec, 0x20 });       // SUB  RSP, 0x20
		InjectCode((void*)0x1405e5995, { 0xe8, 0x56, 0xe8, 0xb9, 0xff }); // CALL  0x1401841f0
		InjectCode((void*)0x1405e599a, { 0x48, 0x83, 0xc4, 0x20 });       // ADD  RSP, 0x20
		InjectCode((void*)0x1405e599e, { 0xc3 });                         // RET
		InjectCode((void*)0x1405e534a, { 0xe8, 0x42, 0x06, 0x00, 0x00 }); // CALL  0x1405e5991

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)loadFromFarcThunk, (PVOID)hookedLoadFromFarcThunk);
		DetourTransactionCommit();

		loadGpuName();
		printf("[ShaderPatch] Detected GPU: %s\n", gpuName);
		//Sleep(2000);

		LoadConfig();
	}
	return TRUE;
}


//using namespace PluginConfig;
//
//extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
//{
//	return L"ShaderPatch";
//}
//
//extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
//{
//	return L"Automatically patches shaders";
//}