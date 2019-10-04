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

		for (std::string &arch : patch.archs)
		{
			if (gpuName.size() >= arch.size() && gpuName.rfind(arch, 0) == 0)
			{
				archMatches = true;
				break;
			}
		}

		if (archMatches && std::regex_match(file.fileName.GetCharBuf(), patch.fileRegex))
		{
			if (modifiedStr.length() == 0)
				modifiedStr = std::string(file.data, file.dataSize);

			modifiedStr = std::regex_replace(modifiedStr, patch.dataRegex, patch.dataReplace);
		}
	}

	if (modifiedStr.length() > 0)
	{
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