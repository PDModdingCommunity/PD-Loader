#include <windows.h>
#include "Xinput.h"
#include "../../Constants.h"
#include "../../FileSystem/ConfigFile.h"
#include "../../framework.h"
#include "../../Components/GameTargets/TargetInspector.h"
#include "../../Components/Input/TouchSliderEmulator.h"

namespace TLAC::Input
{
	Xinput* Xinput::instance;

	Xinput::Xinput()
	{
		TLAC::FileSystem::ConfigFile config(TLAC::framework::GetModuleDirectory(), "keyconfig.ini");
		config.OpenRead();
		xinput_num = config.GetIntegerValue("xinput_preferred");
		rumble = config.GetBooleanValue("rumble");
	}

	Xinput* Xinput::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new Xinput();
		}
		return instance;
	}

	void Xinput::SetTapStates(BYTE keycode, float elapsed)
	{
		KeyDoubleTapStates[keycode] = IsTapped(keycode) ? KeyDoubleTapWatches[keycode].Restart() <= DOUBLE_TAP_THRESHOLD : false;

		bool isDown = currentState.KeyStates[keycode];
		bool isTapped = IsTapped(keycode);

		keyIntervalTapStates[keycode] = isTapped;

		if (isTapped)
		{
			keyIntervalTapTimes[keycode] = 0;
			keyIntervalInitials[keycode] = true;
		}
		else if (isDown)
		{
			float threshold = keyIntervalInitials[keycode] ? INTERVAL_TAP_DELAY_THRESHOLD : INTERVAL_TAP_THRESHOLD;

			bool intervalTapped = (keyIntervalTapTimes[keycode] += elapsed) > threshold;
			keyIntervalTapStates[keycode] = intervalTapped;

			if (intervalTapped)
			{
				keyIntervalTapTimes[keycode] = 0;
				keyIntervalInitials[keycode] = false;
			}
		}
	}
	
	bool Xinput::PollInput()
	{
		lastState = currentState;
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		float elapsed = keyIntervalWatch.Restart();

		int xc_pref = xinput_num;
		if (xc_pref < 0 || xc_pref > 3) xc_pref = 0;
		for (int n = 0; n < 4; n++)
		{
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			if (XInputGetState(xc_pref, &state) == ERROR_SUCCESS)
			{
				BYTE i = XINPUT_A;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_DOWN;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_B;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_RIGHT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_X;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_LEFT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_Y;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_UP;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_LS;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_RS;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_LSB;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_RSB;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_START;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_BACK;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_LT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.bLeftTrigger > 230)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				i = XINPUT_RT;
				{
					currentState.KeyStates[i] = false;
					if (state.Gamepad.bRightTrigger > 230)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}


				{
					//float normLX = fmaxf(-1, (float)state.Gamepad.sThumbLX / 32767);
					i = XINPUT_LRIGHT;
					currentState.KeyStates[i] = false;
					if (state.Gamepad.sThumbLX > 10000)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);

					i = XINPUT_LLEFT;
					currentState.KeyStates[i] = false;
					if (state.Gamepad.sThumbLX < -10000)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				{
					//float normLX = fmaxf(-1, (float)state.Gamepad.sThumbRX / 32767);
					i = XINPUT_RRIGHT;
					currentState.KeyStates[i] = false;
					if (state.Gamepad.sThumbRX > 10000)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);

					i = XINPUT_RLEFT;
					currentState.KeyStates[i] = false;
					if (state.Gamepad.sThumbRX < -10000)
						currentState.KeyStates[i] = true;
					SetTapStates(i, elapsed);
				}

				if (rumble && TLAC::Components::TouchSliderEmulator::LatestInstance->isSliderTouched() && TLAC::Components::TargetInspector::ShouldVibrate)
				{
					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					vibration.wLeftMotorSpeed = 8000; // use any value between 0-65535 here
					vibration.wRightMotorSpeed = 4000; // use any value between 0-65535 here
					XInputSetState(xc_pref, &vibration);
				}
				else
				{
					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					vibration.wLeftMotorSpeed = 0; // use any value between 0-65535 here
					vibration.wRightMotorSpeed = 0; // use any value between 0-65535 here
					XInputSetState(xc_pref, &vibration);
					//printf("0");
				}

				break;
			}
			else if (n == 3) {
				ZeroMemory(&state, sizeof(XINPUT_STATE));
				ZeroMemory(&currentState, sizeof(currentState));
			}

			if (xc_pref == 3) xc_pref = 0;
			else xc_pref++;
		}
			return TRUE;
	}


	bool Xinput::IsDown(BYTE keycode)
	{
		return currentState.IsDown(keycode);
	}

	bool Xinput::IsUp(BYTE keycode)
	{
		return !IsDown(keycode);
	}

	bool Xinput::IsTapped(BYTE keycode)
	{
		return IsDown(keycode) && WasUp(keycode);
	}

	bool Xinput::IsDoubleTapped(BYTE keycode)
	{
		return KeyDoubleTapStates[keycode];
	}

	bool Xinput::IsReleased(BYTE keycode)
	{
		return IsUp(keycode) && WasDown(keycode);
	}

	inline bool Xinput::WasDown(BYTE keycode)
	{
		return lastState.IsDown(keycode);
	}

	inline bool Xinput::WasUp(BYTE keycode)
	{
		return !WasDown(keycode);
	}

	bool Xinput::IsIntervalTapped(BYTE keycode)
	{
		return keyIntervalTapStates[keycode];
	}
}