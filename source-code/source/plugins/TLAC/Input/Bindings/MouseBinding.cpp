#include "MouseBinding.h"
#include "../Keyboard/Keyboard.h"

namespace TLAC::Input
{
	MouseBinding::MouseBinding(MouseAction action) : Action(action)
	{
	}

	MouseBinding::~MouseBinding()
	{
	}

	bool MouseBinding::IsDown()
	{
		switch (Action)
		{
		case MouseAction_LeftButton:
			return Keyboard::GetInstance()->IsDown(MK_LBUTTON);
		case MouseAction_RightButton:
			return Keyboard::GetInstance()->IsDown(MK_RBUTTON);
		case MouseAction_MiddleButton:
			return Keyboard::GetInstance()->IsDown(MK_MBUTTON);
		case MouseAction_ScrollUp:
			return Mouse::GetInstance()->GetIsScrolledUp();
		case MouseAction_ScrollDown:
			return Mouse::GetInstance()->GetIsScrolledDown();
		default:
			return false;
		}
	}

	bool MouseBinding::IsTapped()
	{
		switch (Action)
		{
		case MouseAction_LeftButton:
			return Keyboard::GetInstance()->IsTapped(MK_LBUTTON);
		case MouseAction_RightButton:
			return Keyboard::GetInstance()->IsTapped(MK_RBUTTON);
		case MouseAction_MiddleButton:
			return Keyboard::GetInstance()->IsTapped(MK_MBUTTON);
		case MouseAction_ScrollUp:
			return Mouse::GetInstance()->GetIsScrolledUp();
		case MouseAction_ScrollDown:
			return Mouse::GetInstance()->GetIsScrolledDown();
		default:
			return false;
		}
	}

	bool MouseBinding::IsReleased()
	{
		switch (Action)
		{
		case MouseAction_LeftButton:
			return Keyboard::GetInstance()->IsReleased(MK_LBUTTON);
		case MouseAction_RightButton:
			return Keyboard::GetInstance()->IsReleased(MK_RBUTTON);
		case MouseAction_MiddleButton:
			return Keyboard::GetInstance()->IsReleased(MK_MBUTTON);
		case MouseAction_ScrollUp:
			return Mouse::GetInstance()->GetWasScrolledUp();
		case MouseAction_ScrollDown:
			return Mouse::GetInstance()->GetWasScrolledDown();
		default:
			return false;
		}
	}
}
