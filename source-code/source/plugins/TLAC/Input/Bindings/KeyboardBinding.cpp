#include "KeyboardBinding.h"

namespace TLAC::Input
{
	KeyboardBinding::KeyboardBinding(BYTE keycode, bool bypassTransferCheck) : Keycode(keycode), SetBypassTransferCheck(bypassTransferCheck)
	{
	}

	KeyboardBinding::~KeyboardBinding()
	{
	}

	bool KeyboardBinding::BypassTransferCheck()
	{
		return SetBypassTransferCheck;
	}
	
	bool KeyboardBinding::IsDown()
	{
		return Keyboard::GetInstance()->IsDown(Keycode);
	}

	bool KeyboardBinding::IsTapped()
	{
		return Keyboard::GetInstance()->IsTapped(Keycode);
	}

	bool KeyboardBinding::IsReleased()
	{
		return Keyboard::GetInstance()->IsReleased(Keycode);
	}
}
