#pragma once
#include "EmulatorComponent.h"
#include <vector>

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
		float motionSpeedMultiplier = 5.0;
		void InjectCode(void* address, const std::vector<uint8_t> data);
	};
}
