#include "framework.h"
#include "exception.hpp"

HMODULE hm;
std::vector<std::wstring> iniPaths;

bool iequals(std::wstring_view s1, std::wstring_view s2)
{
	std::wstring str1(std::move(s1));
	std::wstring str2(std::move(s2));
	std::transform(str1.begin(), str1.end(), str1.begin(), [](wchar_t c) { return ::towlower(c); });
	std::transform(str2.begin(), str2.end(), str2.begin(), [](wchar_t c) { return ::towlower(c); });
	return (str1 == str2);
}

std::wstring to_wstring(std::string_view cstr)
{
	std::string str(std::move(cstr));
	auto charsReturned = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(charsReturned, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], charsReturned);
	return wstrTo;
}

std::wstring SHGetKnownFolderPath(REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken)
{
	std::wstring r;
	WCHAR* szSystemPath = nullptr;
	if (SUCCEEDED(SHGetKnownFolderPath(rfid, dwFlags, hToken, &szSystemPath)))
	{
		r = szSystemPath;
	}
	CoTaskMemFree(szSystemPath);
	return r;
};

HMODULE LoadLibraryW(const std::wstring& lpLibFileName)
{
	return LoadLibraryW(lpLibFileName.c_str());
}

std::wstring GetCurrentDirectoryW()
{
	static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
	static constexpr auto MAX_ITERATIONS = 7;
	std::wstring ret;
	auto bufferSize = INITIAL_BUFFER_SIZE;
	for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations)
	{
		ret.resize(bufferSize);
		auto charsReturned = GetCurrentDirectoryW(bufferSize, &ret[0]);
		if (charsReturned < ret.length())
		{
			ret.resize(charsReturned);
			return ret;
		}
		else
		{
			bufferSize *= 2;
		}
	}
	return L"";
}

UINT GetPrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, INT nDefault, const std::vector<std::wstring>& fileNames)
{
	for (const auto& file : fileNames)
	{
		nDefault = GetPrivateProfileIntW(lpAppName, lpKeyName, nDefault, file.c_str());
	}
	return nDefault;
}

std::wstring GetSelfName()
{
	const std::wstring moduleFileName = GetModuleFileNameW(hm);
	return moduleFileName.substr(moduleFileName.find_last_of(L"/\\") + 1);
}

template<typename T, typename... Args>
void GetSections(T&& h, Args... args)
{
	const std::set< std::string_view, std::less<> > s = { args... };
	size_t dwLoadOffset = (size_t)GetModuleHandle(NULL);
	BYTE* pImageBase = reinterpret_cast<BYTE*>(dwLoadOffset);
	PIMAGE_DOS_HEADER   pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(dwLoadOffset);
	PIMAGE_NT_HEADERS   pNtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(pImageBase + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
	for (int iSection = 0; iSection < pNtHeader->FileHeader.NumberOfSections; ++iSection, ++pSection)
	{
		auto pszSectionName = reinterpret_cast<const char*>(pSection->Name);
		if (s.find(pszSectionName) != s.end())
		{
			DWORD dwPhysSize = (pSection->Misc.VirtualSize + 4095) & ~4095;
			std::forward<T>(h)(pSection, dwLoadOffset, dwPhysSize);
		}
	}
}

enum Kernel32ExportsNames
{
	eGetStartupInfoA,
	eGetStartupInfoW,
	eGetModuleHandleA,
	eGetModuleHandleW,
	eGetProcAddress,
	eGetShortPathNameA,
	eFindNextFileA,
	eFindNextFileW,
	eLoadLibraryA,
	eLoadLibraryW,
	eFreeLibrary,
	eCreateEventA,
	eCreateEventW,
	eGetSystemInfo,
	eInterlockedCompareExchange,
	eSleep,

	Kernel32ExportsNamesCount
};

enum Kernel32ExportsData
{
	IATPtr,
	ProcAddress,

	Kernel32ExportsDataCount
};

size_t Kernel32Data[Kernel32ExportsNamesCount][Kernel32ExportsDataCount];

static LONG OriginalLibraryLoaded = 0;
void LoadOriginalLibrary()
{
	if (_InterlockedCompareExchange(&OriginalLibraryLoaded, 1, 0) != 0) return;

	auto szSelfName = GetSelfName();
	auto szSystemPath = SHGetKnownFolderPath(FOLDERID_System, 0, nullptr) + L'\\' + szSelfName;
	auto szLocalPath = GetModuleFileNameW(hm); szLocalPath = szLocalPath.substr(0, szLocalPath.find_last_of(L"/\\") + 1);

	if (iequals(szSelfName, L"dnsapi.dll")) {
		dnsapi.LoadOriginalLibrary(LoadLibraryW(szSystemPath));
	}
}

void FindFiles(WIN32_FIND_DATAW* fd)
{
	auto dir = GetCurrentDirectoryW();

	HANDLE dvaFile = FindFirstFileW(L"*.dva", fd);
	if (dvaFile != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				auto pos = wcslen(fd->cFileName);

				if (fd->cFileName[pos - 4] == '.' &&
					(fd->cFileName[pos - 3] == 'd' || fd->cFileName[pos - 3] == 'D') &&
					(fd->cFileName[pos - 2] == 'v' || fd->cFileName[pos - 2] == 'V') &&
					(fd->cFileName[pos - 1] == 'a' || fd->cFileName[pos - 1] == 'A') &&
					GetPrivateProfileIntW(L"plugins", fd->cFileName, TRUE, iniPaths))
				{
					auto path = dir + L'\\' + fd->cFileName;

					if (GetModuleHandle(path.c_str()) == NULL)
					{
						auto h = LoadLibraryW(path);
						SetCurrentDirectoryW(dir.c_str()); //in case dva switched it

						if (h == NULL)
						{
							auto e = GetLastError();
							if (e != ERROR_DLL_INIT_FAILED) // in case dllmain returns false
							{
								std::wstring msg = L"Unable to load " + std::wstring(fd->cFileName) + L". Error: " + std::to_wstring(e);
								MessageBoxW(0, msg.c_str(), L"PD Loader", MB_ICONERROR);
							}
						}
						else
						{
							auto procedure = (void(*)())GetProcAddress(h, "InitializeDVA");

							if (procedure != NULL)
							{
								procedure();
							}
						}
					}
				}
			}
		} while (FindNextFileW(dvaFile, fd));
		FindClose(dvaFile);
	}
}

void LoadPlugins()
{
	auto oldDir = GetCurrentDirectoryW(); // store the current directory

	auto szSelfPath = GetModuleFileNameW(hm).substr(0, GetModuleFileNameW(hm).find_last_of(L"/\\") + 1);
	SetCurrentDirectoryW(szSelfPath.c_str());

	auto nWantsToLoadPlugins = GetPrivateProfileIntW(L"global", L"enable", TRUE, iniPaths);

	if (nWantsToLoadPlugins)
	{
		WIN32_FIND_DATAW fd;

		SetCurrentDirectoryW(szSelfPath.c_str());

		if (SetCurrentDirectoryW(L"plugins\\"))
			FindFiles(&fd);
	}

	SetCurrentDirectoryW(oldDir.c_str()); // Reset the current directory
}

static LONG LoadedPluginsYet = 0;
void LoadEverything()
{
	if (_InterlockedCompareExchange(&LoadedPluginsYet, 1, 0) != 0) return;

	LoadOriginalLibrary();
	LoadPlugins();
}

static LONG RestoredOnce = 0;
void LoadPluginsAndRestoreIAT(uintptr_t retaddr)
{
	bool calledFromBind = false;

	//steam drm check
	GetSections([&](PIMAGE_SECTION_HEADER pSection, size_t dwLoadOffset, DWORD dwPhysSize) {
		auto dwStart = static_cast<uintptr_t>(dwLoadOffset + pSection->VirtualAddress);
		auto dwEnd = dwStart + dwPhysSize;
		if (retaddr >= dwStart && retaddr <= dwEnd)
			calledFromBind = true;
	}, ".bind");

	if (calledFromBind) return;

	if (_InterlockedCompareExchange(&RestoredOnce, 1, 0) != 0) return;

	LoadEverything();

	for (size_t i = 0; i < Kernel32ExportsNamesCount; i++)
	{
		if (Kernel32Data[i][IATPtr] && Kernel32Data[i][ProcAddress])
		{
			auto ptr = (size_t*)Kernel32Data[i][IATPtr];
			DWORD dwProtect[2];
			VirtualProtect(ptr, sizeof(size_t), PAGE_EXECUTE_READWRITE, &dwProtect[0]);
			*ptr = Kernel32Data[i][ProcAddress];
			VirtualProtect(ptr, sizeof(size_t), dwProtect[0], &dwProtect[1]);
		}
	}
}

void WINAPI CustomGetStartupInfoA(LPSTARTUPINFOA lpStartupInfo)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetStartupInfoA(lpStartupInfo);
}

void WINAPI CustomGetStartupInfoW(LPSTARTUPINFOW lpStartupInfo)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetStartupInfoW(lpStartupInfo);
}

HMODULE WINAPI CustomGetModuleHandleA(LPCSTR lpModuleName)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetModuleHandleA(lpModuleName);
}

HMODULE WINAPI CustomGetModuleHandleW(LPCWSTR lpModuleName)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetModuleHandleW(lpModuleName);
}

FARPROC WINAPI CustomGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetProcAddress(hModule, lpProcName);
}

DWORD WINAPI CustomGetShortPathNameA(LPCSTR lpszLongPath, LPSTR lpszShortPath, DWORD cchBuffer)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetShortPathNameA(lpszLongPath, lpszShortPath, cchBuffer);
}

BOOL WINAPI CustomFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return FindNextFileA(hFindFile, lpFindFileData);
}

BOOL WINAPI CustomFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return FindNextFileW(hFindFile, lpFindFileData);
}

HMODULE WINAPI CustomLoadLibraryA(LPCSTR lpLibFileName)
{
	LoadOriginalLibrary();

	return LoadLibraryA(lpLibFileName);
}

HMODULE WINAPI CustomLoadLibraryW(LPCWSTR lpLibFileName)
{
	LoadOriginalLibrary();

	return LoadLibraryW(lpLibFileName);
}

BOOL WINAPI CustomFreeLibrary(HMODULE hLibModule)
{
	if (hLibModule != hm)
		return FreeLibrary(hLibModule);
	else
		return !NULL;
}

HANDLE WINAPI CustomCreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
}

HANDLE WINAPI CustomCreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
}

void WINAPI CustomGetSystemInfo(LPSYSTEM_INFO lpSystemInfo)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return GetSystemInfo(lpSystemInfo);
}

LONG WINAPI CustomInterlockedCompareExchange(LONG volatile* Destination, LONG ExChange, LONG Comperand)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return _InterlockedCompareExchange(Destination, ExChange, Comperand);
}

void WINAPI CustomSleep(DWORD dwMilliseconds)
{
	LoadPluginsAndRestoreIAT((uintptr_t)_ReturnAddress());
	return Sleep(dwMilliseconds);
}

bool HookKernel32IAT(HMODULE mod, bool exe)
{
	auto hExecutableInstance = (size_t)mod;
	IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(hExecutableInstance + ((IMAGE_DOS_HEADER*)hExecutableInstance)->e_lfanew);
	IMAGE_IMPORT_DESCRIPTOR* pImports = (IMAGE_IMPORT_DESCRIPTOR*)(hExecutableInstance + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	size_t                      nNumImports = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR) - 1;

	if (exe)
	{
		Kernel32Data[eGetStartupInfoA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetStartupInfoA");
		Kernel32Data[eGetStartupInfoW][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetStartupInfoW");
		Kernel32Data[eGetModuleHandleA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetModuleHandleA");
		Kernel32Data[eGetModuleHandleW][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetModuleHandleW");
		Kernel32Data[eGetProcAddress][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetProcAddress");
		Kernel32Data[eGetShortPathNameA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetShortPathNameA");
		Kernel32Data[eFindNextFileA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "FindNextFileA");
		Kernel32Data[eFindNextFileW][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "FindNextFileW");
		Kernel32Data[eLoadLibraryA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "LoadLibraryA");
		Kernel32Data[eLoadLibraryW][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "LoadLibraryW");
		Kernel32Data[eFreeLibrary][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "FreeLibrary");
		Kernel32Data[eCreateEventA][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "CreateEventA");
		Kernel32Data[eCreateEventW][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "CreateEventW");
		Kernel32Data[eGetSystemInfo][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "GetSystemInfo");
		Kernel32Data[eInterlockedCompareExchange][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "InterlockedCompareExchange");
		Kernel32Data[eSleep][ProcAddress] = (size_t)GetProcAddress(GetModuleHandle(TEXT("KERNEL32.DLL")), "Sleep");
	}

	uint32_t matchedImports = 0;

	auto PatchIAT = [&](size_t start, size_t end, size_t exe_end)
	{
		for (size_t i = 0; i < nNumImports; i++)
		{
			if (hExecutableInstance + (pImports + i)->FirstThunk > start && !(end && hExecutableInstance + (pImports + i)->FirstThunk > end))
				end = hExecutableInstance + (pImports + i)->FirstThunk;
		}

		if (!end) { end = start + 0x100; }
		if (end > exe_end) //for very broken exes
		{
			start = hExecutableInstance;
			end = exe_end;
		}

		for (auto i = start; i < end; i += sizeof(size_t))
		{
			DWORD dwProtect[2];
			VirtualProtect((size_t*)i, sizeof(size_t), PAGE_EXECUTE_READWRITE, &dwProtect[0]);

			auto ptr = *(size_t*)i;
			if (!ptr)
				continue;

			if (ptr == Kernel32Data[eGetStartupInfoA][ProcAddress])
			{
				if (exe) Kernel32Data[eGetStartupInfoA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetStartupInfoA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetStartupInfoW][ProcAddress])
			{
				if (exe) Kernel32Data[eGetStartupInfoW][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetStartupInfoW;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetModuleHandleA][ProcAddress])
			{
				if (exe) Kernel32Data[eGetModuleHandleA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetModuleHandleA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetModuleHandleW][ProcAddress])
			{
				if (exe) Kernel32Data[eGetModuleHandleW][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetModuleHandleW;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetProcAddress][ProcAddress])
			{
				if (exe) Kernel32Data[eGetProcAddress][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetProcAddress;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetShortPathNameA][ProcAddress])
			{
				if (exe) Kernel32Data[eGetShortPathNameA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetShortPathNameA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eFindNextFileA][ProcAddress])
			{
				if (exe) Kernel32Data[eFindNextFileA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomFindNextFileA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eFindNextFileW][ProcAddress])
			{
				if (exe) Kernel32Data[eFindNextFileW][IATPtr] = i;
				*(size_t*)i = (size_t)CustomFindNextFileW;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eLoadLibraryA][ProcAddress])
			{
				if (exe) Kernel32Data[eLoadLibraryA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomLoadLibraryA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eLoadLibraryW][ProcAddress])
			{
				if (exe) Kernel32Data[eLoadLibraryW][IATPtr] = i;
				*(size_t*)i = (size_t)CustomLoadLibraryW;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eFreeLibrary][ProcAddress])
			{
				if (exe) Kernel32Data[eFreeLibrary][IATPtr] = i;
				*(size_t*)i = (size_t)CustomFreeLibrary;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eCreateEventA][ProcAddress])
			{
				if (exe) Kernel32Data[eCreateEventA][IATPtr] = i;
				*(size_t*)i = (size_t)CustomCreateEventA;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eCreateEventW][ProcAddress])
			{
				if (exe) Kernel32Data[eCreateEventW][IATPtr] = i;
				*(size_t*)i = (size_t)CustomCreateEventW;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eGetSystemInfo][ProcAddress])
			{
				if (exe) Kernel32Data[eGetSystemInfo][IATPtr] = i;
				*(size_t*)i = (size_t)CustomGetSystemInfo;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eInterlockedCompareExchange][ProcAddress])
			{
				if (exe) Kernel32Data[eInterlockedCompareExchange][IATPtr] = i;
				*(size_t*)i = (size_t)CustomInterlockedCompareExchange;
				matchedImports++;
			}
			else if (ptr == Kernel32Data[eSleep][ProcAddress])
			{
				if (exe) Kernel32Data[eSleep][IATPtr] = i;
				*(size_t*)i = (size_t)CustomSleep;
				matchedImports++;
			}

			VirtualProtect((size_t*)i, sizeof(size_t), dwProtect[0], &dwProtect[1]);
		}
	};

	static auto getSection = [](const PIMAGE_NT_HEADERS nt_headers, unsigned section) -> PIMAGE_SECTION_HEADER
	{
		return reinterpret_cast<PIMAGE_SECTION_HEADER>(
			(UCHAR*)nt_headers->OptionalHeader.DataDirectory +
			nt_headers->OptionalHeader.NumberOfRvaAndSizes * sizeof(IMAGE_DATA_DIRECTORY) +
			section * sizeof(IMAGE_SECTION_HEADER));
	};

	static auto getSectionEnd = [](IMAGE_NT_HEADERS* ntHeader, size_t inst) -> auto
	{
		auto sec = getSection(ntHeader, ntHeader->FileHeader.NumberOfSections - 1);
		auto secSize = max(sec->SizeOfRawData, sec->Misc.VirtualSize);
		auto end = inst + max(sec->PointerToRawData, sec->VirtualAddress) + secSize;
		return end;
	};

	auto hExecutableInstance_end = getSectionEnd(ntHeader, hExecutableInstance);

	// Find kernel32.dll
	for (size_t i = 0; i < nNumImports; i++)
	{
		if ((size_t)(hExecutableInstance + (pImports + i)->Name) < hExecutableInstance_end)
		{
			if (!_stricmp((const char*)(hExecutableInstance + (pImports + i)->Name), "KERNEL32.DLL"))
				PatchIAT(hExecutableInstance + (pImports + i)->FirstThunk, 0, hExecutableInstance_end);
		}
	}

	// Fixing ordinals
	auto szSelfName = GetSelfName();

	static auto PatchOrdinals = [&szSelfName](size_t hInstance)
	{
		IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(hInstance + ((IMAGE_DOS_HEADER*)hInstance)->e_lfanew);
		IMAGE_IMPORT_DESCRIPTOR* pImports = (IMAGE_IMPORT_DESCRIPTOR*)(hInstance + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		size_t                      nNumImports = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size / sizeof(IMAGE_IMPORT_DESCRIPTOR) - 1;

		for (size_t i = 0; i < nNumImports; i++)
		{
			if ((size_t)(hInstance + (pImports + i)->Name) < getSectionEnd(ntHeader, (size_t)hInstance))
			{
				if (iequals(szSelfName, (to_wstring((const char*)(hInstance + (pImports + i)->Name)))))
				{
					PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)(hInstance + (pImports + i)->OriginalFirstThunk);
					size_t j = 0;
					while (thunk->u1.Function)
					{
						if (thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
						{
							PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)(hInstance + thunk->u1.AddressOfData);
							void** p = (void**)(hInstance + (pImports + i)->FirstThunk);
						}
						++thunk;
					}
				}
			}
		}
	};

	ModuleList dlls;
	dlls.Enumerate(ModuleList::SearchLocation::LocalOnly);
	for (auto& e : dlls.m_moduleList)
	{
		PatchOrdinals((size_t)std::get<HMODULE>(e));
	}
	return matchedImports > 0;
}

LONG WINAPI CustomUnhandledExceptionFilter(LPEXCEPTION_POINTERS ExceptionInfo)
{
	// step 1: write minidump
	wchar_t		modulename[MAX_PATH];
	wchar_t		filename[MAX_PATH];
	wchar_t		timestamp[128];
	__time64_t	time;
	struct tm	ltime;
	HANDLE		hFile;
	HWND		hWnd;

	wchar_t* modulenameptr = NULL;
	if (GetModuleFileNameW(GetModuleHandle(NULL), modulename, _countof(modulename)) != 0)
	{
		modulenameptr = wcsrchr(modulename, '\\');
		*modulenameptr = L'\0';
		modulenameptr += 1;
	}
	else
	{
		*modulenameptr = L'err.err';
	}

	_time64(&time);
	_localtime64_s(&ltime, &time);
	wcsftime(timestamp, _countof(timestamp), L"%Y%m%d%H%M%S", &ltime);
	swprintf_s(filename, L"%s\\%s\\%s.%s.dmp", modulename, L"logs", modulenameptr, timestamp);

	hFile = CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ex;
		memset(&ex, 0, sizeof(ex));
		ex.ThreadId = GetCurrentThreadId();
		ex.ExceptionPointers = ExceptionInfo;
		ex.ClientPointers = TRUE;

		if (FAILED(MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithDataSegs, &ex, NULL, NULL)))
		{
		}

		CloseHandle(hFile);
	}

	// step 2: write log
	// Logs exception into buffer and writes to file
	swprintf_s(filename, L"%s\\%s\\%s.%s.log", modulename, L"logs", modulenameptr, timestamp);
	hFile = CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		auto Log = [ExceptionInfo, hFile](char* buffer, size_t size, bool reg, bool stack, bool trace)
		{
			if (LogException(buffer, size, (LPEXCEPTION_POINTERS)ExceptionInfo, reg, stack, trace))
			{
				DWORD NumberOfBytesWritten = 0;
				WriteFile(hFile, buffer, strlen(buffer), &NumberOfBytesWritten, NULL);
			}
		};

		// Try to make a very descriptive exception, for that we need to malloc a huge buffer...
		if (auto buffer = (char*)malloc(max_logsize_ever))
		{
			Log(buffer, max_logsize_ever, true, true, true);
			free(buffer);
		}
		else
		{
			// Use a static buffer, no need for any allocation
			static const auto size = max_logsize_basic + max_logsize_regs + max_logsize_stackdump;
			static char static_buf[size];
			static_assert(size <= max_static_buffer, "Static buffer is too big");

			Log(buffer = static_buf, sizeof(static_buf), true, true, false);
		}

		CloseHandle(hFile);
	}

	// step 3: exit the application
	ShowCursor(TRUE);
	hWnd = FindWindowW(0, L"");
	SetForegroundWindow(hWnd);

	return EXCEPTION_CONTINUE_SEARCH;
}

void Init()
{
	std::wstring modulePath = GetModuleFileNameW(hm);
	std::wstring moduleName = modulePath.substr(modulePath.find_last_of(L"/\\") + 1);
	moduleName.resize(moduleName.find_last_of(L'.'));
	modulePath.resize(modulePath.find_last_of(L"/\\") + 1);
	iniPaths.emplace_back(modulePath + moduleName + L".ini");
	iniPaths.emplace_back(modulePath + L"plugins\\config.ini");

	std::wstring m = GetModuleFileNameW(NULL);
	m = m.substr(0, m.find_last_of(L"/\\") + 1) + L"logs";

	auto FolderExists = [](LPCWSTR szPath) -> BOOL
	{
		DWORD dwAttrib = GetFileAttributes(szPath);
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	};

	if (FolderExists(m.c_str()))
	{
		SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
		// Now stub out CustomUnhandledExceptionFilter so NO ONE ELSE can set it!
		uint32_t ret = 0x909090C3; //ret
		DWORD protect[2];
		VirtualProtect(&SetUnhandledExceptionFilter, sizeof(ret), PAGE_EXECUTE_READWRITE, &protect[0]);
		memcpy(&SetUnhandledExceptionFilter, &ret, sizeof(ret));
		VirtualProtect(&SetUnhandledExceptionFilter, sizeof(ret), protect[0], &protect[1]);
	}

	LoadEverything();

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		hm = hModule;
		Init();
	}
	return TRUE;
}
