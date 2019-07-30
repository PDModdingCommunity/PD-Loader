#pragma once
#include "EmulatorComponent.h"
#include <chrono>

using seconds = std::chrono::seconds;

namespace TLAC::Components
{
	class FPSLimiter : public EmulatorComponent
	{
	public:
		FPSLimiter();
		~FPSLimiter();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;

	private:
		int frameCountPerSecond;
	};
}