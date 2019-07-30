#pragma once
#include <stdint.h>

#define SLIDER_OK 3
#define SLIDER_SECTIONS 4
#define SLIDER_SENSORS 32

#define NO_PRESSURE 0
#define FULL_PRESSURE 180

namespace TLAC::Components
{
	struct TouchSliderState
	{
		uint8_t Padding0000[112];

		int32_t State;

		uint8_t Padding0074[20 + 12];

		int32_t SensorPressureLevels[SLIDER_SENSORS];

		uint8_t Padding0108[52 - 12];

		float SectionPositions[SLIDER_SECTIONS];
		int SectionConnections[SLIDER_SECTIONS];
		uint8_t Padding015C[4];
		bool SectionTouched[SLIDER_SECTIONS];

		uint8_t Padding013C[3128 - 52 - 40];

		struct
		{
			uint8_t Padding00[2];
			bool IsTouched;
			uint8_t Padding[45];
		} SensorTouched[SLIDER_SENSORS];

		void SetSensor(int index, int value);
		void ResetSensors();
	};
}

