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
#include "registry.h"
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
			this->checkBox_nofsopt->Checked = !getFullscreenOptimizations();
			this->checkBox_nofsopt->CheckedChanged += gcnew System::EventHandler(this, &ui::checkBox_nofsopt_CheckedChanged);


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
			int optionsY = 33;
			for (ConfigOptionBase* option : optionsArray)
			{
				option->hasChanged = OptionsConfigChanged;
				optionsY += option->AddToPanel(panel_Patches, 12, optionsY, toolTip1);
			}

			Button^ keycfg_button = gcnew Button();
			keycfg_button->Left = 12;
			keycfg_button->Top = 3;
			keycfg_button->Width = 200;
			Form^ RootForm = panel_Patches->FindForm();
			float ScaleWidth = 1.0f;
			float ScaleHeight = 1.0f;
			if (RootForm)
			{
				Drawing::SizeF CurrentScaleSize = RootForm->CurrentAutoScaleDimensions;
				ScaleWidth = CurrentScaleSize.Width / BaseScaleSize;
				ScaleHeight = CurrentScaleSize.Height / BaseScaleSize;
			}
			keycfg_button->Scale(ScaleWidth, ScaleHeight);
			keycfg_button->Text = gcnew String("Keyboard/Controller Configuration");
			keycfg_button->Click += gcnew EventHandler(this, &ui::keycfg_button_Click);
			panel_Patches->Controls->Add(keycfg_button);

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

			scanModpacks();


			glutInit(&argc, argv);
			int window = glutCreateWindow("glut"); // a context must be created to use glGetString
			glutHideWindow();

			String^ vendor = gcnew String((char*)glGetString(GL_VENDOR));
			vendor = vendor->Replace(" Corporation", ""); // this is useless..  remove it to help ensure the GPU type line fits
			String^ renderer = gcnew String((char*)glGetString(GL_RENDERER));
			String^ version = gcnew String((char*)glGetString(GL_VERSION));

			String^ gpuModel = gcnew String(GPUModel::getGpuName().c_str());

			String^ wineVersion = gcnew String(WineVer::getWineVer().c_str());

			glutDestroyWindow(window); // destroy the window so it doesn't remain on screen
			if (glutMainLoopEventDynamic != NULL) glutMainLoopEventDynamic(); // freeglut needs this

			if (wineVersion != "")
			{
				this->labelGPU->Text = "Wine " + wineVersion + " (remember to disable movies!)\n";
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
					this->labelGPU->Text += "Issues: AMD GPU support is unofficial.";
					GPUIssueText = "AMD GPUs are not supported without mods.\nThe PD Loader AMDPack seems to be installed, but please keep in mind that it may have issues.";
					this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
				}
				else
				{
					this->labelGPU->Text += "Issues: Mods needed for AMD compatibility!";
					GPUIssueText = "AMD GPUs are not supported without mods.\nThe PD Loader AMDPack can be used to make 3D rendering work. Please download it and follow the included instructions.\n\nIf you have a laptop with an NVIDIA GPU and wish to use it instead of the detected GPU, you may need to set diva.exe to use it in Windows settings or NVIDIA Control Panel.";
					this->labelGPU->LinkColor = System::Drawing::Color::Red;
					showGpuDialog = true;
				}
			}
			else if (!vendor->Contains("NVIDIA"))
			{
				this->labelGPU->Text += "Issues: UNSUPPORTED GPU!";
				GPUIssueText = "Your graphics card is not supported. Only NVIDIA GPUs are recommended. (though AMD ones can be used with mods)\nPlease use a GTX 600 series or later NVIDIA GPU.\n\nIf you have a laptop with an NVIDIA GPU and wish to use it instead of the detected GPU, you may need to set diva.exe to use it in Windows settings or NVIDIA Control Panel.\n\nOwners of other GPUs may be able to run the game using plugins or mods, but 3D graphics will probably not work.";
				this->labelGPU->LinkColor = System::Drawing::Color::Red;
				showGpuDialog = true;
			}
			else if (gpuModel->StartsWith("GA")) // unconfirmed??
			{
				this->labelGPU->Text += "Issues: Ampere GPU detected! Unknown issues.\n(Click for more information)";
				GPUIssueText = "Ampere GPUs have not been tested yet. On the previous generation Turing GPUs (GTX 16xx/RTX 20xx), some important character shaders have issues resulting in lines/noise.\nPlease make sure the ShaderPatch plugin is enabled and report any further issues to the developers.";
				this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
			}
			else if (gpuModel->StartsWith("TU"))
			{
				this->labelGPU->Text += "Issues: Turing GPU detected! Possible noise.\n(Click for more information)";
				GPUIssueText = "On Turing GPUs (GTX 16xx/RTX 20xx), some important character shaders have issues resulting in lines/noise.\nPlease make sure the ShaderPatch plugin is enabled.";
				this->labelGPU->LinkColor = System::Drawing::Color::Yellow;
			}
			else if (gpuModel->StartsWith("GM") || gpuModel->StartsWith("GP") || gpuModel->StartsWith("GV"))
			{
				this->labelGPU->Text += "Issues: May have minor noise on some stages.\n(Click for more information)";
				GPUIssueText = "On Maxwell GPUs (~GTX 900) or newer, some minor stage shaders create noise.\nPlease make sure the ShaderPatch plugin is enabled.";
				this->labelGPU->LinkColor = System::Drawing::Color::Teal;
			}
			else if (gpuModel->StartsWith("GK"))
			{
				this->labelGPU->Text += "Issues: None.";
				GPUIssueText = "Your GPU should have no issues running the game.";
				this->labelGPU->LinkColor = System::Drawing::Color::LightBlue;
			}
			else if (gpuModel->StartsWith("GF"))
			{
				if (version[0] < '4')
				{
					this->labelGPU->Text += "Issues: Driver too old.";
					GPUIssueText = "Your GPU should be able to run the game, but it looks like your OpenGL version is too old.\nA driver update should fix this.";
					this->labelGPU->LinkColor = System::Drawing::Color::Orange;
					showGpuDialog = true;
				}
				else
				{
					this->labelGPU->Text += "Issues: No known issues.";
					GPUIssueText = "Your GPU should have no issues running the game, but it is older than the GPU the game was originally designed for (GTX 650 Ti).\nThere may be some issues with graphics.\nPlease report any issues so that they can be analysed and potentially fixed.";
					this->labelGPU->LinkColor = System::Drawing::Color::Teal;
				}
			}
			else if (version[0] >= '4' && gpuModel->Length > 0 && !gpuModel->StartsWith("Unk") && !gpuModel->StartsWith("Oth"))
			{
				this->labelGPU->Text += "Issues: GPU may be too new.\n(Click for more information)";
				GPUIssueText = "It looks like your GPU may be too new. Only up to Turing (GTX 16xx/RTX 20xx) is currently supported.\nGood news: the game should be capable of running, but shader patches (probably needed) may not support it yet.\nYou will likely see lines/noise on important character shaders and some minor stage shaders.\nPlease report any issues so that ShaderPatch can be updated.";
				this->labelGPU->LinkColor = System::Drawing::Color::Orange;
			}
			else if (gpuModel->StartsWith("G") || gpuModel->StartsWith("NV") || gpuModel->StartsWith("NB") || gpuModel->StartsWith("N10") || gpuModel->StartsWith("MCP"))
			{
				this->labelGPU->Text += "Issues: GPU too old! 3D rendering might be broken.\n(Click for more information)";
				GPUIssueText = "Your GPU is very old and does not support rendering techniques used by the game.\nYou may be able to play, but graphics will likely have major issues.\nPlease upgrade to a GTX 600 series or later GPU.";
				this->labelGPU->LinkColor = System::Drawing::Color::Orange;
				showGpuDialog = true;
			}
			else //if (gpuModel->Length == 0 || gpuModel->StartsWith("Unk") || gpuModel->StartsWith("Oth"))
			{
				this->labelGPU->Text += "Issues: Unable to detect GPU architecture.\n(Click for more information)";
				GPUIssueText = "It looks like you have an NVIDIA GPU, but something went wrong while trying to determine your GPU's architecture (type).\nThis may be a caused by a bug, but it probably indicates potential issues.\nPlease make sure you have a GTX 600 series or later GPU. (GTX 400 series or later should also work)";
				this->labelGPU->LinkColor = System::Drawing::Color::Orange;
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

			//bool useAcrylic = nAcrylicLauncher && setBlur(hWnd, AccentState::ACCENT_ENABLE_ACRYLICBLURBEHIND);

			Color colourBg, colourBg2, colourFg;
			System::Windows::Forms::FlatStyle cbxStyle;

			if (nDarkLauncher)
			{
				setDarkTheme(hWnd);
				colourFg = Color::White;
				colourBg = Color::FromArgb(255, 32, 32, 32); // tabControl
				colourBg2 = Color::FromArgb(255, 25, 25, 25);
				cbxStyle = System::Windows::Forms::FlatStyle::Flat;
			}
			else
			{
				setDarkTheme(hWnd, false);
				colourFg = Color::FromArgb(255, 64, 64, 64);
				colourBg = Color::White; // tabControl
				colourBg2 = Color::FromArgb(255, 242, 242, 242);
				cbxStyle = System::Windows::Forms::FlatStyle::System;
			}

			/*if (useAcrylic)
			{
				this->button_Discord->BackColor = colourBg2;
				this->button_github->BackColor = colourBg2;
				this->button_Wiki->BackColor = colourBg2;

				colourBg2 = Color::FromArgb(255, 0, 255);
			}
			else
			{
				setBlur(hWnd, AccentState::ACCENT_DISABLED);*/

				this->button_Discord->BackColor = Color::Transparent;
				this->button_github->BackColor = Color::Transparent;
				this->button_Wiki->BackColor = Color::Transparent;
			//}

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

			/*for (int i = 0; i < this->panel_ScreenRes->Controls->Count; i++) this->panel_ScreenRes->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_IntRes->Controls->Count; i++) this->panel_IntRes->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Details->Controls->Count; i++) this->panel_Details->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Patches->Controls->Count; i++) this->panel_Patches->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Custom->Controls->Count; i++) this->panel_Custom->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Playerdata->Controls->Count; i++) this->panel_Playerdata->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Components->Controls->Count; i++) this->panel_Components->Controls[i]->BackColor = colourBg;
			for (int i = 0; i < this->panel_Plugins->Controls->Count; i++) this->panel_Plugins->Controls[i]->BackColor = colourBg;*/

			SetBackCol(this->panel_ScreenRes, colourBg, cbxStyle);
			SetBackCol(this->panel_IntRes, colourBg, cbxStyle);
			SetBackCol(this->panel_Details, colourBg, cbxStyle);
			SetBackCol(this->panel_Patches, colourBg, cbxStyle);
			SetBackCol(this->panel_Custom, colourBg, cbxStyle);
			SetBackCol(this->panel_Playerdata, colourBg, cbxStyle);
			SetBackCol(this->panel_Components, colourBg, cbxStyle);
			SetBackCol(this->panel_Plugins, colourBg, cbxStyle);

			tabPage_Modpacks->Enabled = false;
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
private: System::Windows::Forms::TabPage^ tabPage_Update;
private: System::Windows::Forms::Label^ label_Update_2;

private: System::Windows::Forms::Label^ label_Update_1;
private: System::Windows::Forms::Button^ button_Clean_installation;

private: System::Windows::Forms::Button^ button_Instructions;

private: System::Windows::Forms::Button^ button_Unstable_builds;

private: System::Windows::Forms::Button^ button_Releases_page;
private: System::Windows::Forms::LinkLabel^ linkLabel_Changelog;
private: System::Windows::Forms::LinkLabel^ linkLabel_Official_Discord;
private: System::Windows::Forms::LinkLabel^ linkLabel_Help;
private: System::Windows::Forms::LinkLabel^ linkLabel_Repo;
private: System::Windows::Forms::CheckBox^ checkBox_nofsopt;
private: System::Windows::Forms::TabPage^ tabPage_Modpacks;
private: System::Windows::Forms::CheckBox^ checkBox_ModPatches;
private: System::Windows::Forms::Button^ button_ModDelete;

private: System::Windows::Forms::Button^ button_ModClone;

private: System::Windows::Forms::Button^ button_ModSetActive;
private: System::Windows::Forms::ListBox^ listBox_Mods;

private: System::Windows::Forms::Button^ button_ModFiles;
private: System::Windows::Forms::Button^ button_ModRescan;
private: System::Windows::Forms::Button^ button_ModRename;
private: System::Windows::Forms::LinkLabel^ linkLabel_Console;





















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
			this->checkBox_nofsopt = (gcnew System::Windows::Forms::CheckBox());
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
			this->linkLabel_Console = (gcnew System::Windows::Forms::LinkLabel());
			this->panel_Components = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Plugins = (gcnew System::Windows::Forms::TabPage());
			this->panel_Custom = (gcnew System::Windows::Forms::Panel());
			this->panel_Plugins = (gcnew System::Windows::Forms::Panel());
			this->tabPage_Modpacks = (gcnew System::Windows::Forms::TabPage());
			this->button_ModRename = (gcnew System::Windows::Forms::Button());
			this->button_ModRescan = (gcnew System::Windows::Forms::Button());
			this->button_ModFiles = (gcnew System::Windows::Forms::Button());
			this->checkBox_ModPatches = (gcnew System::Windows::Forms::CheckBox());
			this->button_ModDelete = (gcnew System::Windows::Forms::Button());
			this->button_ModClone = (gcnew System::Windows::Forms::Button());
			this->button_ModSetActive = (gcnew System::Windows::Forms::Button());
			this->listBox_Mods = (gcnew System::Windows::Forms::ListBox());
			this->tabPage_Credits = (gcnew System::Windows::Forms::TabPage());
			this->creditsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->tabPage_Update = (gcnew System::Windows::Forms::TabPage());
			this->linkLabel_Repo = (gcnew System::Windows::Forms::LinkLabel());
			this->linkLabel_Help = (gcnew System::Windows::Forms::LinkLabel());
			this->linkLabel_Official_Discord = (gcnew System::Windows::Forms::LinkLabel());
			this->linkLabel_Changelog = (gcnew System::Windows::Forms::LinkLabel());
			this->button_Clean_installation = (gcnew System::Windows::Forms::Button());
			this->button_Instructions = (gcnew System::Windows::Forms::Button());
			this->button_Unstable_builds = (gcnew System::Windows::Forms::Button());
			this->button_Releases_page = (gcnew System::Windows::Forms::Button());
			this->label_Update_2 = (gcnew System::Windows::Forms::Label());
			this->label_Update_1 = (gcnew System::Windows::Forms::Label());
			this->button_Discord = (gcnew System::Windows::Forms::Button());
			this->button_github = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->button_Apply = (gcnew System::Windows::Forms::Button());
			this->button_Wiki = (gcnew System::Windows::Forms::Button());
			this->groupBox_ScreenRes->SuspendLayout();
			this->panel_ScreenRes->SuspendLayout();
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
			this->tabPage_Modpacks->SuspendLayout();
			this->tabPage_Credits->SuspendLayout();
			this->tabPage_Update->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_Launch
			// 
			this->button_Launch->BackColor = System::Drawing::Color::White;
			this->button_Launch->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Launch->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Launch->Location = System::Drawing::Point(172, 409);
			this->button_Launch->Name = L"button_Launch";
			this->button_Launch->Size = System::Drawing::Size(217, 23);
			this->button_Launch->TabIndex = 20;
			this->button_Launch->Text = L"Launch";
			this->button_Launch->UseVisualStyleBackColor = false;
			this->button_Launch->Click += gcnew System::EventHandler(this, &ui::Button_Launch_Click);
			// 
			// groupBox_ScreenRes
			// 
			this->groupBox_ScreenRes->Controls->Add(this->panel_ScreenRes);
			this->groupBox_ScreenRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_ScreenRes->Location = System::Drawing::Point(5, 6);
			this->groupBox_ScreenRes->Name = L"groupBox_ScreenRes";
			this->groupBox_ScreenRes->Size = System::Drawing::Size(263, 120);
			this->groupBox_ScreenRes->TabIndex = 10;
			this->groupBox_ScreenRes->TabStop = false;
			this->groupBox_ScreenRes->Text = L"Screen Resolution";
			// 
			// panel_ScreenRes
			// 
			this->panel_ScreenRes->AutoScroll = true;
			this->panel_ScreenRes->Controls->Add(this->checkBox_nofsopt);
			this->panel_ScreenRes->Location = System::Drawing::Point(5, 19);
			this->panel_ScreenRes->Name = L"panel_ScreenRes";
			this->panel_ScreenRes->Size = System::Drawing::Size(253, 99);
			this->panel_ScreenRes->TabIndex = 0;
			// 
			// checkBox_nofsopt
			// 
			this->checkBox_nofsopt->AutoSize = true;
			this->checkBox_nofsopt->Location = System::Drawing::Point(14, 77);
			this->checkBox_nofsopt->Margin = System::Windows::Forms::Padding(2);
			this->checkBox_nofsopt->Name = L"checkBox_nofsopt";
			this->checkBox_nofsopt->Size = System::Drawing::Size(177, 17);
			this->checkBox_nofsopt->TabIndex = 0;
			this->checkBox_nofsopt->Text = L"Disable Fullscreen Optimizations";
			this->toolTip1->SetToolTip(this->checkBox_nofsopt, L"Disables Windows 10/11 fullscreen optimizations, which can cause issues with scre"
				L"enshots and/or framerate.");
			this->checkBox_nofsopt->UseVisualStyleBackColor = true;
			// 
			// tabControl
			// 
			this->tabControl->Controls->Add(this->tabPage_Resolution);
			this->tabControl->Controls->Add(this->tabPage_Patches);
			this->tabControl->Controls->Add(this->tabPage_Playerdata);
			this->tabControl->Controls->Add(this->tabPage_Components);
			this->tabControl->Controls->Add(this->tabPage_Plugins);
			this->tabControl->Controls->Add(this->tabPage_Modpacks);
			this->tabControl->Controls->Add(this->tabPage_Credits);
			this->tabControl->Controls->Add(this->tabPage_Update);
			this->tabControl->Location = System::Drawing::Point(0, 0);
			this->tabControl->Name = L"tabControl";
			this->tabControl->SelectedIndex = 0;
			this->tabControl->Size = System::Drawing::Size(560, 404);
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
			this->tabPage_Resolution->Location = System::Drawing::Point(4, 22);
			this->tabPage_Resolution->Name = L"tabPage_Resolution";
			this->tabPage_Resolution->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Resolution->Size = System::Drawing::Size(552, 378);
			this->tabPage_Resolution->TabIndex = 0;
			this->tabPage_Resolution->Text = L"Graphics";
			// 
			// groupBox_Lag
			// 
			this->groupBox_Lag->Controls->Add(this->trackBar_LagCompensation);
			this->groupBox_Lag->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_Lag->Location = System::Drawing::Point(5, 303);
			this->groupBox_Lag->Name = L"groupBox_Lag";
			this->groupBox_Lag->Size = System::Drawing::Size(263, 69);
			this->groupBox_Lag->TabIndex = 21;
			this->groupBox_Lag->TabStop = false;
			this->groupBox_Lag->Text = L"Lag Compensation";
			// 
			// trackBar_LagCompensation
			// 
			this->trackBar_LagCompensation->LargeChange = 1;
			this->trackBar_LagCompensation->Location = System::Drawing::Point(7, 20);
			this->trackBar_LagCompensation->Maximum = 500;
			this->trackBar_LagCompensation->Name = L"trackBar_LagCompensation";
			this->trackBar_LagCompensation->Size = System::Drawing::Size(250, 45);
			this->trackBar_LagCompensation->TabIndex = 0;
			this->trackBar_LagCompensation->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBar_LagCompensation->ValueChanged += gcnew System::EventHandler(this, &ui::trackBar_LagCompensation_ValueChanged);
			// 
			// groupBox_Details
			// 
			this->groupBox_Details->Controls->Add(this->panel_Details);
			this->groupBox_Details->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_Details->Location = System::Drawing::Point(274, 6);
			this->groupBox_Details->Name = L"groupBox_Details";
			this->groupBox_Details->Size = System::Drawing::Size(272, 366);
			this->groupBox_Details->TabIndex = 11;
			this->groupBox_Details->TabStop = false;
			this->groupBox_Details->Text = L"Details";
			// 
			// panel_Details
			// 
			this->panel_Details->AutoScroll = true;
			this->panel_Details->Location = System::Drawing::Point(5, 19);
			this->panel_Details->Name = L"panel_Details";
			this->panel_Details->Size = System::Drawing::Size(261, 341);
			this->panel_Details->TabIndex = 0;
			// 
			// labelGPU
			// 
			this->labelGPU->AutoSize = true;
			this->labelGPU->BackColor = System::Drawing::Color::Transparent;
			this->labelGPU->ForeColor = System::Drawing::Color::Black;
			this->labelGPU->Location = System::Drawing::Point(5, 213);
			this->labelGPU->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelGPU->MinimumSize = System::Drawing::Size(263, 13);
			this->labelGPU->Name = L"labelGPU";
			this->labelGPU->Size = System::Drawing::Size(263, 13);
			this->labelGPU->TabIndex = 21;
			this->labelGPU->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox_InternalRes
			// 
			this->groupBox_InternalRes->Controls->Add(this->panel_IntRes);
			this->groupBox_InternalRes->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->groupBox_InternalRes->Location = System::Drawing::Point(5, 127);
			this->groupBox_InternalRes->Name = L"groupBox_InternalRes";
			this->groupBox_InternalRes->Size = System::Drawing::Size(263, 83);
			this->groupBox_InternalRes->TabIndex = 20;
			this->groupBox_InternalRes->TabStop = false;
			this->groupBox_InternalRes->Text = L"Internal Resolution (Quality)";
			// 
			// panel_IntRes
			// 
			this->panel_IntRes->AutoScroll = true;
			this->panel_IntRes->Location = System::Drawing::Point(5, 19);
			this->panel_IntRes->Name = L"panel_IntRes";
			this->panel_IntRes->Size = System::Drawing::Size(253, 59);
			this->panel_IntRes->TabIndex = 1;
			// 
			// tabPage_Patches
			// 
			this->tabPage_Patches->BackColor = System::Drawing::Color::White;
			this->tabPage_Patches->Controls->Add(this->panel_Patches);
			this->tabPage_Patches->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Patches->Location = System::Drawing::Point(4, 22);
			this->tabPage_Patches->Name = L"tabPage_Patches";
			this->tabPage_Patches->Size = System::Drawing::Size(552, 378);
			this->tabPage_Patches->TabIndex = 1;
			this->tabPage_Patches->Text = L"Options";
			// 
			// panel_Patches
			// 
			this->panel_Patches->AutoScroll = true;
			this->panel_Patches->Location = System::Drawing::Point(0, 0);
			this->panel_Patches->Name = L"panel_Patches";
			this->panel_Patches->Size = System::Drawing::Size(552, 378);
			this->panel_Patches->TabIndex = 9;
			// 
			// tabPage_Playerdata
			// 
			this->tabPage_Playerdata->BackColor = System::Drawing::Color::White;
			this->tabPage_Playerdata->Controls->Add(this->panel_Playerdata);
			this->tabPage_Playerdata->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Playerdata->Location = System::Drawing::Point(4, 22);
			this->tabPage_Playerdata->Name = L"tabPage_Playerdata";
			this->tabPage_Playerdata->Size = System::Drawing::Size(552, 378);
			this->tabPage_Playerdata->TabIndex = 3;
			this->tabPage_Playerdata->Text = L"Player";
			// 
			// panel_Playerdata
			// 
			this->panel_Playerdata->AutoScroll = true;
			this->panel_Playerdata->Location = System::Drawing::Point(0, 0);
			this->panel_Playerdata->Name = L"panel_Playerdata";
			this->panel_Playerdata->Size = System::Drawing::Size(552, 378);
			this->panel_Playerdata->TabIndex = 1;
			// 
			// tabPage_Components
			// 
			this->tabPage_Components->BackColor = System::Drawing::Color::White;
			this->tabPage_Components->Controls->Add(this->linkLabel_Console);
			this->tabPage_Components->Controls->Add(this->panel_Components);
			this->tabPage_Components->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Components->Location = System::Drawing::Point(4, 22);
			this->tabPage_Components->Name = L"tabPage_Components";
			this->tabPage_Components->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Components->Size = System::Drawing::Size(552, 378);
			this->tabPage_Components->TabIndex = 2;
			this->tabPage_Components->Text = L"Components";
			// 
			// linkLabel_Console
			// 
			this->linkLabel_Console->AutoSize = true;
			this->linkLabel_Console->Location = System::Drawing::Point(504, 362);
			this->linkLabel_Console->Name = L"linkLabel_Console";
			this->linkLabel_Console->Size = System::Drawing::Size(45, 13);
			this->linkLabel_Console->TabIndex = 1;
			this->linkLabel_Console->TabStop = true;
			this->linkLabel_Console->Text = L"Console";
			this->linkLabel_Console->TextAlign = System::Drawing::ContentAlignment::BottomRight;
			this->linkLabel_Console->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::linkLabel_Console_LinkClicked);
			// 
			// panel_Components
			// 
			this->panel_Components->AutoScroll = true;
			this->panel_Components->Location = System::Drawing::Point(0, 0);
			this->panel_Components->Name = L"panel_Components";
			this->panel_Components->Size = System::Drawing::Size(276, 378);
			this->panel_Components->TabIndex = 0;
			// 
			// tabPage_Plugins
			// 
			this->tabPage_Plugins->BackColor = System::Drawing::Color::White;
			this->tabPage_Plugins->Controls->Add(this->panel_Custom);
			this->tabPage_Plugins->Controls->Add(this->panel_Plugins);
			this->tabPage_Plugins->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(64)), static_cast<System::Int32>(static_cast<System::Byte>(64)),
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->tabPage_Plugins->Location = System::Drawing::Point(4, 22);
			this->tabPage_Plugins->Name = L"tabPage_Plugins";
			this->tabPage_Plugins->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Plugins->Size = System::Drawing::Size(552, 378);
			this->tabPage_Plugins->TabIndex = 3;
			this->tabPage_Plugins->Text = L"Plugins and Patches";
			// 
			// panel_Custom
			// 
			this->panel_Custom->AutoScroll = true;
			this->panel_Custom->Location = System::Drawing::Point(276, 0);
			this->panel_Custom->Name = L"panel_Custom";
			this->panel_Custom->Size = System::Drawing::Size(276, 378);
			this->panel_Custom->TabIndex = 3;
			// 
			// panel_Plugins
			// 
			this->panel_Plugins->AutoScroll = true;
			this->panel_Plugins->Location = System::Drawing::Point(0, 0);
			this->panel_Plugins->Name = L"panel_Plugins";
			this->panel_Plugins->Size = System::Drawing::Size(276, 378);
			this->panel_Plugins->TabIndex = 0;
			// 
			// tabPage_Modpacks
			// 
			this->tabPage_Modpacks->Controls->Add(this->button_ModRename);
			this->tabPage_Modpacks->Controls->Add(this->button_ModRescan);
			this->tabPage_Modpacks->Controls->Add(this->button_ModFiles);
			this->tabPage_Modpacks->Controls->Add(this->checkBox_ModPatches);
			this->tabPage_Modpacks->Controls->Add(this->button_ModDelete);
			this->tabPage_Modpacks->Controls->Add(this->button_ModClone);
			this->tabPage_Modpacks->Controls->Add(this->button_ModSetActive);
			this->tabPage_Modpacks->Controls->Add(this->listBox_Mods);
			this->tabPage_Modpacks->Location = System::Drawing::Point(4, 22);
			this->tabPage_Modpacks->Margin = System::Windows::Forms::Padding(2);
			this->tabPage_Modpacks->Name = L"tabPage_Modpacks";
			this->tabPage_Modpacks->Padding = System::Windows::Forms::Padding(2);
			this->tabPage_Modpacks->Size = System::Drawing::Size(552, 378);
			this->tabPage_Modpacks->TabIndex = 7;
			this->tabPage_Modpacks->Text = L"Modpacks";
			this->tabPage_Modpacks->UseVisualStyleBackColor = true;
			this->tabPage_Modpacks->Enter += gcnew System::EventHandler(this, &ui::tabPage_Modpacks_Enter);
			// 
			// button_ModRename
			// 
			this->button_ModRename->Enabled = false;
			this->button_ModRename->Location = System::Drawing::Point(279, 357);
			this->button_ModRename->Margin = System::Windows::Forms::Padding(2);
			this->button_ModRename->Name = L"button_ModRename";
			this->button_ModRename->Size = System::Drawing::Size(65, 21);
			this->button_ModRename->TabIndex = 8;
			this->button_ModRename->Text = L"Rename";
			this->button_ModRename->UseVisualStyleBackColor = true;
			this->button_ModRename->Click += gcnew System::EventHandler(this, &ui::button_ModRename_Click);
			// 
			// button_ModRescan
			// 
			this->button_ModRescan->Location = System::Drawing::Point(348, 357);
			this->button_ModRescan->Margin = System::Windows::Forms::Padding(2);
			this->button_ModRescan->Name = L"button_ModRescan";
			this->button_ModRescan->Size = System::Drawing::Size(65, 21);
			this->button_ModRescan->TabIndex = 7;
			this->button_ModRescan->Text = L"Rescan";
			this->button_ModRescan->UseVisualStyleBackColor = true;
			this->button_ModRescan->Click += gcnew System::EventHandler(this, &ui::button_ModRescan_Click);
			// 
			// button_ModFiles
			// 
			this->button_ModFiles->Enabled = false;
			this->button_ModFiles->Location = System::Drawing::Point(73, 357);
			this->button_ModFiles->Margin = System::Windows::Forms::Padding(2);
			this->button_ModFiles->Name = L"button_ModFiles";
			this->button_ModFiles->Size = System::Drawing::Size(65, 21);
			this->button_ModFiles->TabIndex = 6;
			this->button_ModFiles->Text = L"Files...";
			this->button_ModFiles->UseVisualStyleBackColor = true;
			this->button_ModFiles->Click += gcnew System::EventHandler(this, &ui::button_ModFiles_Click);
			// 
			// checkBox_ModPatches
			// 
			this->checkBox_ModPatches->AutoSize = true;
			this->checkBox_ModPatches->Checked = true;
			this->checkBox_ModPatches->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_ModPatches->Enabled = false;
			this->checkBox_ModPatches->Location = System::Drawing::Point(430, 358);
			this->checkBox_ModPatches->Margin = System::Windows::Forms::Padding(2);
			this->checkBox_ModPatches->Name = L"checkBox_ModPatches";
			this->checkBox_ModPatches->Size = System::Drawing::Size(123, 17);
			this->checkBox_ModPatches->TabIndex = 5;
			this->checkBox_ModPatches->Text = L"Plugins and Patches";
			this->checkBox_ModPatches->UseVisualStyleBackColor = true;
			// 
			// button_ModDelete
			// 
			this->button_ModDelete->Enabled = false;
			this->button_ModDelete->Location = System::Drawing::Point(211, 357);
			this->button_ModDelete->Margin = System::Windows::Forms::Padding(2);
			this->button_ModDelete->Name = L"button_ModDelete";
			this->button_ModDelete->Size = System::Drawing::Size(65, 21);
			this->button_ModDelete->TabIndex = 4;
			this->button_ModDelete->Text = L"Delete";
			this->button_ModDelete->UseVisualStyleBackColor = true;
			this->button_ModDelete->Click += gcnew System::EventHandler(this, &ui::button_ModDelete_Click);
			// 
			// button_ModClone
			// 
			this->button_ModClone->Enabled = false;
			this->button_ModClone->Location = System::Drawing::Point(142, 357);
			this->button_ModClone->Margin = System::Windows::Forms::Padding(2);
			this->button_ModClone->Name = L"button_ModClone";
			this->button_ModClone->Size = System::Drawing::Size(65, 21);
			this->button_ModClone->TabIndex = 3;
			this->button_ModClone->Text = L"New/Clone";
			this->button_ModClone->UseVisualStyleBackColor = true;
			this->button_ModClone->Click += gcnew System::EventHandler(this, &ui::button_ModClone_Click);
			// 
			// button_ModSetActive
			// 
			this->button_ModSetActive->Enabled = false;
			this->button_ModSetActive->Location = System::Drawing::Point(5, 357);
			this->button_ModSetActive->Margin = System::Windows::Forms::Padding(2);
			this->button_ModSetActive->Name = L"button_ModSetActive";
			this->button_ModSetActive->Size = System::Drawing::Size(65, 21);
			this->button_ModSetActive->TabIndex = 2;
			this->button_ModSetActive->Text = L"Set Active";
			this->button_ModSetActive->UseVisualStyleBackColor = true;
			this->button_ModSetActive->Click += gcnew System::EventHandler(this, &ui::button_ModSetActive_Click);
			// 
			// listBox_Mods
			// 
			this->listBox_Mods->FormattingEnabled = true;
			this->listBox_Mods->Location = System::Drawing::Point(5, 5);
			this->listBox_Mods->Margin = System::Windows::Forms::Padding(2);
			this->listBox_Mods->Name = L"listBox_Mods";
			this->listBox_Mods->Size = System::Drawing::Size(546, 342);
			this->listBox_Mods->TabIndex = 0;
			this->listBox_Mods->SelectedValueChanged += gcnew System::EventHandler(this, &ui::listBox_Mods_SelectedValueChanged);
			// 
			// tabPage_Credits
			// 
			this->tabPage_Credits->Controls->Add(this->creditsTextBox);
			this->tabPage_Credits->Location = System::Drawing::Point(4, 22);
			this->tabPage_Credits->Margin = System::Windows::Forms::Padding(2);
			this->tabPage_Credits->Name = L"tabPage_Credits";
			this->tabPage_Credits->Padding = System::Windows::Forms::Padding(2);
			this->tabPage_Credits->Size = System::Drawing::Size(552, 378);
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
			this->creditsTextBox->Margin = System::Windows::Forms::Padding(2);
			this->creditsTextBox->Multiline = true;
			this->creditsTextBox->Name = L"creditsTextBox";
			this->creditsTextBox->ReadOnly = true;
			this->creditsTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->creditsTextBox->Size = System::Drawing::Size(552, 378);
			this->creditsTextBox->TabIndex = 0;
			this->creditsTextBox->Text = resources->GetString(L"creditsTextBox.Text");
			// 
			// tabPage_Update
			// 
			this->tabPage_Update->Controls->Add(this->linkLabel_Repo);
			this->tabPage_Update->Controls->Add(this->linkLabel_Help);
			this->tabPage_Update->Controls->Add(this->linkLabel_Official_Discord);
			this->tabPage_Update->Controls->Add(this->linkLabel_Changelog);
			this->tabPage_Update->Controls->Add(this->button_Clean_installation);
			this->tabPage_Update->Controls->Add(this->button_Instructions);
			this->tabPage_Update->Controls->Add(this->button_Unstable_builds);
			this->tabPage_Update->Controls->Add(this->button_Releases_page);
			this->tabPage_Update->Controls->Add(this->label_Update_2);
			this->tabPage_Update->Controls->Add(this->label_Update_1);
			this->tabPage_Update->Location = System::Drawing::Point(4, 22);
			this->tabPage_Update->Margin = System::Windows::Forms::Padding(2);
			this->tabPage_Update->Name = L"tabPage_Update";
			this->tabPage_Update->Padding = System::Windows::Forms::Padding(2);
			this->tabPage_Update->Size = System::Drawing::Size(552, 378);
			this->tabPage_Update->TabIndex = 6;
			this->tabPage_Update->Text = L"UPDATE";
			this->tabPage_Update->UseVisualStyleBackColor = true;
			// 
			// linkLabel_Repo
			// 
			this->linkLabel_Repo->AutoSize = true;
			this->linkLabel_Repo->Location = System::Drawing::Point(169, 297);
			this->linkLabel_Repo->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->linkLabel_Repo->Name = L"linkLabel_Repo";
			this->linkLabel_Repo->Size = System::Drawing::Size(87, 13);
			this->linkLabel_Repo->TabIndex = 9;
			this->linkLabel_Repo->TabStop = true;
			this->linkLabel_Repo->Text = L"Official repository";
			this->linkLabel_Repo->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::linkLabel_Repo_LinkClicked);
			// 
			// linkLabel_Help
			// 
			this->linkLabel_Help->AutoSize = true;
			this->linkLabel_Help->Location = System::Drawing::Point(169, 257);
			this->linkLabel_Help->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->linkLabel_Help->Name = L"linkLabel_Help";
			this->linkLabel_Help->Size = System::Drawing::Size(29, 13);
			this->linkLabel_Help->TabIndex = 8;
			this->linkLabel_Help->TabStop = true;
			this->linkLabel_Help->Text = L"Help";
			this->linkLabel_Help->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::linkLabel_Help_LinkClicked);
			// 
			// linkLabel_Official_Discord
			// 
			this->linkLabel_Official_Discord->AutoSize = true;
			this->linkLabel_Official_Discord->Location = System::Drawing::Point(169, 277);
			this->linkLabel_Official_Discord->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->linkLabel_Official_Discord->Name = L"linkLabel_Official_Discord";
			this->linkLabel_Official_Discord->Size = System::Drawing::Size(78, 13);
			this->linkLabel_Official_Discord->TabIndex = 7;
			this->linkLabel_Official_Discord->TabStop = true;
			this->linkLabel_Official_Discord->Text = L"Official Discord";
			this->linkLabel_Official_Discord->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::linkLabel_Official_Discord_LinkClicked);
			// 
			// linkLabel_Changelog
			// 
			this->linkLabel_Changelog->AutoSize = true;
			this->linkLabel_Changelog->Location = System::Drawing::Point(169, 238);
			this->linkLabel_Changelog->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->linkLabel_Changelog->Name = L"linkLabel_Changelog";
			this->linkLabel_Changelog->Size = System::Drawing::Size(58, 13);
			this->linkLabel_Changelog->TabIndex = 6;
			this->linkLabel_Changelog->TabStop = true;
			this->linkLabel_Changelog->Text = L"Changelog";
			this->linkLabel_Changelog->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &ui::linkLabel_Changelog_LinkClicked);
			// 
			// button_Clean_installation
			// 
			this->button_Clean_installation->Location = System::Drawing::Point(295, 207);
			this->button_Clean_installation->Margin = System::Windows::Forms::Padding(2);
			this->button_Clean_installation->Name = L"button_Clean_installation";
			this->button_Clean_installation->Size = System::Drawing::Size(92, 26);
			this->button_Clean_installation->TabIndex = 5;
			this->button_Clean_installation->Text = L"Clean installation";
			this->button_Clean_installation->UseVisualStyleBackColor = true;
			this->button_Clean_installation->Click += gcnew System::EventHandler(this, &ui::button_Clean_installation_Click);
			// 
			// button_Instructions
			// 
			this->button_Instructions->Location = System::Drawing::Point(295, 177);
			this->button_Instructions->Margin = System::Windows::Forms::Padding(2);
			this->button_Instructions->Name = L"button_Instructions";
			this->button_Instructions->Size = System::Drawing::Size(92, 26);
			this->button_Instructions->TabIndex = 4;
			this->button_Instructions->Text = L"Instructions";
			this->button_Instructions->UseVisualStyleBackColor = true;
			this->button_Instructions->Click += gcnew System::EventHandler(this, &ui::button_Instructions_Click);
			// 
			// button_Unstable_builds
			// 
			this->button_Unstable_builds->Location = System::Drawing::Point(169, 207);
			this->button_Unstable_builds->Margin = System::Windows::Forms::Padding(2);
			this->button_Unstable_builds->Name = L"button_Unstable_builds";
			this->button_Unstable_builds->Size = System::Drawing::Size(92, 26);
			this->button_Unstable_builds->TabIndex = 3;
			this->button_Unstable_builds->Text = L"Unstable builds";
			this->button_Unstable_builds->UseVisualStyleBackColor = true;
			this->button_Unstable_builds->Click += gcnew System::EventHandler(this, &ui::button_Unstable_builds_Click);
			// 
			// button_Releases_page
			// 
			this->button_Releases_page->Location = System::Drawing::Point(169, 177);
			this->button_Releases_page->Margin = System::Windows::Forms::Padding(2);
			this->button_Releases_page->Name = L"button_Releases_page";
			this->button_Releases_page->Size = System::Drawing::Size(92, 26);
			this->button_Releases_page->TabIndex = 2;
			this->button_Releases_page->Text = L"Releases page";
			this->button_Releases_page->UseVisualStyleBackColor = true;
			this->button_Releases_page->Click += gcnew System::EventHandler(this, &ui::button_Releases_page_Click);
			// 
			// label_Update_2
			// 
			this->label_Update_2->AutoSize = true;
			this->label_Update_2->Location = System::Drawing::Point(48, 73);
			this->label_Update_2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label_Update_2->Name = L"label_Update_2";
			this->label_Update_2->Size = System::Drawing::Size(467, 26);
			this->label_Update_2->TabIndex = 1;
			this->label_Update_2->Text = L"It is highly recommended to download updates directly from the official repositor"
				L"y.\r\nIf you have downloaded PD Loader from a third-party, it is recommended to do"
				L" a clean installation.";
			this->label_Update_2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_Update_1
			// 
			this->label_Update_1->AutoSize = true;
			this->label_Update_1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_Update_1->Location = System::Drawing::Point(11, 30);
			this->label_Update_1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label_Update_1->Name = L"label_Update_1";
			this->label_Update_1->Size = System::Drawing::Size(555, 20);
			this->label_Update_1->TabIndex = 0;
			this->label_Update_1->Text = L"Update PD Loader regularly to get the latest fixes and new features!";
			this->label_Update_1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// button_Discord
			// 
			this->button_Discord->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(242)),
				static_cast<System::Int32>(static_cast<System::Byte>(242)));
			this->button_Discord->Cursor = System::Windows::Forms::Cursors::Hand;
			this->button_Discord->FlatAppearance->BorderSize = 0;
			this->button_Discord->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Discord->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button_Discord.Image")));
			this->button_Discord->Location = System::Drawing::Point(496, 404);
			this->button_Discord->Name = L"button_Discord";
			this->button_Discord->Size = System::Drawing::Size(32, 32);
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
			this->button_github->Location = System::Drawing::Point(528, 404);
			this->button_github->Name = L"button_github";
			this->button_github->Size = System::Drawing::Size(32, 32);
			this->button_github->TabIndex = 32;
			this->button_github->UseVisualStyleBackColor = false;
			this->button_github->Click += gcnew System::EventHandler(this, &ui::button_github_Click);
			// 
			// button_Apply
			// 
			this->button_Apply->BackColor = System::Drawing::Color::White;
			this->button_Apply->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button_Apply->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button_Apply->Location = System::Drawing::Point(4, 409);
			this->button_Apply->Name = L"button_Apply";
			this->button_Apply->Size = System::Drawing::Size(69, 23);
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
			this->button_Wiki->Location = System::Drawing::Point(464, 404);
			this->button_Wiki->Name = L"button_Wiki";
			this->button_Wiki->Size = System::Drawing::Size(32, 32);
			this->button_Wiki->TabIndex = 34;
			this->button_Wiki->UseVisualStyleBackColor = false;
			this->button_Wiki->Click += gcnew System::EventHandler(this, &ui::button_Wiki_Click);
			// 
			// ui
			// 
			this->AcceptButton = this->button_Launch;
			this->AutoScaleDimensions = System::Drawing::SizeF(96, 96);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
			this->AutoSize = true;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->BackColor = System::Drawing::Color::Magenta;
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(560, 437);
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
			this->MaximizeBox = false;
			this->Name = L"ui";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PD Loader 3 - Launcher";
			this->TransparencyKey = System::Drawing::Color::Magenta;
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &ui::Ui_FormClosing);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ui::Ui_FormClosed);
			this->groupBox_ScreenRes->ResumeLayout(false);
			this->panel_ScreenRes->ResumeLayout(false);
			this->panel_ScreenRes->PerformLayout();
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
			this->tabPage_Components->PerformLayout();
			this->tabPage_Plugins->ResumeLayout(false);
			this->tabPage_Modpacks->ResumeLayout(false);
			this->tabPage_Modpacks->PerformLayout();
			this->tabPage_Credits->ResumeLayout(false);
			this->tabPage_Credits->PerformLayout();
			this->tabPage_Update->ResumeLayout(false);
			this->tabPage_Update->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		private: System::Void scanModpacks() {
			button_ModSetActive->Enabled = false;
			button_ModClone->Enabled = false;
			button_ModDelete->Enabled = false;
			button_ModFiles->Enabled = false;
			button_ModRename->Enabled = false;
			listBox_Mods->Items->Clear();
			listBox_Mods->Items->Add("(base)");
			try {
				for (std::filesystem::path p : std::filesystem::directory_iterator("./modpacks"))
				{
					if (std::filesystem::exists(p.string() + "/ram") && std::filesystem::exists(p.string() + "/mdata") &&
						std::filesystem::exists(p.string() + "/plugins") && std::filesystem::exists(p.string() + "/patches"))
					{
						listBox_Mods->Items->Add(gcnew String(p.filename().c_str()));
					}
				}
			}
			catch (const std::filesystem::filesystem_error& e) {
				std::cout << "[Launcher] File system error " << e.what() << " " << e.path1() << " " << e.path2() << " " << e.code() << std::endl;
			}
		}
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

		/*if (getFullscreenOptimizations() == this->checkBox_nofsopt->Checked)*/ setFullscreenOptimizations(!this->checkBox_nofsopt->Checked);
		this->checkBox_nofsopt->Checked = !getFullscreenOptimizations();

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

	if(GetPrivateProfileIntW(LAUNCHER_SECTION, L"use_divahook_bat", FALSE, CONFIG_FILE))
	{
		DIVA_EXECUTABLE = NULL;
		wstring DIVA_EXECUTABLE_LAUNCH_STRING = L"cmd.exe /C \"";
		DIVA_EXECUTABLE_LAUNCH_STRING.append(DIVA_DIRPATH);
		DIVA_EXECUTABLE_LAUNCH_STRING.append(L"\\divahook.bat\"");
		DIVA_EXECUTABLE_LAUNCH = const_cast<WCHAR*>(DIVA_EXECUTABLE_LAUNCH_STRING.c_str());
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
	//ShowWindow(hWnd, SW_HIDE);
	//ShowWindow(hWnd, SW_SHOW);
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
private: System::Void button_Releases_page_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/releases");
}
private: System::Void button_Instructions_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki/2%29+Installation");
}
private: System::Void button_Unstable_builds_Click(System::Object^ sender, System::EventArgs^ e) {
	System::Diagnostics::Process::Start("https://ci.appveyor.com/project/nastys/pd-loader/build/artifacts");
}
private: System::Void button_Clean_installation_Click(System::Object^ sender, System::EventArgs^ e) {
	SkinnedMessageBox::Show(this, "Delete:\n- dnsapi.dll or dinput8.dll\n- patches/\n- plugins/\n\nRestore:\n- glut32.dll\n\nThen install PD Loader from the official repository.", "How to do a clean installation", MessageBoxButtons::OK, MessageBoxIcon::Information);
}
private: System::Void linkLabel_Changelog_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki/Changelog");
}
private: System::Void linkLabel_Official_Discord_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://discord.gg/cvBVGDZ");
}
private: System::Void linkLabel_Help_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader/wiki");
}
private: System::Void linkLabel_Repo_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	System::Diagnostics::Process::Start("https://notabug.org/PDModdingCommunity/PD-Loader");
}
private: System::Void checkBox_nofsopt_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	*ResolutionConfigChanged = true;
}
private: System::Void keycfg_button_Click(System::Object^ sender, System::EventArgs^ e)
{
    System::Diagnostics::Process::Start("\"" + System::Environment::GetEnvironmentVariable("WINDIR") + "\\write.exe\"", "plugins\\keyconfig.ini");
}
private: System::Void linkLabel_Console_LinkClicked(System::Object^ sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^ e) {
	HWND consoleHandle = GetConsoleWindow();
	ShowWindow(consoleHandle, SW_SHOW);
	linkLabel_Console->Visible = false;
}
private: System::Void listBox_Mods_SelectedValueChanged(System::Object^ sender, System::EventArgs^ e) {
	button_ModSetActive->Enabled = true;
	button_ModClone->Enabled = true;
	button_ModDelete->Enabled = true;
	button_ModFiles->Enabled = true;
	button_ModRename->Enabled = true;
}
private: System::Void button_ModRescan_Click(System::Object^ sender, System::EventArgs^ e) {
	scanModpacks();
}
private: System::Void button_ModSetActive_Click(System::Object^ sender, System::EventArgs^ e) {
	LPCWSTR mpname;
	if (listBox_Mods->SelectedIndex == 0) mpname = L"";
	else
	{
		auto str = listBox_Mods->GetItemText(listBox_Mods->SelectedItem);
		IntPtr strptr = System::Runtime::InteropServices::Marshal::StringToHGlobalUni(str);
		mpname = (LPCWSTR)strptr.ToPointer();
	}
	WritePrivateProfileStringW(L"Mods", L"Modpack", mpname, CONFIG_FILE);
}
private: System::Void button_ModFiles_Click(System::Object^ sender, System::EventArgs^ e) {
	auto mpstr = listBox_Mods->SelectedItem->ToString();
	if(mpstr == "(base)") System::Diagnostics::Process::Start(".");
	else System::Diagnostics::Process::Start("modpacks\\"+mpstr);
}
private: System::Void button_ModClone_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button_ModDelete_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button_ModRename_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void tabPage_Modpacks_Enter(System::Object^ sender, System::EventArgs^ e) {
	scanModpacks();
	wchar_t* mpstr = NULL;
	mpstr = (wchar_t*)malloc(256);
	memset(mpstr, 0, 256);
	GetPrivateProfileStringW(L"Mods", L"Modpack", L"", mpstr, 256, CONFIG_FILE);
	listBox_Mods->SelectedIndex = listBox_Mods->FindString(gcnew String(mpstr));
	free(mpstr);
}
};
}
