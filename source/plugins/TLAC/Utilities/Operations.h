#pragma once
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>

namespace TLAC::Utilities
{
	std::vector<std::string> Split(const std::string& str, const std::string& delim);

	void LeftTrim(std::string &s);
	void RightTrim(std::string &s);
	void Trim(std::string &s);
	
	std::string trim(const std::string& str, const std::string& whitespace = " \t");
}
