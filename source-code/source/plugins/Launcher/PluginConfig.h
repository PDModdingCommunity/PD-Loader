#pragma once
#include "ConfigOption.h"
#include "PluginConfigApi.h"

namespace PluginConfig
{
	ConfigOptionBase* GetConfigOption(PluginConfigOption cfg)
	{
		PluginConfigBooleanData* boolData = (PluginConfigBooleanData*)(cfg.data);
		PluginConfigNumericData* numericData = (PluginConfigNumericData*)(cfg.data);
		PluginConfigStringData* stringData = (PluginConfigStringData*)(cfg.data);
		PluginConfigDropdownIndexData* ddIdxData = (PluginConfigDropdownIndexData*)(cfg.data);
		PluginConfigDropdownTextData* ddTextData = (PluginConfigDropdownTextData*)(cfg.data);
		PluginConfigDropdownNumberData* ddNumberData = (PluginConfigDropdownNumberData*)(cfg.data);
		PluginConfigResolutionData* resData = (PluginConfigResolutionData*)(cfg.data);
		PluginConfigGroupData* groupData = (PluginConfigGroupData*)(cfg.data);

		switch (cfg.cfgType)
		{
		case CONFIG_BOOLEAN:
			return new BooleanOption(boolData->iniVarName, boolData->iniSectionName, boolData->iniFilePath, boolData->friendlyName, boolData->description, boolData->defaultVal, boolData->saveAsString);
		case CONFIG_NUMERIC:
			return new NumericOption(numericData->iniVarName, numericData->iniSectionName, numericData->iniFilePath, numericData->friendlyName, numericData->description, numericData->defaultVal, numericData->minVal, numericData->maxVal);
		case CONFIG_STRING:
			return new StringOption(stringData->iniVarName, stringData->iniSectionName, stringData->iniFilePath, stringData->friendlyName, stringData->description, stringData->defaultVal, stringData->useUtf8);
		case CONFIG_DROPDOWN_INDEX:
			return new DropdownOption(ddIdxData->iniVarName, ddIdxData->iniSectionName, ddIdxData->iniFilePath, ddIdxData->friendlyName, ddIdxData->description, ddIdxData->defaultVal, ddIdxData->valueStrings);
		case CONFIG_DROPDOWN_TEXT:
			return new DropdownTextOption(ddTextData->iniVarName, ddTextData->iniSectionName, ddTextData->iniFilePath, ddTextData->friendlyName, ddTextData->description, ddTextData->defaultVal, ddTextData->valueStrings, ddTextData->editable, ddTextData->useUtf8);
		case CONFIG_DROPDOWN_NUMBER:
			return new DropdownNumberOption(ddNumberData->iniVarName, ddNumberData->iniSectionName, ddNumberData->iniFilePath, ddNumberData->friendlyName, ddNumberData->description, ddNumberData->defaultVal, ddNumberData->valueInts, ddNumberData->editable);
		case CONFIG_RESOLUTION:
			return new ResolutionOption(resData->iniVarName, resData->iniVarName2, resData->iniSectionName, resData->iniFilePath, resData->friendlyName, resData->description, resData->defaultVal, resData->valueResolutions);
		case CONFIG_GROUP_START:
			return new OptionMetaGroupStart(groupData->name, groupData->height);
		case CONFIG_GROUP_END:
			return new OptionMetaGroupEnd();
		default:
			return new ConfigOptionBase();
		}
	}

	std::vector<ConfigOptionBase*> GetConfigOptionVec(std::vector<PluginConfigOption> &in)
	{
		std::vector<ConfigOptionBase*> outvec;
		for (PluginConfigOption &opt : in)
		{
			outvec.push_back(GetConfigOption(opt));
		}
		return outvec;
	}

	/*std::vector<PluginConfigOption> testcfg = 
	{
		PluginConfigOption{ CONFIG_GROUP_START, new PluginConfigGroupData{L"Screen Resolution", 400 } },
		PluginConfigOption{ CONFIG_DROPDOWN_INDEX, new PluginConfigDropdownIndexData{ L"display", RESOLUTION_SECTION, CONFIG_FILE, L"Display:", L"Sets the window/screen mode.", 0, std::vector<LPCWSTR>({ L"Windowed", L"Borderless", L"Fullscreen" }) } },
		PluginConfigOption{ CONFIG_RESOLUTION, new PluginConfigResolutionData{ L"width", L"height", RESOLUTION_SECTION, CONFIG_FILE, L"Resolution:", L"Sets the display resolution.", resolution(1280, 720), getScreenResolutionsVec(screenModes) } }
	};*/

	//std::vector<ConfigOptionBase*> testcfgconfigopts = GetConfigOptionVec(testcfg);
};