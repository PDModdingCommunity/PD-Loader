#pragma once
#pragma warning(push)
#pragma warning(disable: 4947)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <string>
#include <fstream>
#include <vector>

#include <msclr\marshal_cppstd.h>

#include "PluginConfigApi.h"

using namespace System;
using namespace System::Windows::Forms;

float BaseScaleSize = 96;
int Col1Width = 110;
int Col2Left = 114;
int Col2Width = 110;
int ConfigBtnLeft = 160;
int ControlSpacing = 6;

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

// declare this early (actual definitions below)
class ConfigOptionBase;
Panel^ MakePanel(int width, int height, std::vector<ConfigOptionBase*> &cfg, ToolTip^ tooltip, bool* hasChanged);


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

ref class PluginConfigHandler
{
public:
	Form^ form;

	PluginConfigHandler(Panel^ optspanel, String^ title)
	{
		form = gcnew Form();

		form->Text = title;

		form->FormBorderStyle = FormBorderStyle::FixedDialog;
		form->StartPosition = FormStartPosition::CenterScreen;
		form->MaximizeBox = false;
		form->MinimizeBox = false;
		form->ShowInTaskbar = false;
		form->ShowIcon = false;
		//form->TopMost = true;

		form->BackColor = Drawing::Color::FromArgb(64, 64, 64);
		form->ForeColor = Drawing::Color::White;

		optspanel->Left = 0;
		optspanel->Top = 0;

		Button^ OkBtn = gcnew Button();
		OkBtn->Text = L"OK";
		OkBtn->Left = 4;
		OkBtn->Top = optspanel->Bottom + 4;
		OkBtn->AutoSize = true;
		OkBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		form->ClientSize = Drawing::Size(optspanel->Width, OkBtn->Bottom + 4);

		form->Controls->Add(optspanel);
		form->Controls->Add(OkBtn);
		form->AcceptButton = OkBtn;

		form->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &PluginConfigHandler::FormClosing);
		OkBtn->Click += gcnew EventHandler(this, &PluginConfigHandler::OkClick);
	}

	System::Void OpenForm(System::Object^ sender, System::EventArgs^ e)
	{
		// ShowDialog seems to prevent disposal by copying the panel...
		// which would be fine if I didn't rely on using the control handles
		// MessageBox::Show(form->Controls[0]->Handle.ToString());

		//Application::OpenForms[0]->Enabled = false;
		form->Owner = Application::OpenForms[0];
		if (form->Visible)
			form->Hide();
		form->Show();
	}

	System::Void FormClosing(System::Object^ sender, FormClosingEventArgs^ e)
	{
		if (e->CloseReason == CloseReason::UserClosing)
		{
			form->Hide();
			//Application::OpenForms[0]->Enabled = true;
			e->Cancel = true;
		}
	}

	System::Void OkClick(System::Object^ sender, System::EventArgs^ e)
	{
		form->Close();
	}
};

ref class ChangeHandler
{
public:
	bool* changebool;

	ChangeHandler(bool* onchangebool)
	{
		changebool = onchangebool;
	}

	System::Void SetChanged(System::Object^ sender, System::EventArgs^ e)
	{
		*changebool = true;
	}
};

ref class CustomFuncHandler
{
public:
	void(*_func)();

	CustomFuncHandler(void(*func)())
	{
		_func = func;
	}

	System::Void RunFunc(System::Object^ sender, System::EventArgs^ e)
	{
		_func();
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
	bool* hasChanged;

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		return 0;
	}

	virtual void SaveOption()
	{
		return;
	}
};


class OptionMetaGroupStart : public ConfigOptionBase
{
public:
	int _height;

	OptionMetaGroupStart(LPCWSTR friendlyName, int height)
	{
		_friendlyName = friendlyName;
		_height = height;
	}
};
class OptionMetaGroupEnd : public ConfigOptionBase
{
public:
	OptionMetaGroupEnd()
	{
	}
};

class OptionMetaSpacer : public ConfigOptionBase
{
public:
	int _height;

	OptionMetaSpacer(int height)
	{
		_height = height;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		return _height;
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
		cb->Left = left + 2;
		cb->Top = top;
		cb->AutoSize = true;
		cb->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		// hack to ensure high contrast
		cb->BackColor = System::Drawing::Color::FromArgb(0, 0, 0, 0);
		
		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		cb->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(cb, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		cb->CheckedChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(cb);
		mainControlHandle = cb->Handle;

		int ControlHeight = cb->Font->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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
		label->Width = Col1Width;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		numberbox->Minimum = _minVal;
		numberbox->Maximum = _maxVal;

		numberbox->Value = (int)GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath); // cast to int because this returns uint and breaks -1
																												// it seems it does read negative values correctly though... but eventually a parser that properly supports negative numbers would be ideal
		numberbox->Left = left + Col2Left;
		numberbox->Top = top;
		numberbox->Width = Col2Width;
		numberbox->AutoSize = true;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		numberbox->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(numberbox, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		numberbox->ValueChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(label);
		panel->Controls->Add(numberbox);
		mainControlHandle = numberbox->Handle;

		int ControlHeight = numberbox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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
		label->Width = Col1Width;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		GetPrivateProfileStringW(_iniSectionName, _iniVarName, _defaultVal, stringBuf, 256, _iniFilePath);
		if (_useUtf8 && wcscmp(_defaultVal, stringBuf) != 0) { // don't convert default value
			WideCharToMultiByte(CP_ACP, 0, stringBuf, -1, utf8Buf, 256, NULL, NULL); // convert back to the original ANSI as read from file
			MultiByteToWideChar(CP_UTF8, 0, utf8Buf, -1, stringBuf, 256); // now use those bytes to convert from UTF8
		}

		textbox->Text = gcnew String(stringBuf);
		textbox->Left = left + Col2Left;
		textbox->Top = top;
		textbox->Width = Col2Width;
		textbox->AutoSize = true;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		textbox->Scale(ScaleWidth, ScaleHeight);
		
		// disable editing for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8) {
			textbox->Enabled = false;
		}

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(textbox, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		textbox->TextChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(label);
		panel->Controls->Add(textbox);
		mainControlHandle = textbox->Handle;

		int ControlHeight = textbox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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
		label->Width = Col1Width;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (LPCWSTR& choice : _valueStrings) {
			combobox->Items->Add(msclr::interop::marshal_as<System::String^>(choice));
		}
		combobox->SelectedIndex = GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath);
		combobox->Left = left + Col2Left;
		combobox->Top = top;
		combobox->Width = Col2Width;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		combobox->DropDownStyle = ComboBoxStyle::DropDownList;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		combobox->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		combobox->SelectedIndexChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;

		int ControlHeight = combobox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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


class DropdownTextOption : public ConfigOptionBase
{
public:
	LPCWSTR _defaultVal;
	std::vector<LPCWSTR> _valueStrings;
	bool _editable;
	bool _useUtf8;

	DropdownTextOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, LPCWSTR defaultVal, std::vector<LPCWSTR> valueStrings, bool editable, bool useUtf8)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueStrings = valueStrings;
		_editable = editable;
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
		label->Width = Col1Width;
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
		combobox->Left = left + Col2Left;
		combobox->Top = top;
		combobox->Width = Col2Width;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		if (_editable)
			combobox->DropDownStyle = ComboBoxStyle::DropDown;
		else
			combobox->DropDownStyle = ComboBoxStyle::DropDownList;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		combobox->Scale(ScaleWidth, ScaleHeight);

		// disable editing for utf8 mode because the ANSI hack used may not work correctly
		if (_useUtf8) {
			label->Enabled = false;
			combobox->Enabled = false;
		}

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		if (_editable)
			combobox->TextChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);
		else
			combobox->SelectedIndexChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;

		int ControlHeight = combobox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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


class DropdownNumberOption : public ConfigOptionBase
{
public:
	int _defaultVal;
	std::vector<int> _valueInts;
	bool _editable;

	DropdownNumberOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, int defaultVal, std::vector<int> valueInts, bool editable)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueInts = valueInts;
		_editable = editable;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		System::String^ tempSysStr;

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = Col1Width;
		label->AutoSize = true;
		label->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		for (int& choice : _valueInts) {
			combobox->Items->Add(Convert::ToInt32(choice).ToString());
		}

		tempSysStr = Convert::ToInt32((int)GetPrivateProfileIntW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath)).ToString();

		combobox->Text = gcnew String(tempSysStr);
		combobox->Left = left + Col2Left;
		combobox->Top = top;
		combobox->Width = Col2Width;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		
		if (_editable)
			combobox->DropDownStyle = ComboBoxStyle::DropDown;
		else
			combobox->DropDownStyle = ComboBoxStyle::DropDownList;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		combobox->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		ComboboxValidation^ validation = gcnew ComboboxValidation(combobox);
		combobox->Leave += gcnew System::EventHandler(validation, &ComboboxValidation::CheckNumberLeave);

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		if (_editable)
			combobox->TextChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);
		else
			combobox->SelectedIndexChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;

		int ControlHeight = combobox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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
	bool _editable;

	ResolutionOption(LPCWSTR iniVarName, LPCWSTR iniVarName2, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, resolution defaultVal, std::vector<resolution> valueResolutions, bool editable)
	{
		_iniVarName = iniVarName;
		_iniVarName2 = iniVarName2;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_valueResolutions = valueResolutions;
		_editable = editable;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Label^ label = gcnew Label();
		ComboBox^ combobox = gcnew ComboBox();

		System::String^ tempSysStr;

		label->Text = gcnew String(_friendlyName);
		label->Left = left;
		label->Top = top + 3;
		label->Width = Col1Width;
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

		combobox->Left = left + Col2Left;
		combobox->Top = top;
		combobox->Width = Col2Width;
		combobox->AutoSize = true;
		combobox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		if (_editable)
			combobox->DropDownStyle = ComboBoxStyle::DropDown;
		else
			combobox->DropDownStyle = ComboBoxStyle::DropDownList;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		label->Scale(ScaleWidth, ScaleHeight);
		combobox->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(label, gcnew String(_description));
		tooltip->SetToolTip(combobox, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		if (_editable)
			combobox->TextChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);
		else
			combobox->SelectedIndexChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		ComboboxValidation^ validation = gcnew ComboboxValidation(combobox);
		combobox->Leave += gcnew System::EventHandler(validation, &ComboboxValidation::CheckResolutionLeave);

		panel->Controls->Add(label);
		panel->Controls->Add(combobox);
		mainControlHandle = combobox->Handle;

		int ControlHeight = combobox->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
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


class ButtonOption : public ConfigOptionBase
{
public:
	void(*_func)();

	ButtonOption(LPCWSTR friendlyName, LPCWSTR description, void(*func)())
	{
		_friendlyName = friendlyName;
		_description = description;
		_func = func;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		Button^ button = gcnew Button();

		button->Text = gcnew String(_friendlyName);
		button->Left = left;
		button->Top = top;
		button->AutoSize = true;
		button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		button->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(button, gcnew String(_description));

		CustomFuncHandler^ funchandler = gcnew CustomFuncHandler(_func);
		button->Click += gcnew System::EventHandler(funchandler, &CustomFuncHandler::RunFunc);

		panel->Controls->Add(button);

		int ControlHeight = button->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
	}
};


class PluginOption : public ConfigOptionBase
{
public:
	bool _defaultVal;
	std::vector<ConfigOptionBase*> _configopts;

	PluginOption(LPCWSTR iniVarName, LPCWSTR iniSectionName, LPCWSTR iniFilePath, LPCWSTR friendlyName, LPCWSTR description, bool defaultVal, std::vector<ConfigOptionBase*> configopts)
	{
		_iniVarName = iniVarName;
		_iniSectionName = iniSectionName;
		_iniFilePath = iniFilePath;
		_friendlyName = friendlyName;
		_description = description;
		_defaultVal = defaultVal;
		_configopts = configopts;
	}

	virtual int AddToPanel(Panel^ panel, unsigned int left, unsigned int top, ToolTip^ tooltip)
	{
		CheckBox^ cb = gcnew CheckBox();
		Button^ button = gcnew Button();

		cb->Text = gcnew String(_friendlyName);
		cb->Checked = GetPrivateProfileBoolW(_iniSectionName, _iniVarName, _defaultVal, _iniFilePath);
		cb->Left = left + 2;
		cb->Top = top + 3;
		cb->AutoSize = true;
		cb->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		// hack to ensure high contrast
		cb->BackColor = System::Drawing::Color::FromArgb(0, 0, 0, 0);

		button->Text = L"Config";
		button->Left = left + ConfigBtnLeft;
		button->Top = top;
		button->AutoSize = true;
		button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;

		Form^ RootForm = panel->FindForm();
		float ScaleWidth = 1.0f;
		float ScaleHeight = 1.0f;
		if (RootForm)
		{
			Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
			ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
			ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
		}
		cb->Scale(ScaleWidth, ScaleHeight);
		button->Scale(ScaleWidth, ScaleHeight);

		tooltip->SetToolTip(cb, gcnew String(_description));

		if (hasChanged == nullptr)
			hasChanged = new bool(false);
		ChangeHandler^ changehandler = gcnew ChangeHandler(hasChanged);
		cb->CheckedChanged += gcnew System::EventHandler(changehandler, &ChangeHandler::SetChanged);

		panel->Controls->Add(cb);
		mainControlHandle = cb->Handle;

		if (_configopts.size() > 0)
		{
			ToolTip^ paneltooltip = gcnew ToolTip();
			Panel^ configPanel = MakePanel((Col2Left + Col2Width + 76), 250, _configopts, paneltooltip, hasChanged);
			configPanel->Scale(ScaleWidth, ScaleHeight);
			PluginConfigHandler^ confighandler = gcnew PluginConfigHandler(configPanel, gcnew String(_friendlyName) + " Options");
			button->Click += gcnew System::EventHandler(confighandler, &PluginConfigHandler::OpenForm);

			panel->Controls->Add(button);
		}

		int ControlHeight = button->Height / ScaleHeight;
		return ControlHeight + ControlSpacing;
	}

	virtual void SaveOption()
	{
		bool boolEnabled = ((CheckBox^)CheckBox::FromHandle(mainControlHandle))->Checked;

		WritePrivateProfileStringW(_iniSectionName, _iniVarName, boolEnabled ? L"1" : L"0", _iniFilePath);

		for (ConfigOptionBase* opt : _configopts)
		{
			opt->SaveOption();
		}
	}
};


Panel^ MakePanel(int width, int height, std::vector<ConfigOptionBase*> &cfg, ToolTip^ tooltip, bool* hasChanged)
{
	Panel^ outpanel = gcnew Panel();
	outpanel->Width = width;
	outpanel->Height = height;
	outpanel->AutoScroll = true;

	Form^ RootForm = outpanel->FindForm();
	float ScaleWidth = 1.0f;
	float ScaleHeight = 1.0f;
	if (RootForm)
	{
		Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
		ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
		ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
	}

	int curX = 12;
	int curY = 3;

	for (int i = 0; i < cfg.size(); i++)
	{
		if (typeid(*cfg[i]).hash_code() == typeid(OptionMetaGroupEnd).hash_code())
			continue;

		if (typeid(*cfg[i]).hash_code() == typeid(OptionMetaGroupStart).hash_code())
		{
			OptionMetaGroupStart* groupData = (OptionMetaGroupStart*)(cfg[i]);
			GroupBox^ groupbox = gcnew GroupBox();
			groupbox->Width = width - (ScaleWidth * 28);
			groupbox->Height = ScaleHeight * (groupData->_height);
			groupbox->Left = ScaleWidth * 4;
			groupbox->Top = ScaleHeight * curY;
			groupbox->Text = gcnew String(groupData->_friendlyName);
			groupbox->ForeColor = Drawing::Color::White;

			// find the end of this group by simple iteration keeping track of indent level
			int level = 1;
			int endidx;
			for (endidx = i + 1; endidx < cfg.size(); endidx++)
			{
				if (typeid(*cfg[endidx]).hash_code() == typeid(OptionMetaGroupStart).hash_code())
					level++;
				if (typeid(*cfg[endidx]).hash_code() == typeid(OptionMetaGroupEnd).hash_code())
					level--;

				if (level == 0)
					break;
			}

			Panel^ groupPanel = MakePanel(groupbox->Width - (ScaleWidth * 12), groupbox->Height - (ScaleHeight * 20), std::vector<ConfigOptionBase*>(&(cfg[i + 1]), &(cfg[endidx])), tooltip, hasChanged);
			groupPanel->Left = ScaleWidth * 2;
			groupPanel->Top = ScaleHeight * 14;

			groupbox->Controls->Add(groupPanel);
			outpanel->Controls->Add(groupbox);

			i = endidx;
			curY += groupData->_height;
		}
		else
		{
			cfg[i]->hasChanged = hasChanged;
			curY += cfg[i]->AddToPanel(outpanel, curX, curY, tooltip);
		}
	}

	return outpanel;
}

#pragma warning(pop)