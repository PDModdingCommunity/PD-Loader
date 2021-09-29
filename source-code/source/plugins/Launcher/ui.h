#pragma once
#include "framework.h"
#include <msclr\marshal_cppstd.h>
#include <cstdlib>
#include "TabPadding.h"
#include <GL\freeglut.h>
#include <GL\GL.h>
#include "SkinnedMessageBox.h"
#include "PluginConfig.h"
#include "GPUModel.h"
#include "WineVer.h"
#include "composition.h"
#include <filesystem>

namespace Launcher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace msclr::interop;

	HWND hWnd;

	/// <summary>
	/// Summary for ui
	/// </summary>
	public ref class ui : public System::Windows::Forms::Form
	{
	public:
		ui(void)
		{
			InitializeComponent();

			hWnd = (HWND)Control::Handle.ToPointer();

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

			// if keyconfig.ini has no section name, add one
			if (!IsLineInFile("[keyconfig]", KEYCONFIG_FILE))
			{
				PrependFile("[keyconfig]\n", KEYCONFIG_FILE);
			}


			this->panel_ScreenRes->SuspendLayout();

			// populate options (patches) from array in framework
			// (easier than manually setting everything up)
			int screenresY = 3;
			for (ConfigOptionBase* option : screenResolutionArray)
			{
				option->hasChanged = ResolutionConfigChanged;
				screenresY += option->AddToPanel(panel_ScreenRes, 12, screenresY, toolTip1);
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
				option->hasChanged = ResolutionConfigChanged;
				intresY += option->AddToPanel(panel_IntRes, 12, intresY, toolTip1);
			}
			((CheckBox^)CheckBox::FromHandle(InternalResolutionCheckbox->mainControlHandle))->CheckedChanged += gcnew System::EventHandler(this, &ui::InternalResEnabledChangedHandler);
			InternalResEnabledChangedHandler(this, gcnew EventArgs); // run handler now
			this->panel_IntRes->ResumeLayout(false);
			this->panel_IntRes->PerformLayout();


			this->panel_Details->SuspendLayout();

			// populate details (patches) from array in framework
			// (easier than manually setting everything up)
			int detailsY = 3;
			for (ConfigOptionBase* option : graphicsArray)
			{
				option->hasChanged = OptionsConfigChanged;
				detailsY += option->AddToPanel(panel_Details, 12, detailsY, toolTip1);
			}
			trackBar_LagCompensation->Value = GetIniInt(GRAPHICS_SECTION, L"lag_compensation", 0, CONFIG_FILE);
			this->panel_Details->ResumeLayout(false);
			this->panel_Details->PerformLayout();


			this->panel_Patches->SuspendLayout();

			// populate options (patches) from array in framework
			// (easier than manually setting everything up)
			int optionsY = 3;
			for (ConfigOptionBase* option : optionsArray)
			{
				option->hasChanged = OptionsConfigChanged;
				optionsY += option->AddToPanel(panel_Patches, 12, optionsY, toolTip1);
			}
			this->panel_Patches->ResumeLayout(false);
			this->panel_Patches->PerformLayout();



			this->panel_Playerdata->SuspendLayout();

			// populate playerdata options from array in framework
			// (easier than manually setting everything up)
			int playerdataY = 3;
			for (ConfigOptionBase* option : playerdataArray)
			{
				option->hasChanged = PlayerdataConfigChanged;
				playerdataY += option->AddToPanel(panel_Playerdata, 12, playerdataY, toolTip1);
			}
			this->panel_Playerdata->ResumeLayout(false);
			this->panel_Playerdata->PerformLayout();


			this->panel_Components->SuspendLayout();

			// populate components from array in framework
			// (easier than manually setting everything up)
			int componentsY = 3;
			for (ConfigOptionBase* component : componentsArray)
			{
				component->hasChanged = ComponentsConfigChanged;
				componentsY += component->AddToPanel(panel_Components, 12, componentsY, toolTip1);
			}
			this->panel_Components->ResumeLayout(false);
			this->panel_Components->PerformLayout();


			AllPlugins = LoadPlugins();
			AllPluginOpts = GetPluginOptions(&AllPlugins);

			this->panel_Plugins->SuspendLayout();

			int pluginsY = 3;
			for (ConfigOptionBase* option : AllPluginOpts)
			{
				option->hasChanged = new bool(false);
				pluginsY += option->AddToPanel(panel_Plugins, 12, pluginsY, toolTip1);
			}
			this->panel_Plugins->ResumeLayout(false);
			this->panel_Plugins->PerformLayout();


			AllCustomPatches = LoadCustom();
			AllCustomOpts = GetPluginOptions(&AllCustomPatches);

			this->panel_Custom->SuspendLayout();

			int customY = 3;
			for (ConfigOptionBase* custom : AllCustomOpts)
			{
				custom->hasChanged = new bool(false);
				customY += custom->AddToPanel(panel_Custom, 12, customY, toolTip1);
			}

			this->panel_Custom->ResumeLayout(false);
			this->panel_Custom->PerformLayout();


			// trick Optimus into switching to the NVIDIA GPU
			if (cuInit != NULL) cuInit(0);

			int argc = 1;
			char* argv[1] = { (char*)"" };


			updateLagCompMsec();

			updateStyle();


			glutInit(&argc, argv);
			int window = glutCreateWindow("glut"); // a context must be created to use glGetString
			glutHideWindow();

			String^ vendor = gcnew String((char*)glGetString(GL_VENDOR));
			vendor = vendor->Replace(" Corporation", ""); // this is useless..  remove it to help ensure the GPU type line fits
			String^ renderer = gcnew String((char*)glGetString(GL_RENDERER));
			String^ version = gcnew String((char*)glGetString(GL_VERSION));
			version = version->Replace(" Profile Context", ""); // we don't need this, either

			String^ gpuModel = gcnew String(GPUModel::getGpuName().c_str());

			String^ wineVersion = gcnew String(WineVer::getWineVer().c_str());

			int driver_version_major = 1;
			int driver_version_minor = 0;
			auto version_split = version->Split(' ');
			if (version_split->Length > 1) // make sure we don't use the OpenGL version instead of the driver version
			{
				String^ driver_version = gcnew String(version_split[version_split->Length - 1]);
				auto driver_version_split_major_minor = driver_version->Split('.');
				if (driver_version_split_major_minor->Length >= 2)
				{
					driver_version_major = int::Parse(driver_version_split_major_minor[0]);
					driver_version_minor = int::Parse(driver_version_split_major_minor[1]);
				}
			}

			glutDestroyWindow(window); // destroy the window so it doesn't remain on screen
			if (glutMainLoopEventDynamic != NULL) glutMainLoopEventDynamic(); // freeglut needs this

			if (wineVersion != "")
			{
				this->labelGPU->Text = "Wine " + wineVersion + " (at least 4.12.1 is required)\n";
			}
			else this->labelGPU->Text = "GPU Info:\n";
			this->labelGPU->Text += vendor + " " + renderer + "\n";
			this->labelGPU->Text += "OpenGL: " + version + "\n";

			int linkStart = this->labelGPU->Text->Length;
			bool showGpuDialog = false;
			if (vendor->Contains("AMD") || vendor->Contains("ATI")) // check OpenGL to get actual GPU being used for vendor check (to get accurate results for iGPU)
			{
				bool hasNovidia = false;
				for (PluginInfo i : AllPlugins)
				{
					if (i.name == L"Novidia")
						hasNovidia = true;
				}

				if (hasNovidia)
				{
					if (wineVersion == "" && (driver_version_major < 21 || (driver_version_major == 21 && driver_version_minor < 6)))
					{
						this->labelGPU->Text += L"Driver has known issues.";
						GPUIssueText = L"The graphics driver you're using has known issues. Please install the latest driver from your GPU vendor's website.";
						this->labelGPU->LinkColor = System::Drawing::Color::Orange;
						showGpuDialog = true;
					}
					else
					{
						this->labelGPU->Text += "Issues: AMD GPU support is unofficial.";
						GPUIssueText = "AMD GPUs are not supported without mods.\nThe PD Loader AMDPack seems to be installed, but please keep in mind that it may have issues.";
						this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
					}
				}
				else
				{
					this->labelGPU->Text += "Issues: Mods needed for AMD compatibility!";
					GPUIssueText = "AMD GPUs are not supported without mods.\nThe PD Loader AMDPack can be used to make 3D rendering work. Please download it and follow the included instructions.\n\nIf you have a laptop with an NVIDIA GPU and wish to use it instead of the detected GPU, you may need to set diva.exe to use it in Windows settings or NVIDIA Control Panel.";
					this->labelGPU->LinkColor = System::Drawing::Color::Red;
					showGpuDialog = true;
				}
			}
			else if (vendor->Contains("NVIDIA"))
			{
				if (wineVersion == "" && (driver_version_major < 391 || (driver_version_major > 446 && driver_version_major < 460)))
				{
					this->labelGPU->Text += L"Driver has known issues.";
					GPUIssueText = L"The graphics driver you're using has known issues. Please install the latest driver from your GPU vendor's website.";
					this->labelGPU->LinkColor = System::Drawing::Color::Orange;
					showGpuDialog = true;
				}
				else if (gpuModel->StartsWith("GA")) // unconfirmed??
				{
					this->labelGPU->Text += L"Issues: Ampere GPU detected! Possible noise.\n(Click for more information)";
					GPUIssueText = L"On Ampere GPUs (RTX 30xx), some important character shaders have issues resulting in lines/noise.\nPlease make sure the ShaderPatch plugin is enabled.";
					this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
				}
				else if (gpuModel->StartsWith("TU") || gpuModel->StartsWith("GV")) // let's assume Volta is like Turing for now
				{
					this->labelGPU->Text += L"Issues: Ampere GPU detected! Possible noise.\n(Click for more information)";
					GPUIssueText = L"On Turing GPUs (GTX 16xx/RTX 20xx), some important character shaders have issues resulting in lines/noise.\nPlease make sure the ShaderPatch plugin is enabled.";
					this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
				}
				else if (gpuModel->StartsWith("GM") || gpuModel->StartsWith("GP"))
				{
					this->labelGPU->Text += L"Issues: May have minor noise on some stages.\n(Click for more information)";
					GPUIssueText = L"On Maxwell GPUs (~GTX 900) and newer, some minor stage shaders create noise.\nPlease make sure the ShaderPatch plugin is enabled.";
					this->labelGPU->LinkColor = System::Drawing::Color::Teal;
				}
				else if (gpuModel->StartsWith("GK"))
				{
					this->labelGPU->Text += L"Issues: None.";
					GPUIssueText = L"Your GPU should have no issues running the game.";
					this->labelGPU->LinkColor = System::Drawing::Color::LightBlue;
				}
				else if (gpuModel->StartsWith("GF"))
				{
					if (version[0] < '4')
					{
						this->labelGPU->Text += L"Issues: Driver too old.";
						GPUIssueText = L"Your GPU should be able to run the game, but it looks like your OpenGL version is too old.\nA driver update should fix this.";
						this->labelGPU->LinkColor = System::Drawing::Color::Orange;
						showGpuDialog = true;
					}
					else
					{
						this->labelGPU->Text += L"Issues: No known issues.";
						GPUIssueText = L"Your GPU should have no issues running the game, but it is older than the GPU the game was originally designed for (GTX 650 Ti).\nThere may be some issues with graphics.\nPlease report any issues so that they can be analysed and potentially fixed.";
						this->labelGPU->LinkColor = System::Drawing::Color::Teal;
					}
				}
				else if (gpuModel->StartsWith("G") || gpuModel->StartsWith("NV") || gpuModel->StartsWith("NB") || gpuModel->StartsWith("N10") || gpuModel->StartsWith("MCP"))
				{
					this->labelGPU->Text += L"Issues: GPU too old! 3D rendering might be broken.\n(Click for more information)";
					GPUIssueText = L"Your GPU is very old and does not support rendering techniques used by the game.\nYou may be able to play, but graphics will likely have major issues.\nPlease upgrade to a GTX 600 series or later GPU.";
					this->labelGPU->LinkColor = System::Drawing::Color::Orange;
					showGpuDialog = true;
				}
				else if (version[0] >= '4' && gpuModel->Length > 0 && !gpuModel->StartsWith("Unk") && !gpuModel->StartsWith("Oth"))
				{
					this->labelGPU->Text += L"Issues: GPU may be too new.\n(Click for more information)";
					GPUIssueText = L"It looks like your GPU may be too new. Only up to Ampere (RTX 30xx) is currently supported.\nGood news: the game should be capable of running, but shader patches (probably needed) may not support it yet.\nYou will likely see lines/noise on important character shaders and some minor stage shaders.\nPlease report any issues so that ShaderPatch can be updated.\nYou can also try forcing a specific GPU workaround type.";
					this->labelGPU->LinkColor = System::Drawing::Color::Orange;
				}
				else
				{
					this->labelGPU->Text += L"Issues: Unable to detect GPU architecture.\n(Click for more information)";
					GPUIssueText = L"It looks like you have an NVIDIA GPU, but something went wrong while trying to determine your GPU's architecture (type).\nThis may be a caused by a bug, but it probably indicates potential issues.\nPlease make sure you have a GTX 600 series or later GPU. (GTX 400 series or later should also work)";
					this->labelGPU->LinkColor = System::Drawing::Color::Orange;
					showGpuDialog = true;
				}
			}
			else //if (gpuModel->Length == 0 || gpuModel->StartsWith("Unk") || gpuModel->StartsWith("Oth"))
			{
				this->labelGPU->Text += L"Issues: UNSUPPORTED GPU!";
				GPUIssueText = L"Your graphics card is not supported. Only NVIDIA GPUs are recommended (though AMD ones can be used with mods).\nPlease use a GTX 600 series or later NVIDIA GPU.\n\nIf you have a laptop with an NVIDIA GPU and wish to use it instead of the detected GPU, you may need to set diva.exe to use it in Windows settings or NVIDIA Control Panel.\n\nOwners of other GPUs may be able to run the game using plugins or mods, but 3D graphics will probably not work.";
				this->labelGPU->LinkColor = System::Drawing::Color::Red;
				showGpuDialog = true;
			}
			int linkEnd = this->labelGPU->Text->Length - linkStart;

			this->labelGPU->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::LinkLabelLinkClickedGPUIssueHandler);
			this->labelGPU->LinkArea = System::Windows::Forms::LinkArea(linkStart, linkEnd);

			if (showGpuDialog && !nNoGPUDialog)
				SkinnedMessageBox::Show(this, GPUIssueText + "\n\nYou can disable this message from the options tab.", "PD Launcher", MessageBoxButtons::OK, MessageBoxIcon::Warning);
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

		void OnPaintBackground(PaintEventArgs e) override
		{
		}

		/*property System::Windows::Forms::CreateParams^ CreateParams
		{
			virtual System::Windows::Forms::CreateParams^ get() override
			{
				System::Windows::Forms::CreateParams^ params = Form::CreateParams;
				params->ExStyle |= 0x00200000L;

				return params;
			}
		}*/

		void updateLagCompMsec()
		{
			this->groupBox_Lag->Text = "Lag Compensation (" + this->trackBar_LagCompensation->Value + " msec)";
		}

		void updateStyle()
		{
			int nDarkLauncher = GetPrivateProfileIntW(LAUNCHER_SECTION, L"dark_launcher", FALSE, CONFIG_FILE);
			int nAcrylicLauncher = GetPrivateProfileIntW(LAUNCHER_SECTION, L"acrylic_blur", FALSE, CONFIG_FILE);

			bool useAcrylic = nAcrylicLauncher && setBlur(hWnd, AccentState::ACCENT_ENABLE_ACRYLICBLURBEHIND);

			Color colourBg, colourBg2, colourFg;

			if (nDarkLauncher)
			{
				setDarkTheme(hWnd);
				colourFg = Color::White;
				colourBg = Color::FromArgb(32, 32, 32); // tabControl
				colourBg2 = Color::FromArgb(25, 25, 25);
			}
			else
			{
				setDarkTheme(hWnd, false);
				colourFg = Color::FromArgb(64, 64, 64);
				colourBg = Color::White; // tabControl
				colourBg2 = Color::FromArgb(242, 242, 242);
			}

			if (useAcrylic)
			{
				this->button_Discord->BackColor = colourBg2;
				this->button_github->BackColor = colourBg2;
				this->button_Wiki->BackColor = colourBg2;

				colourBg2 = Color::FromArgb(255, 0, 255);
			}
			else
			{
				setBlur(hWnd, AccentState::ACCENT_DISABLED);

				this->button_Discord->BackColor = Color::Transparent;
				this->button_github->BackColor = Color::Transparent;
				this->button_Wiki->BackColor = Color::Transparent;
			}

			this->tabPage_Resolution->BackColor = colourBg;
			this->tabPage_Resolution->ForeColor = colourFg;
			this->tabPage_Patches->BackColor = colourBg;
			this->tabPage_Patches->ForeColor = colourFg;
			this->tabPage_Playerdata->BackColor = colourBg;
			this->tabPage_Playerdata->ForeColor = colourFg;
			this->tabPage_Plugins->BackColor = colourBg;
			this->tabPage_Plugins->ForeColor = colourFg;
			this->tabPage_Components->BackColor = colourBg;
			this->tabPage_Components->ForeColor = colourFg;
			this->creditsTextBox->BackColor = colourBg;
			this->creditsTextBox->ForeColor = colourFg;

			this->tabControl->BackColor = colourBg;
			this->tabControl->ForeColor = colourFg;

			this->BackColor = colourBg2;

			this->button_Launch->BackColor = colourBg;
			this->button_Launch->ForeColor = colourFg;
			this->button_Apply->BackColor = colourBg;
			this->button_Apply->ForeColor = colourFg;

			this->labelGPU->ForeColor = colourFg;

			this->groupBox_Details->ForeColor = colourFg;
			this->groupBox_InternalRes->ForeColor = colourFg;
			this->groupBox_Lag->ForeColor = colourFg;
			this->groupBox_ScreenRes->ForeColor = colourFg;
		}

	private:


	private: System::Windows::Forms::Button^ button_Launch;

	private: System::Windows::Forms::GroupBox^ groupBox_ScreenRes;
	private: System::Windows::Forms::TabControl^ tabControl;
	private: System::Windows::Forms::TabPage^ tabPage_Resolution;
	private: System::Windows::Forms::GroupBox^ groupBox_InternalRes;
	private: System::Windows::Forms::TabPage^ tabPage_Components;
	private: System::Windows::Forms::Panel^ panel_Components;
	private: System::Windows::Forms::TabPage^ tabPage_Patches;
	private: System::Windows::Forms::Panel^ panel_Patches;
	private: System::Windows::Forms::TabPage^ tabPage_Plugins;
	private: System::Windows::Forms::Panel^ panel_Plugins;
	private: System::Windows::Forms::Button^ button_Discord;
	private: System::Windows::Forms::Button^ button_github;
	private: System::Windows::Forms::TabPage^  tabPage_Playerdata;
	private: System::Windows::Forms::Panel^  panel_Playerdata;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::Panel^  panel_ScreenRes;
	private: System::Windows::Forms::Panel^  panel_IntRes;
	private: System::Windows::Forms::LinkLabel^ labelGPU;



private: System::Windows::Forms::TabPage^ tabPage_Credits;
private: System::Windows::Forms::TextBox^ creditsTextBox;
private: System::Windows::Forms::GroupBox^ groupBox_Details;
private: System::Windows::Forms::Panel^ panel_Details;
private: System::Windows::Forms::Panel^ panel_Custom;
private: System::Windows::Forms::Button^ button_Apply;
private: System::Windows::Forms::GroupBox^ groupBox_Lag;
private: System::Windows::Forms::TrackBar^ trackBar_LagCompensation;
private: System::Windows::Forms::Button^ button_Wiki;




















	private: System::ComponentModel::IContainer^ components;

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
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(ui::typeid));
			this->button_Launch = (gcnew System::Windows::Forms::Button());
			this->groupBox_ScreenRes = (gcnew System::Windows::Forms::GroupBox());
			this->panel_ScreenRes = (gcnew System::Windows::Forms::Panel());
			this->tabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Resolution = (gcnew System::Windows::Forms::TabPage());
			this->groupBox_Lag = (gcnew System::Windows::Forms::GroupBox());
			this->trackBar_LagCompensation = (gcnew System::Windows::Forms::TrackBar());
			this->groupBox_Details = (gcnew System::Windows::Forms::GroupBox());
			this->panel_Details = (gcnew System::Windows::Forms::Panel());
			this->labelGPU = (gcnew System::Windows::Forms::LinkLabel());
			this->groupBox_InternalRes = (gcnew System::Windows::Forms::GroupBox());
			this->panel_IntRes = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Patches = (gcnew System::Windows::Forms::TabPage());
			this->panel_Patches = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Playerdata = (gcnew System::Windows::Forms::TabPage());
			this->panel_Playerdata = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Components = (gcnew System::Windows::Forms::TabPage());
			this->panel_Components = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Plugins = (gcnew System::Windows::Forms::TabPage());
			this->panel_Custom = (gcnew System::Windows::Forms::Panel());
			this->panel_Plugins = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Credits = (gcnew System::Windows::Forms::TabPage());
			this->creditsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->button_Discord = (gcnew System::Windows::Forms::Button());
			this->button_github = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->button_Apply = (gcnew System::Windows::Forms::Button());
			this->button_Wiki = (gcnew System::Windows::Forms::Button());
			this->groupBox_ScreenRes->SuspendLayout();
			this->tabControl->SuspendLayout();
			this->tabPage_Resolution->SuspendLayout();
			this->groupBox_Lag->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_LagCompensation))->BeginInit();
			this->groupBox_Details->SuspendLayout();
			this->groupBox_InternalRes->SuspendLayout();
			this->tabPage_Patches->SuspendLayout();
			this->tabPage_Playerdata->SuspendLayout();
			this->tabPage_Components->SuspendLayout();
			this->tabPage_Plugins->SuspendLayout();
			this->tabPage_Credits->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_Launch
			// 
			this->button_Launch->BackColor = System::Drawing::Color::White;
			this->button_Launch->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Launch->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Launch->Location = System::Drawing::Point(344, 778);
			this->button_Launch->Margin = System::Windows::Forms::Padding(6);
			this->button_Launch->Name = L"button_Launch";
			this->button_Launch->Size = System::Drawing::Size(434, 46);
			this->button_Launch->TabIndex = 20;
			this->button_Launch->Text = L"Launch";
			this->button_Launch->UseVisualStyleBackColor = false;
			this->button_Launch->Click += gcnew System::EventHandler(this, &ui::Button_Launch_Click);
			// 
			// groupBox_ScreenRes
			// 
			this->groupBox_ScreenRes->Controls->Add(this->panel_ScreenRes);
			this->groupBox_ScreenRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_ScreenRes->Location = System::Drawing::Point(10, 12);
			this->groupBox_ScreenRes->Margin = System::Windows::Forms::Padding(6);
			this->groupBox_ScreenRes->Name = L"groupBox_ScreenRes";
			this->groupBox_ScreenRes->Padding = System::Windows::Forms::Padding(6);
			this->groupBox_ScreenRes->Size = System::Drawing::Size(526, 226);
			this->groupBox_ScreenRes->TabIndex = 10;
			this->groupBox_ScreenRes->TabStop = false;
			this->groupBox_ScreenRes->Text = L"Screen Resolution";
			// 
			// panel_ScreenRes
			// 
			this->panel_ScreenRes->AutoScroll = true;
			this->panel_ScreenRes->Location = System::Drawing::Point(10, 38);
			this->panel_ScreenRes->Margin = System::Windows::Forms::Padding(6);
			this->panel_ScreenRes->Name = L"panel_ScreenRes";
			this->panel_ScreenRes->Size = System::Drawing::Size(506, 178);
			this->panel_ScreenRes->TabIndex = 0;
			// 
			// tabControl
			// 
			this->tabControl->Controls->Add(this->tabPage_Resolution);
			this->tabControl->Controls->Add(this->tabPage_Patches);
			this->tabControl->Controls->Add(this->tabPage_Playerdata);
			this->tabControl->Controls->Add(this->tabPage_Components);
			this->tabControl->Controls->Add(this->tabPage_Plugins);
			this->tabControl->Controls->Add(this->tabPage_Credits);
			this->tabControl->Location = System::Drawing::Point(0, 0);
			this->tabControl->Margin = System::Windows::Forms::Padding(6);
			this->tabControl->Name = L"tabControl";
			this->tabControl->SelectedIndex = 0;
			this->tabControl->Size = System::Drawing::Size(1120, 768);
			this->tabControl->TabIndex = 10;
			// 
			// tabPage_Resolution
			// 
			this->tabPage_Resolution->BackColor = System::Drawing::Color::White;
			this->tabPage_Resolution->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->tabPage_Resolution->Controls->Add(this->groupBox_Lag);
			this->tabPage_Resolution->Controls->Add(this->groupBox_Details);
			this->tabPage_Resolution->Controls->Add(this->labelGPU);
			this->tabPage_Resolution->Controls->Add(this->groupBox_InternalRes);
			this->tabPage_Resolution->Controls->Add(this->groupBox_ScreenRes);
			this->tabPage_Resolution->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Resolution->Location = System::Drawing::Point(8, 39);
			this->tabPage_Resolution->Margin = System::Windows::Forms::Padding(6);
			this->tabPage_Resolution->Name = L"tabPage_Resolution";
			this->tabPage_Resolution->Padding = System::Windows::Forms::Padding(6);
			this->tabPage_Resolution->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Resolution->TabIndex = 0;
			this->tabPage_Resolution->Text = L"Graphics";
			// 
			// groupBox_Lag
			// 
			this->groupBox_Lag->Controls->Add(this->trackBar_LagCompensation);
			this->groupBox_Lag->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_Lag->Location = System::Drawing::Point(10, 564);
			this->groupBox_Lag->Margin = System::Windows::Forms::Padding(6);
			this->groupBox_Lag->Name = L"groupBox_Lag";
			this->groupBox_Lag->Padding = System::Windows::Forms::Padding(6);
			this->groupBox_Lag->Size = System::Drawing::Size(526, 146);
			this->groupBox_Lag->TabIndex = 21;
			this->groupBox_Lag->TabStop = false;
			this->groupBox_Lag->Text = L"Lag Compensation";
			// 
			// trackBar_LagCompensation
			// 
			this->trackBar_LagCompensation->LargeChange = 1;
			this->trackBar_LagCompensation->Location = System::Drawing::Point(14, 40);
			this->trackBar_LagCompensation->Margin = System::Windows::Forms::Padding(6);
			this->trackBar_LagCompensation->Maximum = 500;
			this->trackBar_LagCompensation->Name = L"trackBar_LagCompensation";
			this->trackBar_LagCompensation->Size = System::Drawing::Size(500, 90);
			this->trackBar_LagCompensation->TabIndex = 0;
			this->trackBar_LagCompensation->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBar_LagCompensation->ValueChanged += gcnew System::EventHandler(this, &ui::trackBar_LagCompensation_ValueChanged);
			// 
			// groupBox_Details
			// 
			this->groupBox_Details->Controls->Add(this->panel_Details);
			this->groupBox_Details->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_Details->Location = System::Drawing::Point(548, 12);
			this->groupBox_Details->Margin = System::Windows::Forms::Padding(6);
			this->groupBox_Details->Name = L"groupBox_Details";
			this->groupBox_Details->Padding = System::Windows::Forms::Padding(6);
			this->groupBox_Details->Size = System::Drawing::Size(544, 698);
			this->groupBox_Details->TabIndex = 11;
			this->groupBox_Details->TabStop = false;
			this->groupBox_Details->Text = L"Details";
			// 
			// panel_Details
			// 
			this->panel_Details->AutoScroll = true;
			this->panel_Details->Location = System::Drawing::Point(10, 23);
			this->panel_Details->Margin = System::Windows::Forms::Padding(6);
			this->panel_Details->Name = L"panel_Details";
			this->panel_Details->Size = System::Drawing::Size(522, 667);
			this->panel_Details->TabIndex = 0;
			// 
			// labelGPU
			// 
			this->labelGPU->AutoSize = true;
			this->labelGPU->BackColor = System::Drawing::Color::Transparent;
			this->labelGPU->ForeColor = System::Drawing::Color::Black;
			this->labelGPU->Location = System::Drawing::Point(10, 418);
			this->labelGPU->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->labelGPU->MinimumSize = System::Drawing::Size(526, 26);
			this->labelGPU->Name = L"labelGPU";
			this->labelGPU->Size = System::Drawing::Size(526, 26);
			this->labelGPU->TabIndex = 21;
			this->labelGPU->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_InternalRes
			// 
			this->groupBox_InternalRes->Controls->Add(this->panel_IntRes);
			this->groupBox_InternalRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_InternalRes->Location = System::Drawing::Point(10, 248);
			this->groupBox_InternalRes->Margin = System::Windows::Forms::Padding(6);
			this->groupBox_InternalRes->Name = L"groupBox_InternalRes";
			this->groupBox_InternalRes->Padding = System::Windows::Forms::Padding(6);
			this->groupBox_InternalRes->Size = System::Drawing::Size(526, 166);
			this->groupBox_InternalRes->TabIndex = 20;
			this->groupBox_InternalRes->TabStop = false;
			this->groupBox_InternalRes->Text = L"Internal Resolution (Quality)";
			// 
			// panel_IntRes
			// 
			this->panel_IntRes->AutoScroll = true;
			this->panel_IntRes->Location = System::Drawing::Point(10, 38);
			this->panel_IntRes->Margin = System::Windows::Forms::Padding(6);
			this->panel_IntRes->Name = L"panel_IntRes";
			this->panel_IntRes->Size = System::Drawing::Size(506, 118);
			this->panel_IntRes->TabIndex = 1;
			// 
			// tabPage_Patches
			// 
			this->tabPage_Patches->BackColor = System::Drawing::Color::White;
			this->tabPage_Patches->Controls->Add(this->panel_Patches);
			this->tabPage_Patches->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Patches->Location = System::Drawing::Point(8, 39);
			this->tabPage_Patches->Margin = System::Windows::Forms::Padding(6);
			this->tabPage_Patches->Name = L"tabPage_Patches";
			this->tabPage_Patches->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Patches->TabIndex = 1;
			this->tabPage_Patches->Text = L"Options";
			// 
			// panel_Patches
			// 
			this->panel_Patches->AutoScroll = true;
			this->panel_Patches->Location = System::Drawing::Point(0, 0);
			this->panel_Patches->Margin = System::Windows::Forms::Padding(6);
			this->panel_Patches->Name = L"panel_Patches";
			this->panel_Patches->Size = System::Drawing::Size(1104, 716);
			this->panel_Patches->TabIndex = 9;
			// 
			// tabPage_Playerdata
			// 
			this->tabPage_Playerdata->BackColor = System::Drawing::Color::White;
			this->tabPage_Playerdata->Controls->Add(this->panel_Playerdata);
			this->tabPage_Playerdata->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Playerdata->Location = System::Drawing::Point(8, 39);
			this->tabPage_Playerdata->Margin = System::Windows::Forms::Padding(6);
			this->tabPage_Playerdata->Name = L"tabPage_Playerdata";
			this->tabPage_Playerdata->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Playerdata->TabIndex = 3;
			this->tabPage_Playerdata->Text = L"Player";
			// 
			// panel_Playerdata
			// 
			this->panel_Playerdata->AutoScroll = true;
			this->panel_Playerdata->Location = System::Drawing::Point(0, 0);
			this->panel_Playerdata->Margin = System::Windows::Forms::Padding(6);
			this->panel_Playerdata->Name = L"panel_Playerdata";
			this->panel_Playerdata->Size = System::Drawing::Size(1104, 716);
			this->panel_Playerdata->TabIndex = 1;
			// 
			// tabPage_Components
			// 
			this->tabPage_Components->BackColor = System::Drawing::Color::White;
			this->tabPage_Components->Controls->Add(this->panel_Components);
			this->tabPage_Components->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Components->Location = System::Drawing::Point(8, 39);
			this->tabPage_Components->Margin = System::Windows::Forms::Padding(6);
			this->tabPage_Components->Name = L"tabPage_Components";
			this->tabPage_Components->Padding = System::Windows::Forms::Padding(6);
			this->tabPage_Components->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Components->TabIndex = 2;
			this->tabPage_Components->Text = L"Components";
			// 
			// panel_Components
			// 
			this->panel_Components->AutoScroll = true;
			this->panel_Components->Location = System::Drawing::Point(0, 0);
			this->panel_Components->Margin = System::Windows::Forms::Padding(6);
			this->panel_Components->Name = L"panel_Components";
			this->panel_Components->Size = System::Drawing::Size(1104, 716);
			this->panel_Components->TabIndex = 0;
			// 
			// tabPage_Plugins
			// 
			this->tabPage_Plugins->BackColor = System::Drawing::Color::White;
			this->tabPage_Plugins->Controls->Add(this->panel_Custom);
			this->tabPage_Plugins->Controls->Add(this->panel_Plugins);
			this->tabPage_Plugins->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Plugins->Location = System::Drawing::Point(8, 39);
			this->tabPage_Plugins->Margin = System::Windows::Forms::Padding(6);
			this->tabPage_Plugins->Name = L"tabPage_Plugins";
			this->tabPage_Plugins->Padding = System::Windows::Forms::Padding(6);
			this->tabPage_Plugins->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Plugins->TabIndex = 3;
			this->tabPage_Plugins->Text = L"Plugins and Patches";
			// 
			// panel_Custom
			// 
			this->panel_Custom->AutoScroll = true;
			this->panel_Custom->Location = System::Drawing::Point(552, 0);
			this->panel_Custom->Margin = System::Windows::Forms::Padding(6);
			this->panel_Custom->Name = L"panel_Custom";
			this->panel_Custom->Size = System::Drawing::Size(552, 716);
			this->panel_Custom->TabIndex = 3;
			// 
			// panel_Plugins
			// 
			this->panel_Plugins->AutoScroll = true;
			this->panel_Plugins->Location = System::Drawing::Point(0, 0);
			this->panel_Plugins->Margin = System::Windows::Forms::Padding(6);
			this->panel_Plugins->Name = L"panel_Plugins";
			this->panel_Plugins->Size = System::Drawing::Size(552, 716);
			this->panel_Plugins->TabIndex = 0;
			// 
			// tabPage_Credits
			// 
			this->tabPage_Credits->Controls->Add(this->creditsTextBox);
			this->tabPage_Credits->Location = System::Drawing::Point(8, 39);
			this->tabPage_Credits->Margin = System::Windows::Forms::Padding(4);
			this->tabPage_Credits->Name = L"tabPage_Credits";
			this->tabPage_Credits->Padding = System::Windows::Forms::Padding(4);
			this->tabPage_Credits->Size = System::Drawing::Size(1104, 721);
			this->tabPage_Credits->TabIndex = 5;
			this->tabPage_Credits->Text = L"Credits";
			this->tabPage_Credits->UseVisualStyleBackColor = true;
			// 
			// creditsTextBox
			// 
			this->creditsTextBox->BackColor = System::Drawing::Color::White;
			this->creditsTextBox->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->creditsTextBox->Location = System::Drawing::Point(0, 0);
			this->creditsTextBox->Margin = System::Windows::Forms::Padding(4);
			this->creditsTextBox->Multiline = true;
			this->creditsTextBox->Name = L"creditsTextBox";
			this->creditsTextBox->ReadOnly = true;
			this->creditsTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->creditsTextBox->Size = System::Drawing::Size(1100, 712);
			this->creditsTextBox->TabIndex = 0;
			this->creditsTextBox->Text = resources->GetString(L"creditsTextBox.Text");
			// 
			// button_Discord
			// 
			this->button_Discord->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->button_Discord->Cursor = System::Windows::Forms::Cursors::Hand;
			this->button_Discord->FlatAppearance->BorderSize = 0;
			this->button_Discord->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Discord->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Discord.Image")));
			this->button_Discord->Location = System::Drawing::Point(992, 768);
			this->button_Discord->Margin = System::Windows::Forms::Padding(6);
			this->button_Discord->Name = L"button_Discord";
			this->button_Discord->Size = System::Drawing::Size(64, 64);
			this->button_Discord->TabIndex = 31;
			this->button_Discord->UseVisualStyleBackColor = false;
			this->button_Discord->Click += gcnew System::EventHandler(this, &ui::button_Discord_Click);
			// 
			// button_github
			// 
			this->button_github->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->button_github->Cursor = System::Windows::Forms::Cursors::Hand;
			this->button_github->FlatAppearance->BorderSize = 0;
			this->button_github->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_github->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_github.Image")));
			this->button_github->Location = System::Drawing::Point(1056, 768);
			this->button_github->Margin = System::Windows::Forms::Padding(6);
			this->button_github->Name = L"button_github";
			this->button_github->Size = System::Drawing::Size(64, 64);
			this->button_github->TabIndex = 32;
			this->button_github->UseVisualStyleBackColor = false;
			this->button_github->Click += gcnew System::EventHandler(this, &ui::button_github_Click);
			// 
			// button_Apply
			// 
			this->button_Apply->BackColor = System::Drawing::Color::White;
			this->button_Apply->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Apply->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Apply->Location = System::Drawing::Point(8, 778);
			this->button_Apply->Margin = System::Windows::Forms::Padding(6);
			this->button_Apply->Name = L"button_Apply";
			this->button_Apply->Size = System::Drawing::Size(138, 46);
			this->button_Apply->TabIndex = 33;
			this->button_Apply->Text = L"Apply";
			this->button_Apply->UseVisualStyleBackColor = false;
			this->button_Apply->Click += gcnew System::EventHandler(this, &ui::button_Apply_Click);
			// 
			// button_Wiki
			// 
			this->button_Wiki->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->button_Wiki->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Wiki.BackgroundImage")));
			this->button_Wiki->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->button_Wiki->Cursor = System::Windows::Forms::Cursors::Hand;
			this->button_Wiki->FlatAppearance->BorderSize = 0;
			this->button_Wiki->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Wiki->Location = System::Drawing::Point(928, 768);
			this->button_Wiki->Margin = System::Windows::Forms::Padding(6);
			this->button_Wiki->Name = L"button_Wiki";
			this->button_Wiki->Size = System::Drawing::Size(64, 64);
			this->button_Wiki->TabIndex = 34;
			this->button_Wiki->UseVisualStyleBackColor = false;
			this->button_Wiki->Click += gcnew System::EventHandler(this, &ui::button_Wiki_Click);
			// 
			// ui
			// 
			this->AcceptButton = this->button_Launch;
			this->AutoScaleDimensions = System::Drawing::SizeF(192, 192);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
			this->AutoSize = true;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->BackColor = System::Drawing::Color::Magenta;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(1120, 834);
			this->Controls->Add(this->button_Wiki);
			this->Controls->Add(this->button_Apply);
			this->Controls->Add(this->tabControl);
			this->Controls->Add(this->button_Launch);
			this->Controls->Add(this->button_github);
			this->Controls->Add(this->button_Discord);
			this->DoubleBuffered = true;
			this->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->HelpButton = true;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->KeyPreview = true;
			this->Margin = System::Windows::Forms::Padding(6);
			this->MaximizeBox = false;
			this->Name = L"ui";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PD Launcher (2.6.3)";
			this->TransparencyKey = System::Drawing::Color::Magenta;
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ui::Ui_FormClosing);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ui::Ui_FormClosed);
			this->groupBox_ScreenRes->ResumeLayout(false);
			this->tabControl->ResumeLayout(false);
			this->tabPage_Resolution->ResumeLayout(false);
			this->tabPage_Resolution->PerformLayout();
			this->groupBox_Lag->ResumeLayout(false);
			this->groupBox_Lag->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBar_LagCompensation))->EndInit();
			this->groupBox_Details->ResumeLayout(false);
			this->groupBox_InternalRes->ResumeLayout(false);
			this->tabPage_Patches->ResumeLayout(false);
			this->tabPage_Playerdata->ResumeLayout(false);
			this->tabPage_Components->ResumeLayout(false);
			this->tabPage_Plugins->ResumeLayout(false);
			this->tabPage_Credits->ResumeLayout(false);
			this->tabPage_Credits->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
private: System::Void SaveSettings() {
	if (*ResolutionConfigChanged)
	{
		for (ConfigOptionBase* option : screenResolutionArray)
		{
			option->SaveOption();
		}

		for (ConfigOptionBase* option : internalResolutionArray)
		{
			option->SaveOption();
		}

		*ResolutionConfigChanged = false;
	}

	if (*OptionsConfigChanged)
	{
		for (ConfigOptionBase* option : optionsArray)
		{
			option->SaveOption();
		}

		for (ConfigOptionBase* option : graphicsArray)
		{
			option->SaveOption();
		}

		*OptionsConfigChanged = false;
	}

	if (*PlayerdataConfigChanged)
	{
		for (ConfigOptionBase* option : playerdataArray)
		{
			option->SaveOption();
		}

		*PlayerdataConfigChanged = false;
	}

	if (*ComponentsConfigChanged)
	{
		for (ConfigOptionBase* component : componentsArray)
		{
			component->SaveOption();
		}

		*ComponentsConfigChanged = false;
	}

	if (*LagCompensationConfigChanged)
	{
		SetIniInt(GRAPHICS_SECTION, L"lag_compensation", trackBar_LagCompensation->Value, CONFIG_FILE);
		*LagCompensationConfigChanged = false;
	}

	for (ConfigOptionBase* option : AllPluginOpts)
	{
		if (*(option->hasChanged))
		{
			option->SaveOption();
		}

		*(option->hasChanged) = false;
	}

	for (ConfigOptionBase* option : AllCustomOpts)
	{
		if (*(option->hasChanged))
		{
			option->SaveOption();
		}

		*(option->hasChanged) = false;
	}
}
private: System::Void Button_Help_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki");
}
private: System::Void Button_Launch_Click(System::Object^ sender, System::EventArgs^ e) {
	
	//SkinnedMessageBox::Show(this, "It looks like the executable has been tampered with, or the version of the game is not 7.10.\n\nPlease use the \"patches\" folder instead of modifying the executable directly (or disable verification).", "PD Launcher", MessageBoxButtons::OK, MessageBoxIcon::Error);

	SaveSettings();

	if (GetPrivateProfileIntW(LAUNCHER_SECTION, L"use_divahook_bat", FALSE, CONFIG_FILE))
	{
		if (GetPrivateProfileIntW(L"patches", L"quick_start", 1, CONFIG_FILE) > 0)
		{
			SkinnedMessageBox::Show(this, L"\"Quick Start\" and \"Use divahook.bat / start.bat\" cannot be enabled at the same time.", "PD Launcher", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		DIVA_EXECUTABLE = NULL;
		wstring DIVA_EXECUTABLE_LAUNCH_STRING_cmd = L"cmd.exe /C \"";
		const wstring DIVA_EXECUTABLE_LAUNCH_STRING_path = DIVA_DIRPATH;

		wstring DIVAHOOK_BAT_PATH = DIVA_EXECUTABLE_LAUNCH_STRING_path;
		DIVAHOOK_BAT_PATH.append(L"\\divahook.bat");
		if (std::filesystem::exists(DIVAHOOK_BAT_PATH))
		{
			DIVA_EXECUTABLE_LAUNCH_STRING_cmd.append(DIVAHOOK_BAT_PATH);
			DIVA_EXECUTABLE_LAUNCH_STRING_cmd.append(L"\"");
			DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING_cmd.c_str());
		}
		else
		{
			wstring START_BAT_PATH = DIVA_EXECUTABLE_LAUNCH_STRING_path;
			START_BAT_PATH.append(L"\\start.bat");
			if (std::filesystem::exists(START_BAT_PATH))
			{
				DIVA_EXECUTABLE_LAUNCH_STRING_cmd.append(START_BAT_PATH);
				DIVA_EXECUTABLE_LAUNCH_STRING_cmd.append(L"\"");
				DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING_cmd.c_str());
			}
			else if (SkinnedMessageBox::Show(this, L"divahook.bat/start.bat not found.\nThe game will start normally/offline.\n\nNOTE: Disable \"Use divahook.bat / start.bat\" to avoid this warning.", "PD Launcher", MessageBoxButtons::OKCancel, MessageBoxIcon::Warning) != System::Windows::Forms::DialogResult::OK) return;
		}
	}
	else
	{
		// read the command line here so it'll be up to date even if the user changed it
		WCHAR stringBuf[256];
		GetPrivateProfileStringW(LAUNCHER_SECTION, L"command_line", L"", stringBuf, 256, CONFIG_FILE);

		DIVA_EXECUTABLE_LAUNCH_STRING += L" " + wstring(stringBuf);
		DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING.c_str());
	}

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

	if (idx == 2) // fullscreen
	{
		for (ConfigOptionBase* option : screenResolutionArray)
		{
			if (option == RefreshRateOption)
			{
				((Control^)Control::FromHandle(option->mainControlHandle))->Enabled = true;
			}
		}
	}
	else
	{
		for (ConfigOptionBase* option : screenResolutionArray)
		{
			if (option == RefreshRateOption)
			{
				((Control^)Control::FromHandle(option->mainControlHandle))->Enabled = false;
			}
		}
	}
}
private: System::Void Ui_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
	if (!AnyConfigChanged())
		return;

	switch (SkinnedMessageBox::Show(this, "Do you want to save your settings?", "PD Launcher", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question))
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
	switch (SkinnedMessageBox::Show(this, "Would you like to read the user guide for troubleshooting?", "PD Launcher", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Information))
	{
	case System::Windows::Forms::DialogResult::Yes:
		System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki");
		break;

	case System::Windows::Forms::DialogResult::No:
		System::Diagnostics::Process::Start("https://discord.gg/cvBVGDZ");
		break;
	}
}
private: System::Void button_github_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader");
}
private: System::Void button_Apply_Click(System::Object^ sender, System::EventArgs^ e) {
	SaveSettings();
	updateStyle();
	ShowWindow(hWnd, SW_HIDE);
	ShowWindow(hWnd, SW_SHOW);
}
private: System::Void Ui_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e) {
	if (e->KeyCode == Keys::Escape) this->Close();
}

private: bool* ResolutionConfigChanged = new bool(false);
		 bool* OptionsConfigChanged = new bool(false);
		 bool* PlayerdataConfigChanged = new bool(false);
		 bool* ComponentsConfigChanged = new bool(false);
		 bool* LagCompensationConfigChanged = new bool(false);

private: bool AnyConfigChanged() {
	if (*ResolutionConfigChanged || *OptionsConfigChanged || *PlayerdataConfigChanged || *ComponentsConfigChanged || *LagCompensationConfigChanged)
		return true; // fast case

	for (ConfigOptionBase* option : AllPluginOpts)
	{
		if (*(option->hasChanged)) return true; // slow case for iterating over individual plugins
	}

	return false;
}

private: String^ GPUIssueText;
private: System::Void LinkLabelLinkClickedGPUIssueHandler(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	SkinnedMessageBox::Show(this, GPUIssueText, "PD Launcher", MessageBoxButtons::OK, MessageBoxIcon::Warning);
}
private: System::Void trackBar_LagCompensation_ValueChanged(System::Object^ sender, System::EventArgs^ e) {
	*LagCompensationConfigChanged = true;

	updateLagCompMsec();
}
private: System::Void tabPage_Resolution_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button_Wiki_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki");
}
private: System::Void trackBar_LagCompensation_Scroll(System::Object^ sender, System::EventArgs^ e) {
}
};
}
