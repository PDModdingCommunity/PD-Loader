#pragma once
#include "IInputBinding.h"
#include "../Keyboard/Keyboard.h"

namespace TLAC::Input
{
	class KeyboardBinding : public IInputBinding
	{
	public:
		BYTE Keycode;
		bool SetBypassTransferCheck;

		KeyboardBinding(BYTE keycode, bool bypassTransferCheck = false);
		~KeyboardBinding();

		bool BypassTransferCheck() override;

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}
