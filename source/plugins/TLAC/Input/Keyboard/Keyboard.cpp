#include "Keyboard.h"

namespace TLAC::Input
{
	Keyboard* Keyboard::instance;

	Keyboard::Keyboard()
	{
	}

	Keyboard* Keyboard::GetInstance()
	{
		if (instance == nullptr)
			instance = new Keyboard();

		return instance;
	}

	bool Keyboard::PollInput()
	{
		lastState = currentState;

		float elapsed = keyIntervalWatch.Restart();

		for (BYTE i = 0; i < KEYBOARD_KEYS; i++)
		{
			// DOWN
			bool isDown = GetAsyncKeyState(i) < 0;
			currentState.KeyStates[i] = isDown;

			// DOUBLE TAPPED
			bool isTapped = IsTapped(i);
			keyDoubleTapStates[i] = isTapped ? keyDoubleTapWatches[i].Restart() <= DOUBLE_TAP_THRESHOLD : false;

			// INTERVAL TAPPED
			keyIntervalTapStates[i] = isTapped;

			if (isTapped)
			{
				keyIntervalTapTimes[i] = 0;
				keyIntervalInitials[i] = true;
			}
			else if (isDown)
			{
				float threshold = keyIntervalInitials[i] ? INTERVAL_TAP_DELAY_THRESHOLD : INTERVAL_TAP_THRESHOLD;

				bool intervalTapped = (keyIntervalTapTimes[i] += elapsed) > threshold;
				keyIntervalTapStates[i] = intervalTapped;

				if (intervalTapped)
				{
					keyIntervalTapTimes[i] = 0;
					keyIntervalInitials[i] = false;
				}
			}
		}

		return true;
	}

	bool Keyboard::IsDown(BYTE keycode)
	{
		return currentState.IsDown(keycode);
	}

	bool Keyboard::IsUp(BYTE keycode)
	{
		return !IsDown(keycode);
	}

	bool Keyboard::IsTapped(BYTE keycode)
	{
		return IsDown(keycode) && WasUp(keycode);
	}

	bool Keyboard::IsDoubleTapped(BYTE keycode)
	{
		return keyDoubleTapStates[keycode];
	}

	bool Keyboard::IsReleased(BYTE keycode)
	{
		return IsUp(keycode) && WasDown(keycode);
	}

	inline bool Keyboard::WasDown(BYTE keycode)
	{
		return lastState.IsDown(keycode);
	}

	inline bool Keyboard::WasUp(BYTE keycode)
	{
		return !WasDown(keycode);
	}

	bool Keyboard::IsIntervalTapped(BYTE keycode)
	{
		return keyIntervalTapStates[keycode];
	}
}
