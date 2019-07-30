#pragma once

namespace TLAC::Components
{
	enum HoldState
	{
		HOLD_NONE,
		HOLD_SANKAKU = 64,
		HOLD_MARU = 128,
		HOLD_BATSU = 256,
		HOLD_SHIKAKU = 512,
	};
}