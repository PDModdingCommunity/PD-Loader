#include "TargetInspector.h"

namespace TLAC::Components
{
	bool TargetInspector::repressTbl[maxTargetSlots];

	TargetInspector::TargetInspector()
	{
	}

	TargetInspector::~TargetInspector()
	{
	}

	void TargetInspector::Initialize(ComponentsManager*)
	{
		tgtTypePtr = (int*)TGT_TYPE_BASE_ADDRESS;
		tgtHitStatePtr = (int*)TGT_HIT_STATE_BASE_ADDRESS;
		tgtRemainingTimePtr = (float*)TGT_REMAINING_DURATION_BASE_ADDRESS;
	}

	void TargetInspector::Update()
	{
		GetTargetStates();
		UpdateRepressTbl();
	}

	const char* TargetInspector::GetDisplayName()
	{
		return "target_inspector";
	}

	void TargetInspector::GetTargetStates()
	{
		for (int i = 0; i < maxTargetSlots; ++i)
		{
			tgtStates[i].tgtType = *((int*)((char*)tgtTypePtr + (i * offset)));
			tgtStates[i].tgtHitState = *((int*)((char*)tgtHitStatePtr + (i * offset)));
			tgtStates[i].tgtRemainingTime = *((float*)((char*)tgtRemainingTimePtr + (i * offset)));
		}
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
