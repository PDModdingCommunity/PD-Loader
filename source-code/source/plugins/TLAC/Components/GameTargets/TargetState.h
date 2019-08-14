#pragma once

namespace TLAC::Components
{
	struct TargetState
	{
		// seems to be some kind of linked list..
		// not sure if these are for all active targets or just used in some functions
		TargetState* prev;
		TargetState* next;
		byte padding10[0x4];
		TargetTypes tgtType;
		float tgtRemainingTime;
		byte padding1C[0x434];
		byte ToBeRemoved; // FromList
		byte padding451[0xB];
		TargetHitStates tgtHitState;
		byte padding460[0x48];
	};
}