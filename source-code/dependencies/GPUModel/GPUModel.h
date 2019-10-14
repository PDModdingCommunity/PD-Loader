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

	void*(*NvAPI_QueryInterface)(unsigned int offset) = NULL;
	int(*NvAPI_Initialize)() = NULL;
	int(*NvAPI_Unload)() = NULL;
	int(*NvAPI_EnumPhysicalGPUs)(int64_t** handle, int* count) = NULL;
	int(*NvAPI_GPU_GetShortName)(int64_t* handle, char* name) = NULL;

	std::string getWineVer()
	{
		HMODULE hntdll = LoadLibraryW(L"ntdll.dll");
		char*(WINAPI * wine_get_version)() = (char*(WINAPI*)())GetProcAddress(hntdll, "wine_get_version");
		if (wine_get_version)
		{
			//puts("Wine detected.");
			//printf("Running on Wine... %s\n", wine_get_version());
			return std::string(wine_get_version());
		}
		return "";
	}

	std::string getGpuName()
	{
/*		if (getWineVer() != "")
		{
			return "Unknown";
		}
		else {*/
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
		//}
	}
}