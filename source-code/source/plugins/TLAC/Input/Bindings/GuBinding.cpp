#include "GuBinding.h"

namespace TLAC::Input
{
	#define GuInstanceCheckDefault(checkFunc) (GenericUsbInput::InstanceInitialized() ? GenericUsbInput::GetInstance()->checkFunc : false)

	GuBinding::GuBinding(GuButton button) : Button(button)
	{
	}

	GuBinding::~GuBinding()
	{
	}

	bool GuBinding::IsDown()
	{
		return GuInstanceCheckDefault(IsDown(Button));
	}

	bool GuBinding::IsTapped()
	{
		return GuInstanceCheckDefault(IsTapped(Button));
	}

	bool GuBinding::IsReleased()
	{
		return GuInstanceCheckDefault(IsReleased(Button));
	}
}