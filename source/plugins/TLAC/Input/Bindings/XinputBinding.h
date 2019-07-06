#pragma once
#include "IInputBinding.h"
#include <windows.h>
#include <vector>

namespace TLAC::Input
{
	class XinputBinding : public IInputBinding
	{
	public:
		BYTE Keycode;

		XinputBinding(BYTE keycode);
		~XinputBinding();

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
		bool IsDoubleTapped();
	};
}