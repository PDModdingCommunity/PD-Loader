#include "TargetInspector.h"

namespace TLAC::Components
{
	bool TargetInspector::repressTbl[maxTargetSlots];
	bool TargetInspector::ShouldVibrate;

	TargetInspector::TargetInspector()
	{
	}

	TargetInspector::~TargetInspector()
	{
	}

	void TargetInspector::Initialize(ComponentsManager*)
	{
		tgtStates = (TargetState*)TGT_STATES_BASE_ADDRESS;
	}

	void TargetInspector::Update()
	{
		UpdateRepressTbl();
		UpdateShouldVibrate();
	}

	const char* TargetInspector::GetDisplayName()
	{
		return "target_inspector";
	}

	void TargetInspector::UpdateRepressTbl()
	{
		for (int i = 0; i < maxTargetSlots; ++i)
		{
			repressTbl[i] = IsWithinRange(tgtStates[i].tgtRemainingTime)
				&& HasNotBeenHit(tgtStates[i].tgtHitState)
				&& !IsSlide(tgtStates[i].tgtType);
		}
	}

	void TargetInspector::UpdateShouldVibrate()
	{
		for (int i = 0; i < maxTargetSlots; ++i)
		{
			if (IsWithinRange(tgtStates[i].tgtRemainingTime)
				&& (tgtStates[i].tgtType == SLIDE_LONG_L || tgtStates[i].tgtType == SLIDE_LONG_R)
				&& tgtStates[i].tgtHitState == NONE)
				{
					//printf("%d\n", tgtStates[i].tgtHitState);
					ShouldVibrate = true;
					return;
				}
		}
		ShouldVibrate = false;
	}

	bool TargetInspector::IsWithinRange(float time)
	{
		return time < timingThreshold && time > -timingThreshold && time != 0;
	}

	bool TargetInspector::HasNotBeenHit(int hitState)
	{
		return hitState == NONE;
	}

	bool TargetInspector::IsSlide(int type)
	{
		return (type >= SLIDE_L && type <= SLIDE_LONG_R) || type >= SLIDE_L_CH;
	}

	bool TargetInspector::IsAnyRepress()
	{
		for (int i = 0; i < maxTargetSlots; ++i)
		{
			if (repressTbl[i])
				return true;
		}
		return false;
	}
}
