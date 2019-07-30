#pragma once
#include <Windows.h>
#include <string>

namespace TLAC
{
	class framework
	{
	private:
		static std::string *moduleDirectory;

	public:
		static const wchar_t* DivaWindowName;
		static const wchar_t* GlutDefaultName;
		static const wchar_t* freeGlutDefaultName;

		static bool inputDisable;

		static HWND DivaWindowHandle;
		static HMODULE Module;

		static std::string GetModuleDirectory();
		static RECT GetWindowBounds();
	};
}
