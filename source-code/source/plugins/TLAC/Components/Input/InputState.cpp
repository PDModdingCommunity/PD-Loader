#include "InputState.h"
#include <windows.h>
#include <stdio.h>

namespace TLAC::Components
{
	void InputState::ClearState()
	{
		memset(this, 0, sizeof(InputState));
	}

	void InputState::HideCursor()
	{
		MouseX = INT32_MIN;
		MouseY = INT32_MIN;
		MouseDeltaX = 0;
		MouseDeltaY = 0;
	}

	void InputState::SetBit(uint32_t bit, bool value, InputBufferType inputType)
	{
		uint8_t* data = GetInputBuffer(inputType);
		
		if (data == nullptr || bit < 0 || bit >= MAX_BUTTON_BIT)
			return;

		int byteIndex = (bit / 8);
		int bitIndex = (bit % 8);

		BYTE mask = (1 << bitIndex);

		data[byteIndex] = value ? (data[byteIndex] | mask) : (data[byteIndex] & ~mask);
	}

	uint8_t* InputState::GetInputBuffer(InputBufferType inputType)
	{
		switch (inputType)
		{
		case InputBufferType_Tapped: 
			return (uint8_t*)&Tapped;

		case InputBufferType_Released:
			return (uint8_t*)&Released;

		case InputBufferType_Down: 
			return (uint8_t*)&Down;

		case InputBufferType_DoubleTapped: 
			return (uint8_t*)&DoubleTapped;

		case InputBufferType_IntervalTapped:
			return (uint8_t*)&IntervalTapped;

		default:
			return nullptr;
		}
	}
}
