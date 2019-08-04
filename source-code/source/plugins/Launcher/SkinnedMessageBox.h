#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

// ported from https://stackoverflow.com/a/7785745
// original code by user LarsTech https://stackoverflow.com/users/719186/larstech

static ref class SkinnedMessageBox
{
private: ref class SkinnedMessageBoxForm : Form
	{
	private:
		void SkinnedMessageBoxForm_Shown(Object^ sender, EventArgs^ e)
		{
			Activate();
		}

		void SkinnedMessageBoxForm_FormClosed(Object^ sender, FormClosedEventArgs^ e)
		{
			if (CancelButton && (DialogResult == System::Windows::Forms::DialogResult::None || DialogResult == System::Windows::Forms::DialogResult::Cancel))
				DialogResult = CancelButton->DialogResult;
		}

		void InitComponentNew()
		{
			this->SuspendLayout();

			this->AutoSize = true;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Width = 350;
			this->Height = 140;
			this->StartPosition = FormStartPosition::CenterScreen;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->ShowInTaskbar = false;
			this->ShowIcon = false;
			__try
			{
				this->Icon = Application::OpenForms[0]->Icon;
			}
			__except (1) { }
			this->Shown += gcnew EventHandler(this, &SkinnedMessageBoxForm::SkinnedMessageBoxForm_Shown);
			this->FormClosed += gcnew FormClosedEventHandler(this, &SkinnedMessageBoxForm::SkinnedMessageBoxForm_FormClosed);

			//this->Font = SystemFonts::MessageBoxFont;
			//this->Font = gcnew Font(SystemFonts::MessageBoxFont->FontFamily, SystemFonts::MessageBoxFont->SizeInPoints);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;

			this->ResumeLayout(false);
			this->PerformLayout();
		}

		float ScaleWidth = 1.0;
		float ScaleHeight = 1.0;
		int btnWidth = 85;
		int btnHeight = 23;
		int btnPadding = 12;

		static cli::array<Button^>^ buttonrefs = gcnew cli::array<Button^>{ gcnew Button, gcnew Button, gcnew Button };
		int numbuttons = 1;

		Button^ AddBtn(String^ text, System::Windows::Forms::DialogResult result, int btnNumber)
		{
			Button^ btn = buttonrefs[btnNumber];
			btn->Text = text;
			btn->DialogResult = result;
			btn->Anchor = AnchorStyles::Bottom | AnchorStyles::Right;
			btn->Width = btnWidth;
			btn->Height = btnHeight;

			btn->Location = Point(ClientSize.Width - btnWidth * (numbuttons - btnNumber) - btnPadding * (numbuttons - btnNumber), ClientSize.Height - btnHeight - btnPadding);
			Controls->Add(btn);

			//buttonrefs[btnNumber] = btn;
			return btn;
		}

		void SetBackCol(Control^ elem, Color color)
		{
			Type^ elemType = elem->GetType();

			if ((elem->HasChildren || elemType == Button::typeid || elemType == ComboBox::typeid) && elemType != NumericUpDown::typeid)
			{
				elem->BackColor = color;

				for (int i = 0; i < elem->Controls->Count; i++)
				{
					SetBackCol(elem->Controls[i], color);
				}
			}
		}

		void SetForeCol(Control^ elem, Color color)
		{
			Type^ elemType = elem->GetType();

			if (elemType != NumericUpDown::typeid && elemType != TextBox::typeid)
			{
				elem->ForeColor = color;
			}
			if (elem->HasChildren && elemType != NumericUpDown::typeid)
			{
				for (int i = 0; i < elem->Controls->Count; i++)
				{
					SetForeCol(elem->Controls[i], color);
				}
			}
		}

		void SetUIStyle(Control^ elem, FlatStyle style)
		{
			Type^ elemType = elem->GetType();

			if (elemType == Button::typeid)
			{
				((Button^)elem)->FlatStyle = style;
			}
			else if (elemType  == ComboBox::typeid)
			{
				((ComboBox^)elem)->FlatStyle = style;
			}
			else if (elemType == CheckBox::typeid)
			{
				((CheckBox^)elem)->FlatStyle = style;
			}

			if (elem->HasChildren && elemType != NumericUpDown::typeid)
			{
				for (int i = 0; i < elem->Controls->Count; i++)
				{
					SetUIStyle(elem->Controls[i], style);
				}
			}
		}

	public:
		SkinnedMessageBoxForm(IWin32Window^ owner, String^ message, String^ caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultbutton, MessageBoxOptions options)
		{
			InitComponentNew();
			Graphics^ grfx = Graphics::FromHwnd(Handle);
			ScaleWidth = Font->SizeInPoints / 72 * grfx->DpiX / 10.67f;
			ScaleHeight = ScaleWidth;
			btnWidth *= ScaleWidth;
			btnHeight *= ScaleHeight;
			btnPadding *= ScaleWidth;

			Width *= ScaleWidth;
			Height *= ScaleHeight;

			if (owner->Handle != IntPtr::Zero)
			{
				Owner = Control::FromHandle(owner->Handle)->FindForm();
			}
			else
			{
				Owner = ActiveForm;
			}

			Text = caption;

			System::Windows::Forms::Padding lblMargin = System::Windows::Forms::Padding(14 * ScaleWidth, 0 * ScaleHeight, 14 * ScaleWidth, 45 * ScaleHeight);
			Point lblLocation = Point(14 * ScaleWidth, 14 * ScaleHeight);
			Drawing::Size lblMinSize = Drawing::Size(ClientSize.Width - lblLocation.X - lblMargin.Right, 0);
			Drawing::Size lblMaxSize = Drawing::Size(600 * ScaleWidth, 0 * ScaleHeight);

			Label^ MessageLbl = gcnew Label();
			MessageLbl->Text = message;
			MessageLbl->AutoSize = true;
			MessageLbl->Margin = lblMargin;
			MessageLbl->Location = lblLocation;
			MessageLbl->MinimumSize = lblMinSize;
			MessageLbl->MaximumSize = lblMaxSize;

			Controls->Add(MessageLbl);

			if (buttons == MessageBoxButtons::OK)
			{
				numbuttons = 1;
				Button^ OkBtn = AddBtn("&OK", System::Windows::Forms::DialogResult::OK, 0);
				CancelButton = OkBtn;
				//(not sure why, but this matches official .NET behaviour)
			}
			else if (buttons == MessageBoxButtons::OKCancel)
			{
				numbuttons = 2;
				AddBtn("&OK", System::Windows::Forms::DialogResult::OK, 0);
				Button^ CancelBtn = AddBtn("&Cancel", System::Windows::Forms::DialogResult::Cancel, 1);
				CancelButton = CancelBtn;
			}
			else if (buttons == MessageBoxButtons::YesNo)
			{
				numbuttons = 2;
				AddBtn("&Yes", System::Windows::Forms::DialogResult::Yes, 0);
				Button^ NoBtn = AddBtn("&No", System::Windows::Forms::DialogResult::No, 1);
				CancelButton = NoBtn;
				//(official .NET doesn't set cancel button, but this seems pretty clear to me...)
			}
			else if (buttons == MessageBoxButtons::RetryCancel)
			{
				numbuttons = 2;
				AddBtn("&Retry", System::Windows::Forms::DialogResult::Retry, 0);
				Button^ CancelBtn = AddBtn("&Cancel", System::Windows::Forms::DialogResult::Cancel, 1);
				CancelButton = CancelBtn;
			}
			else if (buttons == MessageBoxButtons::YesNoCancel)
			{
				numbuttons = 3;
				AddBtn("&Yes", System::Windows::Forms::DialogResult::Yes, 0);
				AddBtn("&No", System::Windows::Forms::DialogResult::No, 1);
				Button^ CancelBtn = AddBtn("&Cancel", System::Windows::Forms::DialogResult::Cancel, 2);
				CancelButton = CancelBtn;
			}
			else if (buttons == MessageBoxButtons::AbortRetryIgnore)
			{
				numbuttons = 3;
				AddBtn("&Abort", System::Windows::Forms::DialogResult::Abort, 0);
				AddBtn("&Retry", System::Windows::Forms::DialogResult::Retry, 1);
				AddBtn("&Ignore", System::Windows::Forms::DialogResult::Ignore, 2);
				//this.CancelButton = IgnoreBtn;
				//(official .NET doesn't set cancel button)
			}
			else
			{
				throw gcnew System::ComponentModel::InvalidEnumArgumentException("buttons", (int)buttons, MessageBoxButtons::typeid);
			}

			if (!CancelButton)
				ControlBox = false;


			if (icon != MessageBoxIcon::None)
			{
				int iconsize = 32 * ScaleWidth;
				int iconpadding = 12 * ScaleWidth;
				int textpadding = -8 * ScaleWidth;

				PictureBox^ iconPB = gcnew PictureBox();
				iconPB->Width = iconsize;
				iconPB->Height = iconsize;
				iconPB->Top = 10 * ScaleHeight;
				iconPB->Left = 10 * ScaleWidth;
				iconPB->AccessibleName = "Icon";
				iconPB->SizeMode = PictureBoxSizeMode::Zoom;

				System::Resources::ResourceManager^ ResMgr = gcnew System::Resources::ResourceManager("Launcher.SkinnedMessageBox", SkinnedMessageBox::typeid->Assembly);

				// there are many duplicated values:
				// https://docs.microsoft.com/en-us/dotnet/api/system.windows.forms.messageboxicon?view=netframework-4.7.2
				if (icon == MessageBoxIcon::Error) // also Hand and Stop
				{
					Drawing::Icon^ erricon = gcnew Drawing::Icon(SystemIcons::Error, iconsize, iconsize);
					iconPB->Image = erricon->ToBitmap();
					iconPB->AccessibleDescription = "Error";
					//this.Icon = SystemIcons.Error;
					System::Media::SystemSounds::Hand->Play();
				}
				else if (icon == MessageBoxIcon::Question)
				{
					iconPB->Image = (Image^)(ResMgr->GetObject("PokeWhat4xvc2"));
					iconPB->AccessibleDescription = "Question";
					//this.Icon = SystemIcons.Question;
					System::Media::SystemSounds::Question->Play();
				}
				else if (icon == MessageBoxIcon::Warning) // also Exclamation
				{
					iconPB->Image = (Image^)(ResMgr->GetObject("PokeSnarf4xd"));
					iconPB->AccessibleDescription = "Warning";
					//this.Icon = SystemIcons.Warning;
					//textpadding -= 2 * ScaleWidth; // triangle shape looks worse without this
					System::Media::SystemSounds::Exclamation->Play();
				}
				else if (icon == MessageBoxIcon::Information) // also Asterisk
				{
					Drawing::Icon^ infoicon = gcnew Drawing::Icon(SystemIcons::Information, iconsize, iconsize);
					iconPB->Image = infoicon->ToBitmap();
					iconPB->AccessibleDescription = "Information";
					//this.Icon = SystemIcons.Question;
					System::Media::SystemSounds::Asterisk->Play();
				}
				else
				{
					throw gcnew System::ComponentModel::InvalidEnumArgumentException("icon", (int)icon, MessageBoxIcon::typeid);
				}

				MessageLbl->MinimumSize = Drawing::Size(MessageLbl->MinimumSize.Width - (iconsize + iconpadding + textpadding), MessageLbl->MinimumSize.Height);
				MessageLbl->Left += (iconsize + iconpadding + textpadding);
				Controls->Add(iconPB);
			}

			if (defaultbutton == MessageBoxDefaultButton::Button1)
			{
				AcceptButton = buttonrefs[0];
				ActiveControl = buttonrefs[0];
			}
			else if (defaultbutton == MessageBoxDefaultButton::Button2)
			{
				if (!Controls->Contains(buttonrefs[1])) throw gcnew ArgumentOutOfRangeException("defaultbutton", "defaultbutton cannot be set to a button number that does not exist");
				AcceptButton = buttonrefs[1];
				ActiveControl = buttonrefs[1];
			}
			else if (defaultbutton == MessageBoxDefaultButton::Button3)
			{
				if (!Controls->Contains(buttonrefs[2])) throw gcnew ArgumentOutOfRangeException("defaultbutton", "defaultbutton cannot be set to a button number that does not exist");
				AcceptButton = buttonrefs[2];
				ActiveControl = buttonrefs[2];
			}
			else
			{
				throw gcnew System::ComponentModel::InvalidEnumArgumentException("defaultbutton", (int)defaultbutton, MessageBoxDefaultButton::typeid);
			}


			// TODO: TEXT ALIGNMENT AND RTL MODE
			/*
				// MessageBoxOptions are more like flags than concrete settings.
				// For ease of use, we assume default value is whatever winforms gives us in current locale -- but we override if any value is set
				if ((options & MessageBoxOptions.RightAlign) == MessageBoxOptions.RightAlign) MessageLbl.TextAlign = ContentAlignment.TopRight;
				else if (options != 0) MessageLbl.TextAlign = ContentAlignment.TopLeft;
				
				// RtlReading should be what actually affects layout, and RightAlign only affects the text label
				if ((options & MessageBoxOptions.RtlReading) == MessageBoxOptions.RtlReading)
				{
				    foreach (Control control in this.Controls)
				    {
				        control.Left = this.ClientSize.Width - control.Right;
				        control.RightToLeft = RightToLeft.Yes;
				    }
				    this.RightToLeft = RightToLeft.Yes;
				    this.RightToLeftLayout = true;
				}
				else if (options != 0)
				{
				    foreach (Control control in this.Controls)
				    {
				        control.RightToLeft = RightToLeft.No;
				    }
				    this.RightToLeft = RightToLeft.No;
				    this.RightToLeftLayout = false;
				}
			*/

			SetBackCol(this, Color::FromArgb(64, 64, 64));
			SetForeCol(this, Color::White);
			SetUIStyle(this, FlatStyle::Flat);
		}

		SkinnedMessageBoxForm(String^ message, String^ caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultbutton, MessageBoxOptions options)
		{
			SkinnedMessageBoxForm(NativeWindow::FromHandle((IntPtr::Zero)), message, caption, buttons, icon, defaultbutton, options);
		}
	};

public:
	static DialogResult Show(IWin32Window^ owner, String^ message, String^ caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultbutton, MessageBoxOptions options)
	{
		// I probably shouldn't do this, but from what I can tell this should be safe enough -- out of thread owner is allowed
		// and it only seems to appear when debugging (???)
		Control::CheckForIllegalCrossThreadCalls = false;
		SkinnedMessageBoxForm^ mb = gcnew SkinnedMessageBoxForm(owner, message, caption, buttons, icon, defaultbutton, options);
		DialogResult res = mb->ShowDialog();
		Control::CheckForIllegalCrossThreadCalls = true;
		return res;
	}

	static DialogResult Show(IWin32Window^ owner, String^ message, String^ caption, MessageBoxButtons buttons, MessageBoxIcon icon, MessageBoxDefaultButton defaultbutton)
	{
		return Show(owner, message, caption, buttons, icon, defaultbutton, (MessageBoxOptions)0);
	}

	static DialogResult Show(IWin32Window^ owner, String^ message, String^ caption, MessageBoxButtons buttons, MessageBoxIcon icon)
	{
		return Show(owner, message, caption, buttons, icon, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
	}

	static DialogResult Show(IWin32Window^ owner, String^ message, String^ caption, MessageBoxButtons buttons)
	{
		return Show(owner, message, caption, buttons, MessageBoxIcon::None, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
	}

	static DialogResult Show(IWin32Window^ owner, String^ message, String^ caption)
	{
		return Show(owner, message, caption, MessageBoxButtons::OK, MessageBoxIcon::None, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
	}

	static DialogResult Show(IWin32Window^ owner, String^ message)
	{
		return Show(owner, message, "", MessageBoxButtons::OK, MessageBoxIcon::None, MessageBoxDefaultButton::Button1, (MessageBoxOptions)0);
	}
};