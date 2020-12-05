#pragma once

// kinda rushed and hacky wrapper around simpleini for use by ConfigOption
// (has a similar API to windows GetPrivateProfile/SetPrivateProfile stuff)

#include <SimpleIni.h>
CSimpleIniW ini_reader;
std::wstring loaded_ini_file;

bool LoadIni(const wchar_t* filename, bool use_utf8=false)
{
	if (ini_reader.IsUnicode() == use_utf8 && loaded_ini_file == filename)
	{
		return true;
	}

	ini_reader.Reset();
	ini_reader.SetUnicode(use_utf8);

	if (ini_reader.LoadFile(filename) != 0)
	{
		loaded_ini_file = L"";
		return false;
	}
	else
	{
		loaded_ini_file = filename;
		return true;
	}
}

const wchar_t* GetIniString(const wchar_t* section, const wchar_t* key, const wchar_t* default, const wchar_t* filename, bool use_utf8)
{
	if (!LoadIni(filename, use_utf8))
	{
		return default;
	}

	return ini_reader.GetValue(section, key, default);
}

int GetIniInt(const wchar_t* section, const wchar_t* key, int default, const wchar_t* filename)
{
	if (!LoadIni(filename))
	{
		return default;
	}

	return ini_reader.GetLongValue(section, key, default);
}

bool GetIniBool(const wchar_t* section, const wchar_t* key, bool default, const wchar_t* filename)
{
	if (!LoadIni(filename))
	{
		return default;
	}

	return ini_reader.GetBoolValue(section, key, default);
}

void SetIniString(const wchar_t* section, const wchar_t* key, const wchar_t* value, const wchar_t* filename, bool use_utf8)
{
	if (!LoadIni(filename, use_utf8))
	{
		return;
	}

	ini_reader.SetValue(section, key, value);
	ini_reader.SaveFile(filename);
}

void SetIniInt(const wchar_t* section, const wchar_t* key, int value, const wchar_t* filename)
{
	if (!LoadIni(filename))
	{
		return;
	}

	ini_reader.SetLongValue(section, key, value);
	ini_reader.SaveFile(filename);
}

void SetIniBool(const wchar_t* section, const wchar_t* key, bool value, const wchar_t* filename, bool use_string)
{
	if (!LoadIni(filename))
	{
		return;
	}

	if (use_string)
		ini_reader.SetValue(section, key, value ? L"true" : L"false");
	else
		ini_reader.SetValue(section, key, value ? L"1" : L"0");

	ini_reader.SaveFile(filename);
}