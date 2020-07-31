/*
Simple header for detecting version of Wine
*/

namespace WineVer
{
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <string>
	#include <stdio.h>
	#pragma comment(lib, "advapi32")

	std::string getWineVer()
	{
		HMODULE hntdll = LoadLibraryW(L"ntdll.dll");
		
		if (hntdll == NULL)
			return "";
		
		char*(WINAPI * wine_get_version)() = (char*(WINAPI*)())GetProcAddress(hntdll, "wine_get_version");
		if (wine_get_version)
		{
			//puts("Wine detected.");
			//printf("Running on Wine... %s\n", wine_get_version());
			return std::string(wine_get_version());
		}
		return "";
	}
}