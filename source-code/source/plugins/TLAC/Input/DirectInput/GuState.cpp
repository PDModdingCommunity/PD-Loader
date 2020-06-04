#include "GuState.h"

namespace TLAC::Input
{
	GuJoystick::GuJoystick() : XAxis(0.0f), YAxis(0.0f)
	{
		return;
	};

	GuJoystick::GuJoystick(float xAxis, float yAxis) : XAxis(xAxis), YAxis(yAxis) 
	{
		return;
	};
}