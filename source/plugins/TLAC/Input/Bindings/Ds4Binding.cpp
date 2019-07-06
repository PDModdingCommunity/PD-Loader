#include "Ds4Binding.h"

namespace TLAC::Input
{
	#define Ds4InstanceCheckDefault(checkFunc) (DualShock4::InstanceInitialized() ? DualShock4::GetInstance()->checkFunc : false)

	Ds4Binding::Ds4Binding(Ds4Button button) : Button(button)
	{
	}

	Ds4Binding::~Ds4Binding()
	{
	}

	bool Ds4Binding::IsDown()
	{
		return Ds4InstanceCheckDefault(IsDown(Button));
	}

	bool Ds4Binding::IsTapped()
	{
		return Ds4InstanceCheckDefault(IsTapped(Button));
	}

	bool Ds4Binding::IsReleased()
	{
		return Ds4InstanceCheckDefault(IsReleased(Button));
	}
}