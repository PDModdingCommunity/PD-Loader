#include <stdio.h>
#include "ConfigFile.h"
#include "../Utilities/Operations.h"

namespace TLAC::FileSystem
{
	ConfigFile::ConfigFile(const std::string &path) : TextFile(path)
	{
		return;
	}

	ConfigFile::ConfigFile(const std::string &directory, const std::string &file) : TextFile(directory, file)
	{
		return;
	}

	bool ConfigFile::TryGetValue(const std::string &key, std::string **value)
	{
		auto pair = ConfigMap.find(key);
		bool found = pair != ConfigMap.end();

		*value = found ? new std::string(pair->second) : nullptr;
		return found;
	}

	int ConfigFile::GetIntegerValue(const std::string& key)
	{
		auto pair = ConfigMap.find(key);
		bool found = pair != ConfigMap.end();

		return found ? atoi(pair->second.c_str()) : 0;
	}

	bool ConfigFile::GetBooleanValue(const std::string& key)
	{
		auto pair = ConfigMap.find(key);
		bool found = pair != ConfigMap.end();

		return found ? pair->second == "true" : false;
	}

	float ConfigFile::GetFloatValue(const std::string & key)
	{
		auto pair = ConfigMap.find(key);
		bool found = pair != ConfigMap.end();

		return found ? (float)atof(pair->second.c_str()) : 0.0f;
	}

	void ConfigFile::Parse(std::ifstream &fileStream)
	{
		std::string line;

		// check for BOM
		std::getline(fileStream, line);
		if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
			fileStream.seekg(3);
		else
			fileStream.seekg(0);

		while (std::getline(fileStream, line))
		{
			if (IsComment(line))
				continue;

			auto splitline = Utilities::Split(line, "=");

			for (auto &line : splitline)
				Utilities::Trim(line);

			ConfigMap.insert(std::make_pair(splitline[0], splitline[1]));
		}
	}

	bool ConfigFile::IsComment(const std::string &line)
	{
		return line.size() <= 0 || line[0] == '#' || line[0] == '[' || (line.size() >= 2 && line.rfind("//", 0) == 0);
	}
}
