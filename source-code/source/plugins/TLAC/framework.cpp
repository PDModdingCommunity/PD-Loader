#include "framework.h"
#include <filesystem>

namespace TLAC
{
	typedef std::filesystem::path fspath;

	std::string *framework::moduleDirectory;

	const wchar_t* framework::DivaWindowName = L"Hatsune Miku Project DIVA Arcade Future Tone";
	const wchar_t* framework::GlutDefaultName = L"GLUT";
	const wchar_t* framework::freeGlutDefaultName = L"FREEGLUT";

	HWND framework::DivaWindowHandle;
	HMODULE framework::Module;

	bool framework::inputDisable = false;

	std::string framework::GetModuleDirectory()
	{
		if (moduleDirectory == nullptr)
		{
			WCHAR modulePathBuffer[MAX_PATH];
			GetModuleFileNameW(framework::Module, modulePathBuffer, MAX_PATH);

			fspath configPath = fspath(modulePathBuffer).parent_path();
			moduleDirectory = new std::string(configPath.u8string());
		}

		return *moduleDirectory;
	}

	RECT framework::GetWindowBounds()
	{
		RECT windowRect;
		GetWindowRect(DivaWindowHandle, &windowRect);

		return windowRect;
	}
}
