#include "TouchSliderState.h"

namespace TLAC::Components
{
	void TouchSliderState::SetSensor(int index, int value)
	{
		if (index < 0 || index >= SLIDER_SENSORS)
			return;
	
		uint32_t* ph = SerialState->sliderSerialResponse.sensors[index].pressureHistory;
		ph[3] = ph[2];
		ph[2] = ph[1];
		ph[1] = ph[0];
		ph[0] = value;

		SerialState->sliderSerialResponse.scanMode = 1;
		SerialState->sliderSerialResponse.scanCount = 1;
		SerialState->sliderResponseCnt = 1;
	}

	void TouchSliderState::ResetSensors()
	{
		for (int i = 0; i < SLIDER_SENSORS; i++)
			SetSensor(i, NO_PRESSURE);
	}
}