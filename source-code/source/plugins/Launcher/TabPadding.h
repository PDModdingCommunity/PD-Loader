#pragma once
using namespace System::Windows::Forms;
using namespace System::Drawing;

// ported from https://stackoverflow.com/a/7785745
// original code by user LarsTech https://stackoverflow.com/users/719186/larstech

ref class TabPadding : public NativeWindow
{
public:
	TabPadding(TabControl^ tc);

private:
	TabControl^ tabControl;
	void tabControl_Selected(Object^ sender, TabControlEventArgs^ e);

protected:
	virtual void WndProc(Message %m) override;
};