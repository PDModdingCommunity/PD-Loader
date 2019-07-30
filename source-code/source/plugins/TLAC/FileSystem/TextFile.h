#pragma once

#include <string>
#include <fstream>

namespace TLAC::FileSystem
{
	class TextFile
	{
	public:
		std::string FileName;

		TextFile(const std::string &path);
		TextFile(const std::string &directory, const std::string &file);
		~TextFile();
	
		bool OpenRead();
	
	protected:
		virtual void Parse(std::ifstream &fileStream) = 0;
	};
}
