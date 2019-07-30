#pragma once
#include <windows.h>

namespace TLAC::Input
{
	struct XinputState
	{
		bool KeyStates[0x8F];

		bool IsDown(BYTE keycode);
	};
}
