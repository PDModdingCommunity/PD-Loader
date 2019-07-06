#pragma once
#include "KeyString.h"

namespace TLAC::Input::KeyConfig
{
	struct KeyStringHash
	{
		size_t operator()(const KeyString& key) const;
	};
}
