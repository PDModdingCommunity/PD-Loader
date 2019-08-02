#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::Windows::Forms;

// Custom function. Works like GetPrivateProfileIntW but returns bool. Can detect a numeric value or string.
bool GetPrivateProfileBoolW(LPCWSTR lpAppName, LPCWSTR lpKeyName, bool default, LPCWSTR lpFileName)
{
	wchar_t buffer[8];
	GetPrivateProfileStringW(lpAppName, lpKeyName, L"", buffer, 8, lpFileName);
	//MessageBoxW(NULL, buffer, NULL, 0);

	for (wchar_t& chr : buffer)
		chr = towlower(chr);

	bool out;
	if ((lstrcmpW(buffer, L"true") == 0) || (lstrcmpW(buffer, L"1") == 0))
		out = true;
	else if ((lstrcmpW(buffer, L"false") == 0) || (lstrcmpW(buffer, L"0") == 0))
		out = false;
	else
		out = default;

	return out;
}

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


ref class ComboboxValidation
{
public:
	ComboBox^ cb;

	ComboboxValidation(ComboBox^ combobox)
	{
		cb = combobox;
	}

	System::Void CheckNumberLeave(System::Object^ sender, System::EventArgs^ e)
	{
		System::String^ text = cb->Text;

		cli::array<Char>^ digitsarray = gcnew cli::array<Char>{ L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
		System::Collections::Generic::List<Char>^ digitslist = gcnew System::Collections::Generic::List<Char>(digitsarray);

		if (text->Length <= 0)
			return;

		while (text->Length > 0 && text[0] != L'-' && !digitslist->Contains(text[0]))
		{
			text = text->Remove(0, 1);
		}

		if (text->Length <= 1) // can discard up to 1 now because first digit is known good
		{
			cb->Text = text;
			return;
		}

		for (int i = 1; i < text->Length; i++)
		{
			if (!digitslist->Contains(text[i]))
			{
				text = text->Remove(i, 1);
				i--; // negate the upcoming ++
			}
		}

		cb->Text = text;
	}

	System::Void CheckResolutionLeave(System::Object^ sender, System::EventArgs^ e)
	{
		System::String^ text = cb->Text;

		cli::array<Char>^ digitsarray = gcnew cli::array<Char>{ L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9' };
		System::Collections::Generic::List<Char>^ digitslist = gcnew System::Collections::Generic::List<Char>(digitsarray);

		int numX = 0;

		if (text->Length <= 0)
		{
			cb->Text = "x";
			return;
		}

		while (text->Length > 0 && text[0] != L'x' && !digitslist->Contains(text[0]))
		{
			text = text->Remove(0, 1);
		}

		if (text[0] == L'x')
		{
			numX++;
		}

		if (text->Length <= 1) // can discard up to 1 now because first digit is known good
		{
			if (numX == 0) // if that digit wasn't an x though, add one
			{
				text += "x";
			}
			cb->Text = text;
			return;
		}

		for (int i = 1; i < text->Length; i++)
		{
			if (!digitslist->Contains(text[i]) && !(numX == 0 && text[i] == L'x' && ++numX))
			{
				text = text->Remove(i, 1);
				i--; // negate the upcoming ++
			}
		}

		if (numX == 0)
		{
			text += "x";
		}

		cb->Text = text;
	}

};


class ConfigOptionBase
{
public:
	LPCWSTR _iniVarName;
	LPCWSTR _iniSectionName;
	LPCWSTR _iniFilePath;

	LPCWSTR _friendlyName;
	LPCWSTR _description;

	System::IntPtr mainControlHandle;

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		return 0;
	}

	virtual void SaveOption()
	{
		return;
	}
};


class BooleanOption : public ConfigOptionBase
{
public:
	bool _defaultVal;
	bool _saveAsString;

	BooleanOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, bool defaultVal, bool saveAsString)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_saveAsString = saveAsString;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		CheckBox^ cb = gcnew CheckBox();

		cb->Text = gcnew String(_friendlyName);
		cb->Checked = GetPrivateProfileBoolW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath);
		cb->Left = left;
		cb->Top = top;
		cb->AutoSize = true;
		cb->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		tooltip->SetToolTip(cb, gcnew String(_description));

		panel->Controls->Add(cb);
		mainControlHandle = cb->Handle;
		return 23;
	}

	virtual void SaveOption()
	{
		bool boolEnabled = ((CheckBox^)CheckBox::FromHandle(mainControlHandle))->Checked;

		if (_saveAsString)
			WritePrivateProfileStringW(_iniSectionName, _iniVarName, boolEnabled ? L"true" : L"false", _iniFilePath);
		else
			WritePrivateProfileStringW(_iniSectionName, _iniVarName, boolEnabled ? L"1" : L"0", _iniFilePath);
	}
};


class NumericOption : public ConfigOptionBase
{
public:
	int _defaultVal;
	int _minVal;
	int _maxVal;

	NumericOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, int defaultVal, int minVal, int maxVal)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_minVal = minVal;
		_maxVal = maxVal;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		NumericUpDown^ numberbox = gcnew NumericUpDown();

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		numberbox->Minimum = _minVal;
		numberbox->Maximum = _maxVal;

		numberbox->Value = (int)GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath); // cast to int because this returns uint and breaks -1
																												// it seems it does read negative values correctly though... but eventually a parser that properly supports negative numbers would be ideal
		numberbox->Left = left + 104;
		numberbox->Top = top;
		numberbox->Width = 90;
		numberbox->AutoSize = true;

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(numberbox, gcnew String(_description));

		panel->Controls->Add(label);
		panel->Controls->Add(numberbox);
		mainControlHandle = numberbox->Handle;
		return 28;
	}

	virtual void SaveOption()
	{
		System::String^ tempSysStr;
		std::wstring tempWStr;

		tempSysStr = Convert::ToInt32(((NumericUpDown^)NumericUpDown::FromHandle(mainControlHandle))->Value).ToString();
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		WritePrivateProfileStringW(_iniSectionName, _iniVarName, tempWStr.c_str(), _iniFilePath);
	}
};


class StringOption : public ConfigOptionBase
{
public:
	LPCWSTR _defaultVal;
	bool _useUtf8;

	StringOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, LPCWSTR defaultVal, bool useUtf8)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_useUtf8 = useUtf8;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		TextBox^ textbox = gcnew TextBox();

		WCHAR stringBuf[256];
		char utf8Buf[256];

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		GetPrivateProfileStringW(_iniSectionName, _iniVarName, _defaultVal, stringBuf, 256, _iniFilePath);
		if (_useUtf8 && wcscmp(_defaultVal, stringBuf) != 0) { // don't convert default value
			WideCharToMultiByte(CP_ACP, 0, stringBuf, -1, utf8Buf, 256, NULL, NULL); // convert back to the original ANSI as read from file
			MultiByteToWideChar(CP_UTF8, 0, utf8Buf, -1, stringBuf, 256); // now use those bytes to convert from UTF8
		}

		textbox->Text = gcnew String(stringBuf);
		textbox->Left = left + 104;
		textbox->Top = top;
		textbox->Width = 90;
		textbox->AutoSize = true;

		// disable editing for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8) {
			textbox->Enabled = false;
		}

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(textbox, gcnew String(_description));

		panel->Controls->Add(label);
		panel->Controls->Add(textbox);
		mainControlHandle = textbox->Handle;
		return 28;
	}

	virtual void SaveOption()
	{
		// disable saving for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8)
			return;

		System::String^ tempSysStr;
		std::wstring tempWStr;
		WCHAR stringBuf[256];
		char utf8Buf[256];

		tempSysStr = ((TextBox^)TextBox::FromHandle(mainControlHandle))->Text;
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		tempWStr.copy(stringBuf, 256, 0);
		stringBuf[tempWStr.length()] = 0;

		if (_useUtf8) {
			WideCharToMultiByte(CP_UTF8, 0, stringBuf, -1, utf8Buf, 256, NULL, NULL); // convert to UTF8
			MultiByteToWideChar(CP_ACP, 0, utf8Buf, -1, stringBuf, 256); // now convert that to wide chars as if it's ANSI so it saves properly after conversion to ANSI by windows
		}

		WritePrivateProfileStringW(_iniSectionName, _iniVarName, stringBuf, _iniFilePath);
	}
};


class DropdownOption : public ConfigOptionBase
{
public:
	int _defaultVal;
	std::vector<LPCWSTR> _valueStrings;

	DropdownOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, int defaultVal, std::vector<LPCWSTR> valueStrings)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueStrings = valueStrings;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (LPCWSTR& choice : _valueStrings) {
			combobox->Items->Add(msclr::interop::marshal_as<System::String^>(choice));
		}
		combobox->SelectedIndex = GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath);
		combobox->Left = left + 104;
		combobox->Top = top;
		combobox->Width = 90;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		combobox->DropDownStyle = ComboBoxStyle::DropDownList;

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;
		return 28;;
	}

	virtual void SaveOption()
	{
		System::String^ tempSysStr;
		std::wstring tempWStr;

		tempSysStr = Convert::ToInt32(((ComboBox^)ComboBox::FromHandle(mainControlHandle))->SelectedIndex).ToString();
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		WritePrivateProfileStringW(_iniSectionName, _iniVarName, tempWStr.c_str(), _iniFilePath);
	}
};


class EditableDropdownOption : public ConfigOptionBase
{
public:
	LPCWSTR _defaultVal;
	std::vector<LPCWSTR> _valueStrings;
	bool _useUtf8;

	EditableDropdownOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, LPCWSTR defaultVal, std::vector<LPCWSTR> valueStrings, bool useUtf8)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueStrings = valueStrings;
		_useUtf8 = useUtf8;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		WCHAR stringBuf[256];
		char utf8Buf[256];

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (LPCWSTR& choice : _valueStrings) {
			combobox->Items->Add(msclr::interop::marshal_as<System::String^>(choice));
		}
		
		GetPrivateProfileStringW(_iniSectionName, _iniVarName, _defaultVal, stringBuf, 256, _iniFilePath);
		if (_useUtf8 && wcscmp(_defaultVal, stringBuf) != 0) { // don't convert default value
			WideCharToMultiByte(CP_ACP, 0, stringBuf, -1, utf8Buf, 256, NULL, NULL); // convert back to the original ANSI as read from file
			MultiByteToWideChar(CP_UTF8, 0, utf8Buf, -1, stringBuf, 256); // now use those bytes to convert from UTF8
		}

		combobox->Text = gcnew String(stringBuf);
		combobox->Left = left + 104;
		combobox->Top = top;
		combobox->Width = 90;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		combobox->DropDownStyle = ComboBoxStyle::DropDown;

		// disable editing for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8) {
			label->Enabled = false;
			combobox->Enabled = false;
		}

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;
		return 28;;
	}

	virtual void SaveOption()
	{
		// disable saving for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8)
			return;

		System::String^ tempSysStr;
		std::wstring tempWStr;
		WCHAR stringBuf[256];
		char utf8Buf[256];

		tempSysStr = ((ComboBox^)ComboBox::FromHandle(mainControlHandle))->Text;
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		tempWStr.copy(stringBuf, 256, 0);
		stringBuf[tempWStr.length()] = 0;

		if (_useUtf8) {
			WideCharToMultiByte(CP_UTF8, 0, stringBuf, -1, utf8Buf, 256, NULL, NULL); // convert to UTF8
			MultiByteToWideChar(CP_ACP, 0, utf8Buf, -1, stringBuf, 256); // now convert that to wide chars as if it's ANSI so it saves properly after conversion to ANSI by windows
		}

		WritePrivateProfileStringW(_iniSectionName, _iniVarName, stringBuf, _iniFilePath);
	}
};


class EditableDropdownNumberOption : public ConfigOptionBase
{
public:
	int _defaultVal;
	std::vector<int> _valueInts;

	EditableDropdownNumberOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, int defaultVal, std::vector<int> valueInts)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueInts = valueInts;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		System::String^ tempSysStr;

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (int& choice : _valueInts) {
			combobox->Items->Add(Convert::ToInt32(choice).ToString());
		}

		tempSysStr = Convert::ToInt32((int)GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath)).ToString();

		combobox->Text = gcnew String(tempSysStr);
		combobox->Left = left + 104;
		combobox->Top = top;
		combobox->Width = 90;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		combobox->DropDownStyle = ComboBoxStyle::DropDown;

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		ComboboxValidation^ validation = gcnew ComboboxValidation(combobox);
		combobox->Leave += gcnew System::EventHandler(validation, &ComboboxValidation::CheckNumberLeave);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;
		return 28;;
	}

	virtual void SaveOption()
	{
		System::String^ tempSysStr;
		std::wstring tempWStr;

		tempSysStr = ((ComboBox^)ComboBox::FromHandle(mainControlHandle))->Text;
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);

		WritePrivateProfileStringW(_iniSectionName, _iniVarName, tempWStr.c_str(), _iniFilePath);
	}
};


class ResolutionOption : public ConfigOptionBase
{
public:
	LPCWSTR _iniVarName2;
	resolution _defaultVal;
	std::vector<resolution> _valueResolutions;

	ResolutionOption(LPCWSTR iniVarName, LPCWSTR iniVarName2, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, resolution defaultVal, std::vector<resolution> valueResolutions)
	{
		_iniVarName = iniVarName;
		_iniVarName2 = iniVarName2;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueResolutions = valueResolutions;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		System::String^ tempSysStr;

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = 100;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (resolution& choice : _valueResolutions) {
			tempSysStr = Convert::ToInt32(choice.width).ToString() + L"x" + Convert::ToInt32(choice.height).ToString();
			combobox->Items->Add(tempSysStr);
		}

		int width = GetPrivateProfileIntW(_iniSectionName, _iniVarName, -1, _iniFilePath);
		int height = GetPrivateProfileIntW(_iniSectionName, _iniVarName2, -1, _iniFilePath);
		if (width == -1 || height == -1) {
			tempSysStr = Convert::ToInt32(_defaultVal.width).ToString() + L"x" + Convert::ToInt32(_defaultVal.height).ToString();
			combobox->Text = tempSysStr;
		}
		else {
			tempSysStr = Convert::ToInt32(width).ToString() + L"x" + Convert::ToInt32(height).ToString();
			combobox->Text = tempSysStr;
		}

		combobox->Left = left + 104;
		combobox->Top = top;
		combobox->Width = 90;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		combobox->DropDownStyle = ComboBoxStyle::DropDown;

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		ComboboxValidation^ validation = gcnew ComboboxValidation(combobox);
		combobox->Leave += gcnew System::EventHandler(validation, &ComboboxValidation::CheckResolutionLeave);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;
		return 28;;
	}

	virtual void SaveOption()
	{
		System::String^ tempSysStr;
		std::wstring tempWStr;
		cli::array<String^>^ resolutionArray;

		tempSysStr = ((ComboBox^)ComboBox::FromHandle(mainControlHandle))->Text;
		resolutionArray = tempSysStr->Split('x');

		tempSysStr = resolutionArray[0];
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		WritePrivateProfileStringW(_iniSectionName, _iniVarName, tempWStr.c_str(), _iniFilePath);

		tempSysStr = resolutionArray[1];
		tempWStr = msclr::interop::marshal_as<std::wstring>(tempSysStr);
		WritePrivateProfileStringW(_iniSectionName, _iniVarName2, tempWStr.c_str(), _iniFilePath);

	}
};