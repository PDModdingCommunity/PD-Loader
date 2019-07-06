#include "TabPadding.h"
//#include <Windows.h>

// ported from https://stackoverflow.com/a/7785745
// original code by user LarsTech https://stackoverflow.com/users/719186/larstech

TabPadding::TabPadding(TabControl^ tc)
{
	tabControl = tc;
	tabControl->Selected += gcnew TabControlEventHandler(this, &TabPadding::tabControl_Selected);
	AssignHandle(tc->Handle);
}

void TabPadding::tabControl_Selected(Object^ sender, TabControlEventArgs^ e)
{
	tabControl->Invalidate();
}

void TabPadding::WndProc(Message %m) {
	NativeWindow::WndProc(m);

	if (m.Msg == 15) { //WM_PAINT
		Graphics^ g = Graphics::FromHwnd(m.HWnd);

		//Replace the outside white borders:
		if (tabControl->Parent) {
			g->SetClip(System::Drawing::Rectangle(0, 0, tabControl->Width - 2, tabControl->Height - 1), Drawing2D::CombineMode::Exclude);
			g->FillRectangle(gcnew SolidBrush(tabControl->Parent->BackColor),
				System::Drawing::Rectangle(0, tabControl->ItemSize.Height + 2, tabControl->Width, tabControl->Height - (tabControl->ItemSize.Height + 2)));
		}
				

		//Replace the inside white borders:
		if (tabControl->SelectedTab) {
			g->ResetClip();
			System::Drawing::Rectangle r = tabControl->SelectedTab->Bounds;
			g->SetClip(r, Drawing2D::CombineMode::Exclude);
			g->FillRectangle(gcnew SolidBrush(tabControl->SelectedTab->BackColor),
				System::Drawing::Rectangle(r.Left - 3, r.Top - 1, r.Width + 4, r.Height + 3));
		}

		delete g;
	}
}