/*
Simple header for detecting Nvidia GPU models
*/

namespace GPUModel
{
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <string>
	#include <stdio.h>
	#pragma comment(lib, "advapi32")

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

	std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\config.ini";
	LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

	void*(*NvAPI_QueryInterface)(unsigned int offset) = NULL;
	int(*NvAPI_Initialize)() = NULL;
	int(*NvAPI_Unload)() = NULL;
	int(*NvAPI_EnumPhysicalGPUs)(int64_t** handle, int* count) = NULL;
	int(*NvAPI_GPU_GetShortName)(int64_t* handle, char* name) = NULL;

	std::string getGpuName()
	{
		// detected model can be overridden -- 0: Kepler, 1: Maxwell, 2: Turing
		int nGPUModel = GetPrivateProfileIntW(L"gpu", L"model", -1, CONFIG_FILE);
		if (nGPUModel >= 0 && nGPUModel <= 2)
		{
			std::string arch;
			switch (nGPUModel)
			{
			case 0:
				arch = "GK000";
				break;
			case 1:
				arch = "GM000";
				break;
			case 2:
				arch = "TU000";
				break;
			}

			printf("[GPUModel] GPU Model override: %s\n", arch.c_str());
			return arch;
		}

		printf("[GPUModel] Checking GPU model\n");
		HMODULE hdlNvapi = LoadLibraryW(L"nvapi64.dll");

		if (hdlNvapi == NULL) return
			std::string("Other");

		NvAPI_QueryInterface = (void* (*)(unsigned int offset))GetProcAddress(hdlNvapi, "nvapi_QueryInterface");
		if (NvAPI_QueryInterface == nullptr)
			return std::string("Other");

		NvAPI_Initialize = (int(*)())NvAPI_QueryInterface(0x0150E828);
		NvAPI_Unload = (int(*)()) NvAPI_QueryInterface(0xD22BDD7E);
		NvAPI_EnumPhysicalGPUs = (int(*)(int64_t * *handle, int* count))NvAPI_QueryInterface(0xE5AC921F);
		NvAPI_GPU_GetShortName = (int(*)(int64_t * handle, char* name))NvAPI_QueryInterface(0xD988F0F3);

		if (NvAPI_Initialize == nullptr ||
			NvAPI_Unload == nullptr ||
			NvAPI_EnumPhysicalGPUs == nullptr ||
			NvAPI_GPU_GetShortName == nullptr)
			return std::string("Unknown");

		int64_t* hdlGpu[64] = { 0 };
		int nGpu = 0;

		NvAPI_Initialize();
		NvAPI_EnumPhysicalGPUs(hdlGpu, &nGpu);

		//printf("[ShaderPatch] nGpu: %d\n", nGpu);
		if (nGpu < 1)
		{
			NvAPI_Unload();
			return std::string("Unknown");
		}

		char nameBuf[64];
		NvAPI_GPU_GetShortName(hdlGpu[0], nameBuf);
		NvAPI_Unload();

		return std::string(nameBuf);
	}
}