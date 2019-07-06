#pragma once
#include "JvsButtons.h"
#include "InputBufferType.h"

namespace TLAC::Components
{
	const int MAX_BUTTON_BIT = 0x6F;

	// The button state is larger than the size of a register
	// but only the first 32 bits are used during normal gameplay
	// so this will provide the convenience of still being able to access them through a bit field
	union ButtonState
	{
		JvsButtons Buttons;
		uint32_t State[4];
	};

	// total sizeof() == 0x20E0
	struct InputState
	{
		ButtonState Tapped;
		ButtonState Released;

		ButtonState Down;
		uint32_t Padding_20[4];

		ButtonState DoubleTapped;
		uint32_t Padding_30[4];

		ButtonState IntervalTapped;
		uint32_t Padding_38[12];

		int32_t MouseX;
		int32_t MouseY;
		int32_t MouseDeltaX;
		int32_t MouseDeltaY;

		uint32_t Padding_AC[8];
		uint8_t Padding_D0[3];
		char Key;

		void ClearState();
		void HideCursor();
		void SetBit(uint32_t bit, bool value, InputBufferType inputType);
		uint8_t* GetInputBuffer(InputBufferType inputType);
	};
}
