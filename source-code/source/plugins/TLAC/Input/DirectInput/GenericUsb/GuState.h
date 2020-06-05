#pragma once
#include "../DirectInput.h"
#include "GuButton.h"

namespace TLAC::Input
{
	struct GuJoystick
	{
		FLOAT XAxis, YAxis;

		GuJoystick();
		GuJoystick(float xAxis, float yAxis);
	};

	struct GuDpad
	{
		BOOL IsDown;
		FLOAT Angle;
		GuJoystick Stick;
	};

	struct GuTrigger
	{
		FLOAT Axis;
	};

	struct GuState
	{
		DIJOYSTATE2 DI_JoyState;

		BYTE Buttons[GU_BUTTON_MAX];

		GuDpad Dpad;
		GuJoystick LeftStick;
		GuJoystick RightStick;
		GuTrigger LeftTrigger;
		GuTrigger RightTrigger;
	};
}