#pragma once
#include "EmulatorComponent.h"
#include "GameState.h"

namespace TLAC::Components
{
	class ScaleComponent : public EmulatorComponent
	{
	public:
		ScaleComponent();
		~ScaleComponent();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

		const int updatesPerFrame = 39;

		float* uiAspectRatio;
		float* uiWidth;
		float* uiHeight;

		int* fb1Width;
		int* fb1Height;
		int* fb2Width;
		int* fb2Height;

		double* fbAspectRatio;

	private:

	};
}