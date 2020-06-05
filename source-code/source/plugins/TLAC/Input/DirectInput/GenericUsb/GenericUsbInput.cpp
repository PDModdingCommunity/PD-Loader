#include "GenericUsbInput.h"
#include "../../../Utilities/Math.h"
#include "../../../Utilities/Operations.h"
#include "../../../framework.h"
#include "../../../FileSystem/ConfigFile.h"
#include <stdio.h>

const std::string GU_CONFIG_FILE_NAME = "genericusbinput.ini";

namespace TLAC::Input
{
	GenericUsbInput* GenericUsbInput::instance;

	GenericUsbInput::GenericUsbInput()
	{
	}

	GenericUsbInput::~GenericUsbInput()
	{
		DI_Dispose();
	}

	bool GenericUsbInput::TryInitializeInstance()
	{
		if (InstanceInitialized())
			return true;

		if (!DirectInputInitialized())
			return false;
		
		GenericUsbInput *genericUsbInput = new GenericUsbInput ();

		bool success = genericUsbInput->Initialize();
		instance = success ? genericUsbInput : nullptr;

		if (!success)
			delete genericUsbInput;

		return success;
	}

	bool GenericUsbInput::Initialize()
	{
		HRESULT result = NULL; 
		
		FileSystem::ConfigFile configFile(framework::GetModuleDirectory(), GU_CONFIG_FILE_NAME);
		configFile.OpenRead();

		customKeyMapping = configFile.ConfigMap;

		std::string stt = "0x";
		std::string vid = configFile.GetStringValue("VID");
		std::string pid = configFile.GetStringValue("PID");
		
		unsigned int gid = std::strtoul((stt + pid + vid).c_str(), 0, 16);

		GUID GUID_GenericUsb = { gid, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } };
		
		result = DI_CreateDevice(GUID_GenericUsb);
		
		if (!FAILED(result))
		{
			if (FAILED(result = DI_SetDataFormat(&c_dfDIJoystick2)))
				return false;

			result = DI_Acquire();

			return true;
		}
		else			
		{
			return false;
		}
	}

	bool GenericUsbInput::PollInput()
	{
		lastState = currentState;

		HRESULT result = NULL;
		result = DI_Poll();
		result = DI_GetDeviceState(sizeof(DIJOYSTATE2), &currentState.DI_JoyState);

		if (result != DI_OK)
			return false;

		UpdateInternalGuState(currentState);

		for (int button = 0; button < GU_BUTTON_MAX; button++)
		{
			bool buttonState = GetButtonState(currentState, (GuButton)button);
			currentState.Buttons[button] = buttonState;

			// If custom button is pressed
			if (buttonState)
			{
				// Check whether this button is a custom mapped button
				auto customMapping = customKeyMapping.find(buttonNames[button]);
				if (customMapping != customKeyMapping.end())
				{
					std::vector<std::string> keys = Utilities::Split(customMapping->second, "+");
					for (std::string key : keys)
					{
						Utilities::Trim(key);

						auto guButton = KeyConfig::Config::GuMap.find(key.c_str());
						currentState.Buttons[guButton->second] = buttonState;
					}
				}
			}
		}

		return true;
	}

	void GenericUsbInput::UpdateInternalGuState(GuState &state)
	{
		if (state.Dpad.IsDown = state.DI_JoyState.rgdwPOV[0] != -1)
		{
			state.Dpad.Angle = (state.DI_JoyState.rgdwPOV[0] / 100.0f);

			auto direction = Utilities::GetDirection(state.Dpad.Angle);
			state.Dpad.Stick = { direction.Y, -direction.X };
		}
		else
		{
			state.Dpad.Angle = 0;
			state.Dpad.Stick = GuJoystick();
		}

		state.LeftStick = NormalizeStick(state.DI_JoyState.lX, state.DI_JoyState.lY);
		state.RightStick = NormalizeStick(state.DI_JoyState.lZ, state.DI_JoyState.lRz);

		state.LeftTrigger = { NormalizeTrigger(state.DI_JoyState.lRx) };
		state.RightTrigger = { NormalizeTrigger(state.DI_JoyState.lRy) };
	}

	bool GenericUsbInput::IsDown(GuButton button)
	{
		return currentState.Buttons[button];
	}

	bool GenericUsbInput::IsUp(GuButton button)
	{
		return !IsDown(button);
	}

	bool GenericUsbInput::IsTapped(GuButton button)
	{
		return IsDown(button) && WasUp(button);
	}

	bool GenericUsbInput::IsReleased(GuButton button)
	{
		return IsUp(button) && WasDown(button);
	}

	bool GenericUsbInput::WasDown(GuButton button)
	{
		return lastState.Buttons[button];
	}

	bool GenericUsbInput::WasUp(GuButton button)
	{
		return !WasDown(button);
	}

	bool GenericUsbInput::MatchesDirection(GuJoystick joystick, GuDirection directionEnum, float threshold)
	{
		switch (directionEnum)
		{
		case TLAC::Input::GU_DIR_UP:
			return joystick.YAxis <= -threshold;

		case TLAC::Input::GU_DIR_RIGHT:
			return joystick.XAxis >= +threshold;

		case TLAC::Input::GU_DIR_DOWN:
			return joystick.YAxis >= +threshold;

		case TLAC::Input::GU_DIR_LEFT:
			return joystick.XAxis <= -threshold;

		default:
			return false;
		}
	}

	bool GenericUsbInput::GetButtonState(GuState &state, GuButton button)
	{
		if (button >= GU_BUTTON1 && button <= GU_BUTTON13)
			return state.DI_JoyState.rgbButtons[button];

		if (button >= GU_DPAD_UP && button <= GU_DPAD_LEFT)
			return state.Dpad.IsDown ? MatchesDirection(state.Dpad.Stick, (GuDirection)(button - GU_DPAD_UP), dpadThreshold) : false;

		if (button >= GU_L_STICK_UP && button <= GU_L_STICK_LEFT)
			return MatchesDirection(state.LeftStick, (GuDirection)(button - GU_L_STICK_UP), joystickThreshold);

		if (button >= GU_R_STICK_UP && button <= GU_R_STICK_LEFT)
			return MatchesDirection(state.RightStick, (GuDirection)(button - GU_R_STICK_UP), joystickThreshold);

		return false;
	}
}
