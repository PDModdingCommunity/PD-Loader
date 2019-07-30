#pragma once
#include "IInputBinding.h"
#include "../Keyboard/Keyboard.h"

namespace TLAC::Input
{
	class KeyboardBinding : public IInputBinding
	{
	public:
		BYTE Keycode;

		KeyboardBinding(BYTE keycode);
		~KeyboardBinding();

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}
