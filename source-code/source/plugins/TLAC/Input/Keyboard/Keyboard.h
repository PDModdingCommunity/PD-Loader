#pragma once
#include "../IInputDevice.h"
#include "KeyboardState.h"
#include "../../Utilities/Stopwatch.h"

using Stopwatch = TLAC::Utilities::Stopwatch;

namespace TLAC::Input
{
	constexpr float DOUBLE_TAP_THRESHOLD = 200.0f;
	constexpr float INTERVAL_TAP_DELAY_THRESHOLD = 500.0f;
	constexpr float INTERVAL_TAP_THRESHOLD = 75.0f;

	class Keyboard : public IInputDevice
	{
	public:
		static Keyboard* GetInstance();
		
		bool PollInput() override;
		bool IsDown(BYTE keycode);
		bool IsUp(BYTE keycode);
		bool IsTapped(BYTE keycode);
		bool IsDoubleTapped(BYTE keycode);
		bool IsReleased(BYTE keycode);
		bool IsIntervalTapped(BYTE keycode);

		bool WasDown(BYTE keycode);
		bool WasUp(BYTE keycode);

	private:
		Keyboard();
		KeyboardState lastState;
		KeyboardState currentState;

		Stopwatch keyIntervalWatch;

		BYTE keyDoubleTapStates[KEYBOARD_KEYS];
		Stopwatch keyDoubleTapWatches[KEYBOARD_KEYS];

		BOOL keyIntervalInitials[KEYBOARD_KEYS];
		BYTE keyIntervalTapStates[KEYBOARD_KEYS];
		FLOAT keyIntervalTapTimes[KEYBOARD_KEYS];

		static Keyboard* instance;
	};
}

