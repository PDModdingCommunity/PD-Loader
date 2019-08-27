#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <vector>

// resolution class to store and sort the width and height easily
class resolution
{
public:
	unsigned int width;
	unsigned int height;

	resolution()
	{
		width = 0;
		height = 0;
	}

	resolution(unsigned int width, unsigned int height)
	{
		resolution::width = width;
		resolution::height = height;
	}

	bool operator ==(const resolution &res2)
	{
		return width == res2.width && height == res2.height;
	}

	// in comparisons width takes priority because it's usually displayed first
	bool operator <(const resolution &res2)
	{
		if (width == res2.width)
			return height < res2.height;
		else
			return width < res2.width;
	}
	bool operator >(const resolution &res2)
	{
		if (width == res2.width)
			return height > res2.height;
		else
			return width > res2.width;
	}
};

namespace PluginConfig
{
#pragma pack(push, 1)
	enum ConfigType {
		CONFIG_BOOLEAN,
		CONFIG_NUMERIC,
		CONFIG_STRING,
		CONFIG_DROPDOWN_INDEX,
		CONFIG_DROPDOWN_TEXT,
		CONFIG_DROPDOWN_NUMBER,
		CONFIG_RESOLUTION,
		CONFIG_GROUP_START,
		CONFIG_GROUP_END,
		CONFIG_BUTTON,
		CONFIG_SPACER
	};

	struct PluginConfigBooleanData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		bool defaultVal;
		bool saveAsString;
	};

	struct PluginConfigNumericData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		int defaultVal;
		int minVal;
		int maxVal;
	};

	struct PluginConfigStringData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		LPCWSTR defaultVal;
		bool useUtf8;
	};

	struct PluginConfigDropdownIndexData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		int defaultVal;
		std::vector<LPCWSTR> valueStrings;
	};

	struct PluginConfigDropdownTextData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		LPCWSTR defaultVal;
		std::vector<LPCWSTR> valueStrings;
		bool editable;
		bool useUtf8;
	};

	struct PluginConfigDropdownNumberData {
		LPCWSTR iniVarName;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		int defaultVal;
		std::vector<int> valueInts;
		bool editable;
	};

	struct PluginConfigResolutionData {
		LPCWSTR iniVarName;
		LPCWSTR iniVarName2;
		LPCWSTR iniSectionName;
		LPCWSTR iniFilePath;
		LPCWSTR friendlyName;
		LPCWSTR description;
		resolution defaultVal;
		std::vector<resolution> valueResolutions;
		bool editable;
	};

	struct PluginConfigGroupData
	{
		LPCWSTR name;
		int height;
	};

	struct PluginConfigButtonData {
		LPCWSTR friendlyName;
		LPCWSTR description;
		void(*func)();
	};

	struct PluginConfigSpacerData {
		int height;
	};

	struct PluginConfigOption
	{
		ConfigType cfgType;
		void* data;
	};

	struct PluginConfigArray
	{
		int len;
		PluginConfigOption* options;
	};
#pragma pack(pop)
}