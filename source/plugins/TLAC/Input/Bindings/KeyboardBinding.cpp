#include "KeyboardBinding.h"

namespace TLAC::Input
{
	KeyboardBinding::KeyboardBinding(BYTE keycode) : Keycode(keycode)
	{
	}

	KeyboardBinding::~KeyboardBinding()
	{
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
