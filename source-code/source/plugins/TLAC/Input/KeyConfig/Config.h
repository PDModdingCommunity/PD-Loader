#pragma once

#include <unordered_map>
#include "KeyString.h"
#include "KeyStringHash.h"
#include "../Bindings/Binding.h"
#include "../DirectInput/Ds4/Ds4Button.h"
#include "../DirectInput/GenericUsb/GuButton.h"

namespace TLAC::Input::KeyConfig
{
	typedef std::unordered_map<KeyString, uint8_t, KeyStringHash> KeycodeMap;
	typedef std::unordered_map<KeyString, Ds4Button, KeyStringHash> Ds4ButtonMap;
	typedef std::unordered_map<KeyString, GuButton, KeyStringHash> GuButtonMap;

	class Config
	{
	public:
		static KeycodeMap Keymap;
		static Ds4ButtonMap Ds4Map;
		static KeycodeMap XinputMap;
		static GuButtonMap GuMap;

		static void BindConfigKeys(std::unordered_map<std::string, std::string> &configMap, const char *configKeyName, Binding &bindObj, std::vector<std::string> defaultKeys);
	};
}
