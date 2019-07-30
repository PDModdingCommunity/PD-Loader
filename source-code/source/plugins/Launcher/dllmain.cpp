#include "ui.h"
#include "framework.h"
#include <detours.h>
#pragma comment(lib, "detours.lib")

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int showUI() {
	if (Environment::OSVersion->Version->Major >= 6)
		SetProcessDPIAware();

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