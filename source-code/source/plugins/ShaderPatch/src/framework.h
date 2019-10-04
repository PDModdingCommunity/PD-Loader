#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>
#include <vector>
#include <regex>
#include <stdint.h>
#include <fstream>


struct ShaderPatchInfo {
	std::regex fileRegex;
	std::regex dataRegex;
	std::string dataReplace;
	std::vector<std::string> archs;
};

std::vector<ShaderPatchInfo> patchesVec;

#pragma pack(push, 1)
struct MsString {
	union {
		char* string_ptr;
		char string_buf[16];
	};
	uint64_t unk;
	uint64_t size;

	char* GetCharBuf()
	{
		if (size > 0xf && string_ptr != nullptr)
			return string_ptr;
		else
			return string_buf;
	};
};

struct FArchivedFile {
	uint8_t padding00[0x10];
	uint8_t notLoadedFlag;
	uint8_t padding11[0x07];
	MsString farcPath;
	MsString fileName;
	uint8_t padding58[0x38];
	uint64_t dataSize;
	char* data;
};
#pragma pack(pop)

void (__stdcall* loadFromFarcThunk)(FArchivedFile** file) = (void(__stdcall*)(FArchivedFile** file))0x1405e5991;


std::string gpuName;
void*(*NvAPI_QueryInterface)(unsigned int offset) = NULL;
int(*NvAPI_Initialize)() = NULL;
int(*NvAPI_Unload)() = NULL;
int(*NvAPI_EnumPhysicalGPUs)(int64_t** handle, int* count) = NULL;
int(*NvAPI_GPU_GetShortName)(int64_t* handle, char* name) = NULL;

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



std::vector<std::string> SplitString(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos)
			pos = str.length();

		std::string token = str.substr(prev, pos - prev);

		if (!token.empty())
			tokens.push_back(token);

		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());

	return tokens;
}


std::wstring ExePath() {
	WCHAR buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	return std::wstring(buffer);
}

std::wstring DirPath() {
	std::wstring exepath = ExePath();
	std::wstring::size_type pos = exepath.find_last_of(L"\\/");
	return exepath.substr(0, pos);
}

std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\ShaderPatch.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

void LoadConfig()
{
	std::ifstream fileStream(CONFIG_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		try {
			if (line.size() <= 0 || line[0] == '#' || line[0] == '[' || (line.size() >= 2 && line.rfind("//", 0) == 0))
				continue;

			std::vector<std::string> equalSplit = SplitString(line, "=");
			if (equalSplit.size() < 2) continue;

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
		catch (int e)
		{

		}
	}

	fileStream.close();
}