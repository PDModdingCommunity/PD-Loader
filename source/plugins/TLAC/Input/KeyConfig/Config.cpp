#include "Config.h"
#include "windows.h"
#include "../Bindings/KeyboardBinding.h"
#include "../Bindings/Ds4Binding.h"
#include "../../Utilities/Operations.h"
#include "../Bindings/XinputBinding.h"
#include "../../Constants.h"

namespace TLAC::Input::KeyConfig
{
	KeycodeMap Config::Keymap =
	{
		// NumPad Keys
		{ "NumPad0", VK_NUMPAD0 },
		{ "NumPad1", VK_NUMPAD1 },
		{ "NumPad2", VK_NUMPAD2 },
		{ "NumPad3", VK_NUMPAD3 },
		{ "NumPad4", VK_NUMPAD4 },
		{ "NumPad5", VK_NUMPAD5 },
		{ "NumPad6", VK_NUMPAD6 },
		{ "NumPad7", VK_NUMPAD7 },
		{ "NumPad8", VK_NUMPAD8 },
		{ "NumPad9", VK_NUMPAD9 },
		{ "Plus", VK_ADD },
		{ "Minus", VK_SUBTRACT },
		{ "Divide", VK_DIVIDE },
		{ "Multiply", VK_MULTIPLY },
		{ "Comma", VK_OEM_COMMA },
		{ "Period", VK_OEM_PERIOD },
		{ "Slash", VK_OEM_2 },
		// F-Keys
		{ "F1", VK_F1 },
		{ "F2", VK_F2 },
		{ "F3", VK_F3 },
		{ "F4", VK_F4 },
		{ "F5", VK_F5 },
		{ "F6", VK_F6 },
		{ "F7", VK_F7 },
		{ "F8", VK_F8 },
		{ "F9", VK_F9 },
		{ "F10", VK_F10 },
		{ "F11", VK_F11 },
		{ "F12", VK_F12 },
		{ "F13", VK_F13 },
		{ "F14", VK_F14 },
		{ "F15", VK_F15 },
		{ "F16", VK_F16 },
		{ "F17", VK_F17 },
		{ "F18", VK_F18 },
		{ "F19", VK_F19 },
		{ "F20", VK_F20 },
		{ "F21", VK_F21 },
		{ "F22", VK_F22 },
		{ "F23", VK_F23 },
		{ "F24", VK_F24 },
		// Shift Keys
		{ "LeftShift", VK_LSHIFT },
		{ "LShift",    VK_LSHIFT },
		{ "RightShift", VK_RSHIFT },
		{ "RShift",     VK_RSHIFT },
		// Control Keys
		{ "LeftControl",  VK_LCONTROL },
		{ "LControl",     VK_LCONTROL },
		{ "LCtrl",        VK_LCONTROL },
		{ "RightControl", VK_RCONTROL },
		{ "RControl",     VK_RCONTROL },
		{ "RCtrl",        VK_RCONTROL },
		// Arrow Keys
		{ "Up",    VK_UP },
		{ "Down",  VK_DOWN },
		{ "Left",  VK_LEFT },
		{ "Right", VK_RIGHT },
		// Special Keys
		{ "Enter",  VK_RETURN },
		{ "Return", VK_RETURN },
		{ "Tab",	VK_TAB },
		{ "Back",      VK_BACK },
		{ "Backspace", VK_BACK },
		{ "Insert", VK_INSERT },
		{ "Ins",    VK_INSERT },
		{ "Delete", VK_DELETE },
		{ "Del",    VK_DELETE },
		{ "Home", VK_HOME },
		{ "End",  VK_END },
		{ "PageUp",   VK_PRIOR },
		{ "PageDown", VK_NEXT },
		{ "ESC",    VK_ESCAPE },
		{ "Escape", VK_ESCAPE },
		{ "Comma",	VK_OEM_COMMA },
		{ "Period", VK_OEM_PERIOD },
		{ "Slash",	VK_OEM_2 },
		// Mouse buttons
		{ "MouseLeft",	VK_LBUTTON },
		{ "MouseMiddle",	VK_MBUTTON },
		{ "MouseRight",	VK_RBUTTON },
		{ "MouseX1",	VK_XBUTTON1 },
		{ "MouseX2",	VK_XBUTTON2 },
	};

	KeycodeMap Config::XinputMap =
	{
		//XINPUT
		{ "XINPUT_A", XINPUT_A},
		{ "XINPUT_B", XINPUT_B},
		{ "XINPUT_X", XINPUT_X},
		{ "XINPUT_Y", XINPUT_Y},
		{ "XINPUT_UP", XINPUT_UP},
		{ "XINPUT_DOWN", XINPUT_DOWN},
		{ "XINPUT_LEFT", XINPUT_LEFT},
		{ "XINPUT_RIGHT", XINPUT_RIGHT},
		{ "XINPUT_START", XINPUT_START},
		{ "XINPUT_BACK", XINPUT_BACK},
		{ "XINPUT_LS", XINPUT_LS},
		{ "XINPUT_RS", XINPUT_RS},
		{ "XINPUT_LT", XINPUT_LT},
		{ "XINPUT_RT", XINPUT_RT},
		{ "XINPUT_LLEFT", XINPUT_LLEFT},
		{ "XINPUT_LRIGHT", XINPUT_LRIGHT},
		{ "XINPUT_RLEFT", XINPUT_RLEFT},
		{ "XINPUT_RRIGHT", XINPUT_RRIGHT},
	};

	Ds4ButtonMap Config::Ds4Map =
	{
		// Face Buttons
		{ "DS4_SQUARE", DS4_SQUARE },
		{ "Ds4_Square", DS4_SQUARE },

		{ "DS4_CROSS", DS4_CROSS },
		{ "Ds4_Cross", DS4_CROSS },

		{ "DS4_CIRCLE", DS4_CIRCLE },
		{ "Ds4_Circle", DS4_CIRCLE },

		{ "DS4_TRIANGLE", DS4_TRIANGLE },
		{ "Ds4_Triangle", DS4_TRIANGLE },

		// Standard Buttons
		{ "DS4_SHARE", DS4_SHARE },
		{ "Ds4_Share", DS4_SHARE },

		{ "DS4_OPTIONS", DS4_OPTIONS },
		{ "Ds4_Options", DS4_OPTIONS },

		{ "DS4_PS", DS4_PS },
		{ "Ds4_PS", DS4_PS },

		{ "DS4_TOUCH", DS4_TOUCH },
		{ "Ds4_Touch", DS4_TOUCH },

		{ "DS4_L1", DS4_L1 },
		{ "Ds4_L1", DS4_L1 },

		{ "DS4_R1", DS4_R1 },
		{ "Ds4_R1", DS4_R1 },

		// D-Pad Directions
		{ "DS4_DPAD_UP", DS4_DPAD_UP },
		{ "Ds4_DPad_Up", DS4_DPAD_UP },

		{ "DS4_DPAD_RIGHT", DS4_DPAD_RIGHT },
		{ "Ds4_DPad_Right", DS4_DPAD_RIGHT },

		{ "DS4_DPAD_DOWN", DS4_DPAD_DOWN },
		{ "Ds4_DPad_Down", DS4_DPAD_DOWN },

		{ "DS4_DPAD_LEFT", DS4_DPAD_LEFT },
		{ "Ds4_DPad_Left", DS4_DPAD_LEFT },

		// Trigger Buttons
		{ "DS4_L_TRIGGER", DS4_L_TRIGGER },
		{ "Ds4_L_Trigger", DS4_L_TRIGGER },

		{ "DS4_R_TRIGGER", DS4_R_TRIGGER },
		{ "Ds4_R_Trigger", DS4_R_TRIGGER },
		
		// Joystick Buttons
		{ "DS4_L3", DS4_L3 },
		{ "Ds4_L3", DS4_L3 },

		{ "DS4_R3", DS4_R3 },
		{ "Ds4_R3", DS4_R3 },

		// Left Joystick
		{ "DS4_L_STICK_UP", DS4_L_STICK_UP },
		{ "Ds4_L_Stick_Up", DS4_L_STICK_UP },

		{ "DS4_L_STICK_RIGHT", DS4_L_STICK_RIGHT },
		{ "Ds4_L_Stick_Right", DS4_L_STICK_RIGHT },

		{ "DS4_L_STICK_DOWN", DS4_L_STICK_DOWN },
		{ "Ds4_L_Stick_Down", DS4_L_STICK_DOWN },

		{ "DS4_L_STICK_LEFT", DS4_L_STICK_LEFT },
		{ "Ds4_L_Stick_Left", DS4_L_STICK_LEFT },

		// Right Joystick
		{ "DS4_R_STICK_UP", DS4_R_STICK_UP },
		{ "Ds4_R_Stick_Up", DS4_R_STICK_UP },

		{ "DS4_R_STICK_RIGHT", DS4_R_STICK_RIGHT },
		{ "Ds4_R_Stick_Right", DS4_R_STICK_RIGHT },

		{ "DS4_R_STICK_DOWN", DS4_R_STICK_DOWN },
		{ "Ds4_R_Stick_Down", DS4_R_STICK_DOWN },

		{ "DS4_R_STICK_LEFT", DS4_R_STICK_LEFT },
		{ "Ds4_R_Stick_Left", DS4_R_STICK_LEFT },
	};

	void Config::BindConfigKeys(std::unordered_map<std::string, std::string> &configMap, const char *configKeyName, Binding &bindObj, std::vector<std::string> defaultKeys)
	{
		std::vector<std::string> keys;

		auto configPair = configMap.find(configKeyName);

		// config variable was found in the ini
		if (configPair != configMap.end())
		{
			keys = Utilities::Split(configPair->second, ",");
		}
		else
		{
			keys = defaultKeys;
		}

		for (std::string key : keys)
		{
			Utilities::Trim(key);

			// Applies only for Single-Character keys
			if (key.length() == 1)
			{
				bindObj.AddBinding(new KeyboardBinding(key[0]));
			}
			else // for special key names
			{
				auto keycode = Config::Keymap.find(key.c_str());

				// name is known in the special keys map
				if (keycode != Config::Keymap.end())
				{
					bindObj.AddBinding(new KeyboardBinding(keycode->second));
				}
				else
				{
					auto xinputBtn = Config::XinputMap.find(key.c_str());

					if (xinputBtn != Config::XinputMap.end())
					{
						bindObj.AddBinding(new XinputBinding(xinputBtn->second));
					}
					else 
					{
						// just gonna be lazy for now and put this inside an else statement
						auto ds4Button = Config::Ds4Map.find(key.c_str());

						if (ds4Button != Config::Ds4Map.end())
						{
							bindObj.AddBinding(new Ds4Binding(ds4Button->second));
						}
						else
						{
							printf("[TLAC] Config::BindConfigKeys(): Unable to parse key: '%s'\n", key.c_str());
						}
					}
				}
			}
		}
	}
}