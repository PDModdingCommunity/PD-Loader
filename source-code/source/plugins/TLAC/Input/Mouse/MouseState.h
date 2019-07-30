#pragma once
#include <windows.h>

namespace TLAC::Input
{
	struct MouseState
	{
		POINT Position;
		POINT RelativePosition;
		long MouseWheel;
		bool ScrolledUp;
		bool ScrolledDown;
	};
}

