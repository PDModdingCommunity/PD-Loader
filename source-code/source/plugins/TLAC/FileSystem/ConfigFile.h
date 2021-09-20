#pragma once

#include "TextFile.h"
#include <unordered_map>

namespace TLAC::FileSystem
{
	class ConfigFile : public TextFile
	{
	public:
		ConfigFile(const std::string &path);
		ConfigFile(const std::string &directory, const std::string &file);

		std::unordered_map<std::string, std::string> ConfigMap;

		bool TryGetValue(const std::string &key, std::string **value);
		int GetIntegerValue(const std::string& key, int defaultval = 0);
		bool GetBooleanValue(const std::string& key);
		float GetFloatValue(const std::string& key);
		std::string GetStringValue(const std::string& key);

	protected:
		virtual void Parse(std::ifstream &fileStream) override;

	private:
		bool IsComment(const std::string &line);
	};
}

