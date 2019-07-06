#include "KeyString.h"

namespace TLAC::Input::KeyConfig
{
	KeyString::KeyString(const char* str) : value(str)
	{
	}

	bool KeyString::operator==(const KeyString& rsv) const
	{
		return !_strcmpi(value.c_str(), rsv.value.c_str());
	}
}
