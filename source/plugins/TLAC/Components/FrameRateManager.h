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

		float commonRefreshRates[5]
		{
			60.0f,
			75.0f,
			120.0f,
			144.0f,
			240.0f,
		};

		float RoundFrameRate(float frameRate);
	};
}
