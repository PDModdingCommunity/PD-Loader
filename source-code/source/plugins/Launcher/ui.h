#pragma once
#include "framework.h"
#include <msclr\marshal_cppstd.h>
#include <cstdlib>
#include "TabPadding.h"

namespace Launcher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace msclr::interop;

	/// <summary>
	/// Summary for ui
	/// </summary>
	public ref class ui : public System::Windows::Forms::Form
	{
	public:
		ui(void)
		{
			InitializeComponent();
			this->button_Discord->Cursor = Cursors::Hand;
			this->ClientSize = Drawing::Size(444, 323);
			TabPadding^ tabpad = gcnew TabPadding(tabControl);


			// if components.ini has no section name, add one
			if (!IsLineInFile("[components]", COMPONENTS_FILE))
			{
				PrependFile("[components]\n", COMPONENTS_FILE);
			}

			// if playerdata.ini has no section name, add one
			if (!IsLineInFile("[playerdata]", PLAYERDATA_FILE))
			{
				PrependFile("[playerdata]\n", PLAYERDATA_FILE);
			}


			this->panel_ScreenRes->SuspendLayout();

			// populate options (patches) from array in framework
			// (easier than manually setting everything up)
			int screenresY = 3;
			for (ConfigOptionBase* option : screenResolutionArray)
			{
				screenresY += option->AddToPanel(panel_ScreenRes, 3, screenresY, toolTip1);
			}
			((ComboBox^)ComboBox::FromHandle(DisplayModeDropdown->mainControlHandle))->SelectedIndexChanged += gcnew System::EventHandler(this, &ui::DisplayTypeChangedHandler);
			DisplayTypeChangedHandler(this, gcnew EventArgs); // run handler now
			this->panel_ScreenRes->ResumeLayout(false);
			this->panel_ScreenRes->PerformLayout();


			this->panel_IntRes->SuspendLayout();

			// populate options (patches) from array in framework
			// (easier than manually setting everything up)
			int intresY = 3;
			for (ConfigOptionBase* option : internalResolutionArray)
			{
				intresY += option->AddToPanel(panel_IntRes, 3, intresY, toolTip1);
			}
			((CheckBox^)CheckBox::FromHandle(InternalResolutionCheckbox->mainControlHandle))->CheckedChanged += gcnew System::EventHandler(this, &ui::InternalResEnabledChangedHandler);
			InternalResEnabledChangedHandler(this, gcnew EventArgs); // run handler now
			this->panel_IntRes->ResumeLayout(false);
			this->panel_IntRes->PerformLayout();


			this->panel_Patches->SuspendLayout();

			// populate options (patches) from array in framework
			// (easier than manually setting everything up)
			int optionsY = 3;
			for (ConfigOptionBase* option : optionsArray)
			{
				optionsY += option->AddToPanel(panel_Patches, 3, optionsY, toolTip1);
			}
			this->panel_Patches->ResumeLayout(false);
			this->panel_Patches->PerformLayout();


			this->panel_Playerdata->SuspendLayout();

			// populate playerdata options from array in framework
			// (easier than manually setting everything up)
			int playerdataY = 3;
			for (ConfigOptionBase* option : playerdataArray)
			{
				playerdataY += option->AddToPanel(panel_Playerdata, 3, playerdataY, toolTip1);
			}
			this->panel_Playerdata->ResumeLayout(false);
			this->panel_Playerdata->PerformLayout();


			this->panel_Components->SuspendLayout();

			// populate components from array in framework
			// (easier than manually setting everything up)
			int componentsY = 3;
			for (ConfigOptionBase* component : componentsArray)
			{
				componentsY += component->AddToPanel(panel_Components, 3, componentsY, toolTip1);
			}
			this->panel_Components->ResumeLayout(false);
			this->panel_Components->PerformLayout();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ui()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ button_Launch;
	private: System::Windows::Forms::Button^ button_Exit;
	private: System::Windows::Forms::GroupBox^ groupBox_ScreenRes;
	private: System::Windows::Forms::TabControl^ tabControl;
	private: System::Windows::Forms::TabPage^ tabPage_Resolution;
	private: System::Windows::Forms::GroupBox^ groupBox_InternalRes;
	private: System::Windows::Forms::TabPage^ tabPage_Components;
	private: System::Windows::Forms::Panel^ panel_Components;
	private: System::Windows::Forms::TabPage^ tabPage_Patches;
	private: System::Windows::Forms::Panel^ panel_Patches;
	private: System::Windows::Forms::Button^ button_Discord;
	private: System::Windows::Forms::Button^ button_github;
	private: System::Windows::Forms::TabPage^  tabPage_Playerdata;
	private: System::Windows::Forms::Panel^  panel_Playerdata;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::Panel^  panel_ScreenRes;
	private: System::Windows::Forms::Panel^  panel_IntRes;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ui::typeid));
			this->button_Launch = (gcnew System::Windows::Forms::Button());
			this->button_Exit = (gcnew System::Windows::Forms::Button());
			this->groupBox_ScreenRes = (gcnew System::Windows::Forms::GroupBox());
			this->panel_ScreenRes = (gcnew System::Windows::Forms::Panel());
			this->tabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Resolution = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_InternalRes = (gcnew System::Windows::Forms::GroupBox());
			this->panel_IntRes = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Patches = (gcnew System::Windows::Forms::TabPage());
			this->panel_Patches = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Playerdata = (gcnew System::Windows::Forms::TabPage());
			this->panel_Playerdata = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Components = (gcnew System::Windows::Forms::TabPage());
			this->panel_Components = (gcnew System::Windows::Forms::Panel());
			this->button_Discord = (gcnew System::Windows::Forms::Button());
			this->button_github = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->groupBox_ScreenRes->SuspendLayout();
			this->tabControl->SuspendLayout();
			this->tabPage_Resolution->SuspendLayout();
			this->groupBox_InternalRes->SuspendLayout();
			this->tabPage_Patches->SuspendLayout();
			this->tabPage_Playerdata->SuspendLayout();
			this->tabPage_Components->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_Launch
			// 
			this->button_Launch->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Launch->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Launch->Location = System::Drawing::Point(18, 267);
			this->button_Launch->Name = L"button_Launch";
			this->button_Launch->Size = System::Drawing::Size(75, 23);
			this->button_Launch->TabIndex = 20;
			this->button_Launch->Text = L"Launch";
			this->button_Launch->Click += gcnew System::EventHandler(this, &ui::Button_Launch_Click);
			// 
			// button_Exit
			// 
			this->button_Exit->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_Exit->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Exit->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Exit->Location = System::Drawing::Point(139, 267);
			this->button_Exit->Name = L"button_Exit";
			this->button_Exit->Size = System::Drawing::Size(75, 23);
			this->button_Exit->TabIndex = 21;
			this->button_Exit->Text = L"Exit";
			this->button_Exit->Click += gcnew System::EventHandler(this, &ui::Button_Exit_Click);
			// 
			// groupBox_ScreenRes
			// 
			this->groupBox_ScreenRes->Controls->Add(this->panel_ScreenRes);
			this->groupBox_ScreenRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_ScreenRes->ForeColor = System::Drawing::Color::White;
			this->groupBox_ScreenRes->Location = System::Drawing::Point(8, 6);
			this->groupBox_ScreenRes->Name = L"groupBox_ScreenRes";
			this->groupBox_ScreenRes->Size = System::Drawing::Size(208, 133);
			this->groupBox_ScreenRes->TabIndex = 10;
			this->groupBox_ScreenRes->TabStop = false;
			this->groupBox_ScreenRes->Text = L"Screen Resolution";
			// 
			// panel_ScreenRes
			// 
			this->panel_ScreenRes->Location = System::Drawing::Point(4, 19);
			this->panel_ScreenRes->Name = L"panel_ScreenRes";
			this->panel_ScreenRes->Size = System::Drawing::Size(200, 108);
			this->panel_ScreenRes->TabIndex = 0;
			// 
			// tabControl
			// 
			this->tabControl->Appearance = System::Windows::Forms::TabAppearance::FlatButtons;
			this->tabControl->Controls->Add(this->tabPage_Resolution);
			this->tabControl->Controls->Add(this->tabPage_Patches);
			this->tabControl->Controls->Add(this->tabPage_Playerdata);
			this->tabControl->Controls->Add(this->tabPage_Components);
			this->tabControl->Location = System::Drawing::Point(0, 0);
			this->tabControl->Name = L"tabControl";
			this->tabControl->SelectedIndex = 0;
			this->tabControl->Size = System::Drawing::Size(233, 261);
			this->tabControl->TabIndex = 10;
			// 
			// tabPage_Resolution
			// 
			this->tabPage_Resolution->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Resolution->Controls->Add(this->groupBox_InternalRes);
			this->tabPage_Resolution->Controls->Add(this->groupBox_ScreenRes);
			this->tabPage_Resolution->Location = System::Drawing::Point(4, 25);
			this->tabPage_Resolution->Name = L"tabPage_Resolution";
			this->tabPage_Resolution->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Resolution->Size = System::Drawing::Size(225, 232);
			this->tabPage_Resolution->TabIndex = 0;
			this->tabPage_Resolution->Text = L"Resolution";
			// 
			// groupBox_InternalRes
			// 
			this->groupBox_InternalRes->Controls->Add(this->panel_IntRes);
			this->groupBox_InternalRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_InternalRes->ForeColor = System::Drawing::Color::White;
			this->groupBox_InternalRes->Location = System::Drawing::Point(8, 145);
			this->groupBox_InternalRes->Name = L"groupBox_InternalRes";
			this->groupBox_InternalRes->Size = System::Drawing::Size(208, 83);
			this->groupBox_InternalRes->TabIndex = 20;
			this->groupBox_InternalRes->TabStop = false;
			this->groupBox_InternalRes->Text = L"Internal Resolution";
			// 
			// panel_IntRes
			// 
			this->panel_IntRes->Location = System::Drawing::Point(4, 19);
			this->panel_IntRes->Name = L"panel_IntRes";
			this->panel_IntRes->Size = System::Drawing::Size(200, 58);
			this->panel_IntRes->TabIndex = 1;
			// 
			// tabPage_Patches
			// 
			this->tabPage_Patches->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Patches->Controls->Add(this->panel_Patches);
			this->tabPage_Patches->Location = System::Drawing::Point(4, 25);
			this->tabPage_Patches->Name = L"tabPage_Patches";
			this->tabPage_Patches->Size = System::Drawing::Size(225, 232);
			this->tabPage_Patches->TabIndex = 1;
			this->tabPage_Patches->Text = L"Options";
			// 
			// panel_Patches
			// 
			this->panel_Patches->AutoScroll = true;
			this->panel_Patches->Location = System::Drawing::Point(0, 0);
			this->panel_Patches->Name = L"panel_Patches";
			this->panel_Patches->Size = System::Drawing::Size(225, 232);
			this->panel_Patches->TabIndex = 9;
			// 
			// tabPage_Playerdata
			// 
			this->tabPage_Playerdata->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Playerdata->Controls->Add(this->panel_Playerdata);
			this->tabPage_Playerdata->Location = System::Drawing::Point(4, 25);
			this->tabPage_Playerdata->Name = L"tabPage_Playerdata";
			this->tabPage_Playerdata->Size = System::Drawing::Size(225, 232);
			this->tabPage_Playerdata->TabIndex = 3;
			this->tabPage_Playerdata->Text = L"Player Data";
			// 
			// panel_Playerdata
			// 
			this->panel_Playerdata->AutoScroll = true;
			this->panel_Playerdata->Location = System::Drawing::Point(0, 0);
			this->panel_Playerdata->Name = L"panel_Playerdata";
			this->panel_Playerdata->Size = System::Drawing::Size(225, 232);
			this->panel_Playerdata->TabIndex = 1;
			// 
			// tabPage_Components
			// 
			this->tabPage_Components->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Components->Controls->Add(this->panel_Components);
			this->tabPage_Components->Location = System::Drawing::Point(4, 25);
			this->tabPage_Components->Name = L"tabPage_Components";
			this->tabPage_Components->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Components->Size = System::Drawing::Size(225, 232);
			this->tabPage_Components->TabIndex = 2;
			this->tabPage_Components->Text = L"Components";
			// 
			// panel_Components
			// 
			this->panel_Components->AutoScroll = true;
			this->panel_Components->Location = System::Drawing::Point(0, 0);
			this->panel_Components->Name = L"panel_Components";
			this->panel_Components->Size = System::Drawing::Size(225, 232);
			this->panel_Components->TabIndex = 0;
			// 
			// button_Discord
			// 
			this->button_Discord->BackColor = System::Drawing::Color::Transparent;
			this->button_Discord->FlatAppearance->BorderSize = 0;
			this->button_Discord->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Transparent;
			this->button_Discord->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Transparent;
			this->button_Discord->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Discord->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Discord.Image")));
			this->button_Discord->Location = System::Drawing::Point(239, 12);
			this->button_Discord->Name = L"button_Discord";
			this->button_Discord->Size = System::Drawing::Size(42, 42);
			this->button_Discord->TabIndex = 31;
			this->button_Discord->UseVisualStyleBackColor = false;
			this->button_Discord->Click += gcnew System::EventHandler(this, &ui::button_Discord_Click);
			// 
			// button_github
			// 
			this->button_github->BackColor = System::Drawing::Color::Transparent;
			this->button_github->FlatAppearance->BorderSize = 0;
			this->button_github->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Transparent;
			this->button_github->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Transparent;
			this->button_github->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_github->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_github.Image")));
			this->button_github->Location = System::Drawing::Point(239, 60);
			this->button_github->Name = L"button_github";
			this->button_github->Size = System::Drawing::Size(42, 42);
			this->button_github->TabIndex = 32;
			this->button_github->UseVisualStyleBackColor = false;
			this->button_github->Click += gcnew System::EventHandler(this, &ui::button_github_Click);
			// 
			// ui
			// 
			this->AcceptButton = this->button_Launch;
			this->AutoScaleDimensions = System::Drawing::SizeF(96, 96);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
			this->AutoSize = true;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->CancelButton = this->button_Exit;
			this->ClientSize = System::Drawing::Size(288, 297);
			this->Controls->Add(this->button_github);
			this->Controls->Add(this->button_Discord);
			this->Controls->Add(this->tabControl);
			this->Controls->Add(this->button_Exit);
			this->Controls->Add(this->button_Launch);
			this->DoubleBuffered = true;
			this->ForeColor = System::Drawing::Color::White;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ui";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PD Launcher";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ui::Ui_FormClosing);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ui::Ui_FormClosed);
			this->Load += gcnew System::EventHandler(this, &ui::Ui_Load);
			this->groupBox_ScreenRes->ResumeLayout(false);
			this->tabControl->ResumeLayout(false);
			this->tabPage_Resolution->ResumeLayout(false);
			this->groupBox_InternalRes->ResumeLayout(false);
			this->tabPage_Patches->ResumeLayout(false);
			this->tabPage_Playerdata->ResumeLayout(false);
			this->tabPage_Components->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void SaveSettings() {	
	for (ConfigOptionBase* option : screenResolutionArray)
	{
		option->SaveOption();
	}

	for (ConfigOptionBase* option : internalResolutionArray)
	{
		option->SaveOption();
	}

	for (ConfigOptionBase* option : optionsArray)
	{
		option->SaveOption();
	}

	for (ConfigOptionBase* option : playerdataArray)
	{
		option->SaveOption();
	}

	for (ConfigOptionBase* component : componentsArray)
	{
		component->SaveOption();
	}
}
private: System::Void Ui_Load(System::Object^ sender, System::EventArgs^ e){
}
private: System::Void Button_Exit_Click(System::Object^ sender, System::EventArgs^ e) {
	this->Close();
}
private: System::Void Button_Launch_Click(System::Object^ sender, System::EventArgs^ e) {

	SaveSettings();

	// read the command line here so it'll be up to date even if the user changed it
	WCHAR stringBuf[256];
	GetPrivateProfileStringW(LAUNCHER_SECTION, L"command_line", L"", stringBuf, 256, CONFIG_FILE);

	DIVA_EXECUTABLE_LAUNCH_STRING += L" " + wstring(stringBuf);
	DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING.c_str());


	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcessW(DIVA_EXECUTABLE, DIVA_EXECUTABLE_LAUNCH, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	
	// this->Close won't work in here since it will prompt the user to save the settings
	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
}
private: System::Void InternalResEnabledChangedHandler(System::Object^ sender, System::EventArgs^ e) {
	if (((CheckBox^)CheckBox::FromHandle(InternalResolutionCheckbox->mainControlHandle))->Checked)
	{
		((Control^)Control::FromHandle(InternalResolutionOption->mainControlHandle))->Enabled = true;
	}
	else
	{
		((Control^)Control::FromHandle(InternalResolutionOption->mainControlHandle))->Enabled = false;
	}
}
private: System::Void DisplayTypeChangedHandler(System::Object^ sender, System::EventArgs^ e) {
	int idx = ((ComboBox^)ComboBox::FromHandle(DisplayModeDropdown->mainControlHandle))->SelectedIndex;

	if (idx == 0 || idx == 2) // windowed or fullscreen
	{
		((Control^)Control::FromHandle(DisplayResolutionOption->mainControlHandle))->Enabled = true;
	}
	else
	{
		((Control^)Control::FromHandle(DisplayResolutionOption->mainControlHandle))->Enabled = false;
	}

	if (idx == 2) // fullscreen
	{
		for (ConfigOptionBase* option : screenResolutionArray)
		{
			if (option != DisplayModeDropdown && option != DisplayResolutionOption)
			{
				((Control^)Control::FromHandle(option->mainControlHandle))->Enabled = true;
			}
		}
	}
	else
	{
		for (ConfigOptionBase* option : screenResolutionArray)
		{
			if (option != DisplayModeDropdown && option != DisplayResolutionOption)
			{
				((Control^)Control::FromHandle(option->mainControlHandle))->Enabled = false;
			}
		}
	}
}
private: System::Void Ui_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	switch (MessageBox::Show("Do you want to save your settings?", "PD Launcher", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question))
	{
	case System::Windows::Forms::DialogResult::Yes:
		SaveSettings();
		break;

	case System::Windows::Forms::DialogResult::No:
		break;

	case System::Windows::Forms::DialogResult::Cancel:
		e->Cancel = true;
		break;
	}
}
private: System::Void Ui_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
	// Prevents abnormal termination messages, remember that the game is still technically running and must be killed!
	TerminateProcess(GetCurrentProcess(), EXIT_SUCCESS);
}
private: System::Void button_Discord_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://discord.gg/cvBVGDZ");
}
private: System::Void button_github_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/nastys/PD-Loader");
}

};
}
