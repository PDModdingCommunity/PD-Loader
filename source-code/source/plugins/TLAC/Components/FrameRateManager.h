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
		virtual void UpdateDraw2D() override;

		static float fspeed_error; // compensation value for use in this frame
		static float fspeed_error_next; // save a compensation value to be used in the next frame

		static float fspeedhook_lastresult; // used by the ageage hair patch to be lazy and avoid setting up for a proper call

	private:
		float *pvFrameRate;
		float *frameSpeed;
		float *aetFrameDuration;
		float defaultAetFrameDuration;
		float motionSpeedMultiplier = 5.0;
		void InjectCode(void* address, const std::vector<uint8_t> data);
	};
}
