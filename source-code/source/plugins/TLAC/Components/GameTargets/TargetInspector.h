#pragma once
#include "../EmulatorComponent.h"
#include "../Input/InputEmulator.h"
#include "../../Constants.h"
#include "TargetHitStates.h"
#include "TargetState.h"

namespace TLAC::Components
{
	const int maxTargetSlots = 64;

	class TargetInspector : public EmulatorComponent
	{
	public:
		TargetInspector();
		~TargetInspector();

		static bool repressTbl[maxTargetSlots];

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual const char* GetDisplayName() override;

		static bool IsAnyRepress();

		static bool ShouldVibrate;

	private:
		//const uint64_t offset = 0x4A8;
		const float timingThreshold = 0.13f; // PS4 estimate

		TargetState* tgtStates;

		bool IsSlide(int);
		bool IsWithinRange(float);
		bool HasNotBeenHit(int);
		void UpdateRepressTbl();
		void UpdateShouldVibrate();
	};
}