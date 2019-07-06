#include "KeyStringHash.h"
#include <filesystem>

namespace TLAC::Input::KeyConfig
{
	size_t KeyStringHash::operator()(const KeyString& key) const
	{
		std::string ret = key.value;
		std::transform(ret.begin(), ret.end(), ret.begin(),
			[](unsigned char c) { return std::tolower(c, std::locale()); });
		return std::hash<std::string>()(ret);
	}
}