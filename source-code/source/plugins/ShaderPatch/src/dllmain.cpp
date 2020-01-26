#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "framework.h"
#include "PluginConfigApi.h"
#include "GPUModel.h"
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





void LoadData()
{
	std::ifstream fileStream(DATA_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;
	std::string section = "patches";
	std::string lastComment;
	bool isInComment = false;

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		// detect comments first to make comment exit logic easier
		int commentStartPos;
		if ((commentStartPos = 1, line.size() >= 1 && line[0] == '#') || (commentStartPos = 2, line.size() >= 2 && line.rfind("//", 0) == 0))
		{
			line.erase(0, commentStartPos);
			line = TrimString(line, " \t");
			if (isInComment)
			{
				lastComment += "\n" + line;
			}
			else
			{
				lastComment = line;
				isInComment = true;
			}
			continue;
		}

		isInComment = false;

		if (line.size() <= 0) // skip empty lines
		{
			lastComment = ""; // consume the last comment
			continue;
		}

		if (line[0] == '[') // section name
		{
			size_t endIdx = line.find(']');
			section = line.substr(1, endIdx - 1);
			std::transform(section.begin(), section.end(), section.begin(), ::tolower);
			lastComment = ""; // consume the last comment
			continue;
		}

		std::vector<std::string> equalSplit = SplitString(line, "=");
		if (equalSplit.size() < 2)
		{
			lastComment = ""; // consume the last comment
			continue;
		}

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
					rule = StringReplace(rule, "\\n", "\n");
					patch.dataReplace = rule;
				}
			}
			lastComment = ""; // consume the last comment

			patchesVec.push_back(patch);
		}
		else if (section == "config")
		{
			// force cfg key to lower because ini shouldn't be case sensitive
			std::transform(equalSplit[0].begin(), equalSplit[0].end(), equalSplit[0].begin(), ::tolower);
			equalSplit[0] = TrimString(equalSplit[0], " \t");
			equalSplit[1] = TrimString(equalSplit[1], " \t");

			dataCfgMap[equalSplit[0]] = strpair(equalSplit[1], lastComment);
			lastComment = ""; // consume the last comment
		}
	}

	fileStream.close();
}

// always call this after LoadData
void LoadUserCfg()
{
	// copy default config into userconfig to avoid having to read from multiple maps while patching
	for (std::map<std::string, strpair>::iterator iter = dataCfgMap.begin(); iter != dataCfgMap.end(); ++iter)
	{
		userCfgMap[iter->first] = iter->second.first;
	}

	std::ifstream fileStream(CONFIG_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;
	std::string section = "config";

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		if (line.size() <= 0) // skip empty lines
		{
			continue;
		}

		// skip comments
		if ((line.size() >= 1 && line[0] == '#') || (line.size() >= 2 && line.rfind("//", 0) == 0))
		{
			continue;
		}

		if (line[0] == '[') // section name
		{
			size_t endIdx = line.find(']');
			section = line.substr(1, endIdx - 1);
			std::transform(section.begin(), section.end(), section.begin(), ::tolower);
			continue;
		}

		std::vector<std::string> equalSplit = SplitString(line, "=");
		if (equalSplit.size() < 2)
		{
			continue;
		}

		if (section == "config")
		{
			// force cfg key to lower because ini shouldn't be case sensitive
			std::transform(equalSplit[0].begin(), equalSplit[0].end(), equalSplit[0].begin(), ::tolower);
			equalSplit[0] = TrimString(equalSplit[0], " \t");
			equalSplit[1] = TrimString(equalSplit[1], " \t");

			userCfgMap[equalSplit[0]] = equalSplit[1];
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
		if (patch.cfg.length() == 0 || // patch has no config setting
			(userCfgMap.find(patch.cfg) != userCfgMap.end() && userCfgMap[patch.cfg] != "0")) // patch has a toggle and is not set to 0
		{
			cfgMatches = true;
		}

		if (archMatches && cfgMatches && std::regex_match(file.fileName.GetCharBuf(), patch.fileRegex))
		{
			if (modifiedStr.length() == 0)
				modifiedStr = std::string(file.data, file.dataSize);

			modifiedStr = std::regex_replace(modifiedStr, patch.dataRegex, patch.dataReplace);
			modifiedStr = StringReplace(modifiedStr, "<fname>", file.fileName.GetCharBuf());

			if (patch.cfg.length() > 0) // patch has a config setting
			{
				int valNum = 0;
				std::string valKey;
				while (valNum++, valKey = patch.cfg + "_val" + std::to_string(valNum),
					userCfgMap.find(valKey) != userCfgMap.end()) // loop until there's no more config values set
				{
					modifiedStr = StringReplace(modifiedStr, "<val" + std::to_string(valNum) + ">", userCfgMap[valKey]);
				}
			}
		}
	}

	if (modifiedStr.length() > 0)
	{
		//free(file.data);
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
		InjectCode((void*)0x1405e5991, { 0x48, 0x83, 0xec, 0x28 });       // SUB  RSP, 0x28
		InjectCode((void*)0x1405e5995, { 0xe8, 0x56, 0xe8, 0xb9, 0xff }); // CALL  0x1401841f0
		InjectCode((void*)0x1405e599a, { 0x48, 0x83, 0xc4, 0x28 });       // ADD  RSP, 0x28
		InjectCode((void*)0x1405e599e, { 0xc3 });                         // RET
		InjectCode((void*)0x1405e534a, { 0xe8, 0x42, 0x06, 0x00, 0x00 }); // CALL  0x1405e5991

		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)loadFromFarcThunk, (PVOID)hookedLoadFromFarcThunk);
		DetourTransactionCommit();

		gpuName = GPUModel::getGpuName();
		printf("[ShaderPatch] Detected GPU: %s\n", gpuName.c_str());
		//Sleep(2000);

		LoadData();
		LoadUserCfg();
	}
	return TRUE;
}


using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"ShaderPatch";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Automatically patches shaders";
}

std::vector<PluginConfigOption> configVec;
extern "C" __declspec(dllexport) PluginConfigArray GetPluginOptions(void)
{
	LoadData();
	LoadUserCfg();

	for (std::map<std::string, strpair>::iterator iter = dataCfgMap.begin(); iter != dataCfgMap.end(); ++iter)
	{
		std::string k = iter->first;
		std::string v = iter->second.first;
		std::string c = iter->second.second;



		if (k.size() < 6 || k.substr(k.size() - 5, 4) != "_val")
		{
			WCHAR utf16key[128];
			WCHAR utf16val[128];
			WCHAR utf16comment[512];

			// count values to approximate the group size
			int valNum = 0;
			int valCount = 0;
			std::string valKey;
			while (valNum++, valKey = k + "_val" + std::to_string(valNum),
				dataCfgMap.find(valKey) != dataCfgMap.end()) // loop until there's no more config values set
			{
				valCount++;
			}


			MultiByteToWideChar(CP_UTF8, 0, k.c_str(), -1, utf16key, 128);
			MultiByteToWideChar(CP_UTF8, 0, c.c_str(), -1, utf16comment, 512);
			LPCWSTR ttDup = _wcsdup(utf16comment);

			configVec.push_back({ CONFIG_GROUP_START, new PluginConfigGroupData{ _wcsdup(utf16key), 45 + valCount * 25 } });
			configVec.push_back({ CONFIG_BOOLEAN, new PluginConfigBooleanData{ _wcsdup(utf16key), L"config", CONFIG_FILE, L"Enable", ttDup, v == "0" ? false : true, false } }); // could use userCfgMap here, but launcher handles that anyway


			valNum = 0;
			while (valNum++, valKey = k + "_val" + std::to_string(valNum),
				dataCfgMap.find(valKey) != dataCfgMap.end()) // loop until there's no more config values set
			{
				MultiByteToWideChar(CP_UTF8, 0, valKey.c_str(), -1, utf16key, 128);
				MultiByteToWideChar(CP_UTF8, 0, dataCfgMap[valKey].first.c_str(), -1, utf16val, 128); // could use userCfgMap here, but launcher handles that anyway

				std::wstring name;
				if (dataCfgMap[valKey].second.size() > 0)
				{
					MultiByteToWideChar(CP_UTF8, 0, dataCfgMap[valKey].second.c_str(), -1, utf16comment, 512);
					name = utf16comment;
					name += L":";
;				}
				else
				{
					name = std::wstring(L"Value ") + std::to_wstring(valNum) + L":";
				}
				configVec.push_back({ CONFIG_STRING, new PluginConfigStringData{ _wcsdup(utf16key), L"config", CONFIG_FILE, _wcsdup(name.c_str()), ttDup, _wcsdup(utf16val), false } });
			}


			configVec.push_back({ CONFIG_GROUP_END, NULL });
			configVec.push_back({ CONFIG_SPACER, new PluginConfigSpacerData{ 10 } });
		}
	}

	return PluginConfigArray{ (int)configVec.size(), &configVec[0] };
}