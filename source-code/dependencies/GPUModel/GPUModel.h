/*
Simple header for detecting Nvidia GPU models
*/

namespace GPUModel
{
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <string>

	void*(*NvAPI_QueryInterface)(unsigned int offset) = NULL;
	int(*NvAPI_Initialize)() = NULL;
	int(*NvAPI_Unload)() = NULL;
	int(*NvAPI_EnumPhysicalGPUs)(int64_t** handle, int* count) = NULL;
	int(*NvAPI_GPU_GetShortName)(int64_t* handle, char* name) = NULL;

	std::string getGpuName()
	{
		NvAPI_QueryInterface = (void*(*)(unsigned int offset))GetProcAddress(LoadLibraryW(L"nvapi64.dll"), "nvapi_QueryInterface");

		if (NvAPI_QueryInterface == nullptr)
		{
			return "Other";
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
			return "Unknown";
		}

		int64_t* hdlGpu[64] = { 0 };
		int nGpu = 0;

		NvAPI_Initialize();
		NvAPI_EnumPhysicalGPUs(hdlGpu, &nGpu);

		//printf("[ShaderPatch] nGpu: %d\n", nGpu);
		if (nGpu < 1)
		{
			NvAPI_Unload();
			return "Unknown";
		}

		char nameBuf[64];
		NvAPI_GPU_GetShortName(hdlGpu[0], nameBuf);
		NvAPI_Unload();

		return std::string(nameBuf);
	}
}