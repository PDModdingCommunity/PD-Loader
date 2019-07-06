#include "XinputBinding.h"
#include "..\Xinput\Xinput.h"

namespace TLAC::Input
{
	XinputBinding::XinputBinding(BYTE keycode) : Keycode(keycode)
	{
	}

	XinputBinding::~XinputBinding()
	{
	}

	bool XinputBinding::IsDown()
	{
		return Xinput::GetInstance()->IsDown(Keycode);
	}

	bool XinputBinding::IsTapped()
	{
		return Xinput::GetInstance()->IsTapped(Keycode);
	}

	bool XinputBinding::IsReleased()
	{
		return Xinput::GetInstance()->IsReleased(Keycode);
	}

	bool XinputBinding::IsDoubleTapped()
	{
		return Xinput::GetInstance()->IsDoubleTapped(Keycode);
	}
}