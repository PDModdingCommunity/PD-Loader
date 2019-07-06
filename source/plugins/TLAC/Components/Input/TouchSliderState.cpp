#include "TouchSliderState.h"

namespace TLAC::Components
{
	void TouchSliderState::SetSensor(int index, int value)
	{
		if (index < 0 || index >= SLIDER_SENSORS)
			return;
	
		SensorPressureLevels[index] = value;
		SensorTouched[index].IsTouched = value > 0;
	}

	void TouchSliderState::ResetSensors()
	{
		for (int i = 0; i < SLIDER_SENSORS; i++)
			SetSensor(i, NO_PRESSURE);
	}
}