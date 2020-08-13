#include "TouchSliderState.h"

namespace TLAC::Components
{
	void TouchSliderState::SetSensor(int index, int value, SliderSensorSetMode mode)
	{
		if (index < 0 || index >= SLIDER_SENSORS)
			return;

		if (mode == SENSOR_SET_MODE_RAW)
		{
			if (SerialState == nullptr)
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
		else
		{
			SensorPressureLevels[index] = value;
			SensorTouched[index].IsTouched = value > 0;
		}
	}

	void TouchSliderState::ResetSensors(SliderSensorSetMode mode)
	{
		for (int i = 0; i < SLIDER_SENSORS; i++)
			SetSensor(i, NO_PRESSURE, mode);
	}
}