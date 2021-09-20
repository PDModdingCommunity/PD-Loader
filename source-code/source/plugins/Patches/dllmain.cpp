#include "windows.h"
#include "vector"
#include <tchar.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include "PluginConfigApi.h"
#include "PatchApplier.h"
#include "PatchApplier600.h"
#include "PatchApplier710.h"

#include <detours.h>
#include "framework.h"
#pragma comment(lib, "detours.lib")

unsigned short game_version = 710;

void InjectCode(void* address, const std::vector<uint8_t> data);
void ApplyPatches();
void ApplyCustomPatches(std::wstring CPATCH_FILE_STRING);



HMODULE *hModulePtr;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hModulePtr = &hModule;
		if (*(char*)0x140A570F0 == '6') game_version = 600;
		ApplyPatches();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void ApplyPatches() {
	if (game_version == 710)
	{
		const char* exe_ver_string = (const char*)0x140A92CB8;
		if (strcmp(exe_ver_string, "7.10.00") != 0)
		{
			MessageBoxW(nullptr, L"Game version not 7.10. Please verify your files.", L"Patches", MB_ICONERROR);
			exit(1);
		}
	}

	std::string version_string = std::to_string(game_version);
	version_string.insert(version_string.begin()+1, '.');
	std::cout << "[Patches] Game version " + version_string << std::endl;

	PatchApplier* pa;

	switch (game_version)
	{
	case 600:
		pa = new PatchApplier600;
		break;
	default:
		pa = new PatchApplier710;
	}

	pa->ApplyPatches();
	printf("[Patches] Patches applied\n");

	if (nCustomPatches)
	{
		std::cout << "[Patches] Reading custom patches...\n";
		try {
			for (std::filesystem::path p : std::filesystem::directory_iterator("../patches"))
			{
				std::string extension = std::filesystem::path(p).extension().string();
				if ((extension == ".p" || extension == ".P" || extension == ".p2" || extension == ".P2") &&
					GetPrivateProfileIntW(L"plugins", std::filesystem::path(p).filename().c_str(), TRUE, CONFIG_FILE))
				{
					std::cout << "[Patches] Reading custom patch file: " << std::filesystem::path(p).filename().string() << std::endl;
					ApplyCustomPatches(std::filesystem::path(p).wstring());
				}
			}
		}
		catch (const std::filesystem::filesystem_error &e) {
			std::cout << "[Patches] File system error " << e.what() << " " << e.path1() << " " << e.path2() << " " << e.code() << std::endl;
		}

		std::cout << "[Patches] All custom patches applied\n";
	}
}



void ApplyCustomPatches(std::wstring CPATCH_FILE_STRING)
{
	LPCWSTR CPATCH_FILE = CPATCH_FILE_STRING.c_str();
	std::ifstream fileStream(CPATCH_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		if (line[0] == '#')
		{
			std::cout << "[Patches]" << line.substr(1, line.size()-1) << std::endl;
			continue;
		}
		if (line == "IGNORE") break;
		if (line.find(':') == std::string::npos || (line[0] == '/' && line[1] == '/')) continue;

		std::vector<std::string> commentHSplit = SplitString(line, "#");
		std::vector<std::string> commentDSSplit = SplitString(commentHSplit[0], "//");
		std::vector<std::string> colonSplit = SplitString(commentDSSplit[0], ":");
		if (colonSplit.size() != 2) continue;
		bool echo = true;
		if (colonSplit[0].at(0) == '@')
		{
			echo = false;
			colonSplit[0].at(0) = ' ';
		}
		long long int address;
		std::istringstream iss(colonSplit[0]);
		iss >> std::setbase(16) >> address;
		if (address == 0) std::cout << "[Patches] Custom patch address wrong: " << std::hex << address << std::endl;

		if (colonSplit[1].at(0) == '!')
		{
			if (echo) std::cout << "[Patches] Patching: " << std::hex << address << ":!";
			std::vector<std::string> fullColonSplit = SplitString(line, ":");
			for (int i = 1; i < fullColonSplit[1].size(); i++)
			{
				unsigned char byte_u = fullColonSplit[1].at(i);
				if(byte_u=='\\' && i<fullColonSplit[1].size())
				{
					switch (fullColonSplit[1].at(i + 1))
					{
					case '0':
						byte_u = '\0';
						i++;
						break;
					case 'n':
						byte_u = '\n';
						i++;
						break;
					case 't':
						byte_u = '\t';
						i++;
						break;
					case 'r':
						byte_u = '\r';
						i++;
						break;
					case 'b':
						byte_u = '\b';
						i++;
						break;
					case 'a':
						byte_u = '\a';
						i++;
						break;
					case 'f':
						byte_u = '\f';
						i++;
						break;
					case 'v':
						byte_u = '\v';
						i++;
						break;
					case '\\':
						byte_u = '\\';
						i++;
						break;
					}
				}
				if (echo) std::cout << byte_u;
				std::vector<uint8_t> patch = { byte_u };
				InjectCode((void*)address, patch);
				address++;
			}
			if (echo) std::cout << std::endl;
		}
		else
		{
			std::vector<std::string> bytes = SplitString(colonSplit[1], " ");
			if (bytes.size() < 1) continue;

			std::string comment_string = "";
			int comment_counter = 0;
			if (commentHSplit.size() > 1)
			{
				bool ignore = 1;
				for (std::string comment : commentHSplit)
				{
					if (ignore)
					{
						ignore = 0;
						continue;
					}
					comment_string = comment_string + comment;
				}
			}

			if (echo) std::cout << "[Patches] Patching: " << std::hex << address << ":";
			for (std::string bytestring : bytes)
			{
				int byte;
				std::istringstream issb(bytestring);
				issb >> std::setbase(16) >> byte;
				unsigned char byte_u = byte;
				if (echo)
				{
					std::cout << std::hex << byte << " ";
					if (comment_counter < comment_string.length())
					{
						std::cout << "(" << comment_string.at(comment_counter) << ") ";
						comment_counter++;
					}
				}
				std::vector<uint8_t> patch = { byte_u };
				InjectCode((void*)address, patch);
				address++;
			}
			if (echo) std::cout << std::endl;
			else if (comment_string.length() > 0)
			{
				std::cout << "[Patches]";;
				if (comment_string.at(0) != ' ') std::cout << ' ';
				std::cout << comment_string << std::endl;
			}
		}
	}

	fileStream.close();
}


using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Patches";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Applies patches/tweaks to the game before it starts.\nThis plugin is required.";
}