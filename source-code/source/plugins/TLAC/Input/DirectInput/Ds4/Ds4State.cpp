#include "Ds4State.h"

namespace TLAC::Input
{
	Joystick::Joystick() : XAxis(0.0f), YAxis(0.0f)
	{
		return;
	};

	Joystick::Joystick(float xAxis, float yAxis) : XAxis(xAxis), YAxis(yAxis) 
	{
		return;
	};
}