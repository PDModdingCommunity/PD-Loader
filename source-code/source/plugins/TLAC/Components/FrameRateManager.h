#pragma once
#include "EmulatorComponent.h"

namespace TLAC::Components
{
	class FrameRateManager : public EmulatorComponent
	{
	public:
		FrameRateManager();
		~FrameRateManager();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;

	private:
		float *pvFrameRate;
		float *frameSpeed;
		float *aetFrameDuration;
		float defaultAetFrameDuration;
		float motionSpeedMultiplier = 3.0;
	};
}
