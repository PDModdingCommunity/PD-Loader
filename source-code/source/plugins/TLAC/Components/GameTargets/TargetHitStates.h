#pragma once

namespace TLAC::Components
{
	enum TargetHitStates : int32_t
	{
		COOL,
		FINE,
		SAFE,
		SAD,
		COOL_WRONG, // unsure
		FINE_WRONG, // unsure
		SAFE_WRONG, // unsure
		SAD_WRONG, // unsure
		WORST,
		NONE = 21,
	};
}