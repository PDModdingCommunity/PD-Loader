#include "ui.h"
#include "framework.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int showUI() {
	SetProcessDPIAware();

	// trick Optimus into switching to the NVIDIA GPU
	HMODULE nvcudaModule = LoadLibraryW(L"nvcuda.dll");
	// cuInit actually returns a CUresult, but we don't really care about it
	void(WINAPI * cuInit)(unsigned int flags) = (void(WINAPI*)(unsigned int flags))GetProcAddress(nvcudaModule, "cuInit");
	if (cuInit != NULL) cuInit(0);

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew Launcher::ui());
	return 0;
}

int hookedMain(int argc, const char** argv, const char** envp)
{
	for (int i = 0; i < argc; ++i)
	{
		arg = argv[i];
		if (arg == "--launch" || nSkipLauncher)
			return divaMain(argc, argv, envp);
	}
	return showUI();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaMain, hookedMain);
		DetourTransactionCommit();
	}
	return TRUE;
}