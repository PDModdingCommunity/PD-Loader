#pragma once
#include "IInputBinding.h"
#include "../Mouse/Mouse.h"

namespace TLAC::Input
{
	enum MouseAction
	{
		MouseAction_LeftButton,
		MouseAction_RightButton,
		MouseAction_MiddleButton,
		MouseAction_ScrollUp,
		MouseAction_ScrollDown,
	};

	class MouseBinding : public IInputBinding
	{
	public:
		MouseAction Action;

		MouseBinding(MouseAction action);
		~MouseBinding();

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}