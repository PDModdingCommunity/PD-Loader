#pragma once

#define WIN32_LEAN_AND_MEAN

#include <cstdio>
#include <windows.h>
#include <detours.h>
#include <string>

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

std::wstring CONFIG_FILE_STRING = DirPath() + L"\\plugins\\DivaMovie.ini";
LPCWSTR CONFIG_FILE = CONFIG_FILE_STRING.c_str();

std::wstring MASTER_CONFIG_FILE_STRING = DirPath() + L"\\plugins\\config.ini";
LPCWSTR MASTER_CONFIG_FILE = MASTER_CONFIG_FILE_STRING.c_str();

#define PRINT(value, ...) if (debug) printf(value, __VA_ARGS__);

#define PROC_ADDRESS(libraryName, procName) \
	GetProcAddress(LoadLibrary(TEXT(libraryName)), procName)

#define SAFE_RELEASE(object) if (object) { object->Release(); object = NULL; }

#define VTABLE_HOOK(returnType, className, functionName, ...) \
	typedef returnType functionName(className* This, __VA_ARGS__); \
	functionName* original##functionName; \
	returnType implOf##functionName(className* This, __VA_ARGS__)

#define INSTALL_VTABLE_HOOK(object, functionName, functionIndex) \
	{ \
		void** addr = &(*(void***)object)[functionIndex]; \
		if (*addr != implOf##functionName) \
		{ \
			PRINT("[DivaMovie] Installing %s hook...\n", #functionName); \
			original##functionName = (functionName*)*addr; \
			DWORD oldProtect; \
			VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect); \
			*addr = implOf##functionName; \
			VirtualProtect(addr, sizeof(void*), oldProtect, NULL); \
		} \
	}

#define HOOK(returnType, function, location, ...) \
    typedef returnType function(__VA_ARGS__); \
    function* original##function = (function*)location; \
    returnType implOf##function(__VA_ARGS__)

#define INSTALL_HOOK(functionName) \
	{ \
		PRINT("[DivaMovie] Installing %s hook...\n", #functionName); \
		DetourTransactionBegin(); \
		DetourUpdateThread(GetCurrentThread()); \
		DetourAttach((void**)&original##functionName, implOf##functionName); \
		DetourTransactionCommit(); \
	}

#define ASSERT(function) \
	if (FAILED(result = (function))) \
	{ \
		PRINT("[DivaMovie] %s failed (error code: 0x%08x)\n", #function, (unsigned int)result); \
		goto end; \
	} /*\
	else \
	{ \
		PRINT("[DivaMovie] %s succeeded\n", #function); \
	}*/
