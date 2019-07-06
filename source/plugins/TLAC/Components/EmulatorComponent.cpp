#include "EmulatorComponent.h"
#include "../Constants.h"

namespace TLAC::Components
{
	EmulatorComponent::EmulatorComponent()
	{
	}

	EmulatorComponent::~EmulatorComponent()
	{
	}

	void EmulatorComponent::SetElapsedTime(float value)
	{
		elapsedTime = value;
	}

	float EmulatorComponent::GetElapsedTime()
	{
		return elapsedTime == 0.0f ? (1000.0f / 60.0f) : elapsedTime;
	}

	float EmulatorComponent::GetFrameRate()
	{
		return 1000.0f / GetElapsedTime();
	}

	float EmulatorComponent::GetGameFrameRate()
	{
		return *(float*)FRAME_RATE_ADDRESS;
	}
}
