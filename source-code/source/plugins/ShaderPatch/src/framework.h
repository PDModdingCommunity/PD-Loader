#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <stdint.h>
#include <fstream>


struct ShaderPatchInfo {
	std::regex fileRegex;
	std::regex dataRegex;
	std::string dataReplace;
	std::vector<std::string> archs;
	std::string cfg;
};

std::vector<ShaderPatchInfo> patchesVec;

std::map<std::string, std::string> configMap;

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

std::string TrimString(const std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);

	if (strBegin == std::string::npos)
		return "";

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string StringReplace(const std::string& str, const std::string& srch, const std::string& repl)
{
	size_t pos = 0;
	std::string outstr = str;

	while (true)
	{
		pos = outstr.find(srch, pos);
		if (pos == std::string::npos)
			break;

		outstr.replace(pos, srch.length(), repl);
	}

	return outstr;
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

