#include "TouchPanelEmulator.h"
#include <iostream>
#include <tchar.h>
#include <tpcshrd.h>
#include "../ComponentsManager.h"
#include "../../Constants.h"
#include "../../framework.h"
#include "../../Input/Mouse/Mouse.h"
#include "../../Input/Keyboard/Keyboard.h"

using namespace TLAC::Input;

namespace TLAC::Components
{
	TouchPanelEmulator::TouchPanelEmulator()
	{
	}

	TouchPanelEmulator::~TouchPanelEmulator()
	{
	}

	const char* TouchPanelEmulator::GetDisplayName()
	{
		return "touch_panel_emulator";
	}

	void TouchPanelEmulator::Initialize(ComponentsManager* manager)
	{
		componentsManager = manager;
		state = GetTouchStatePtr((void*)TASK_TOUCH_ADDRESS);
		// Make touches on actual touchscreens more responsive
		const DWORD_PTR dwHwndTabletProperty =
			TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
			TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
			TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
			TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)
		SetProp(TLAC::framework::DivaWindowHandle, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	}

	void TouchPanelEmulator::Update()
	{
		state->ConnectionState = 1;
	}

	void TouchPanelEmulator::UpdateInput()
	{
		if (!componentsManager->GetUpdateGameInput() || componentsManager->IsDwGuiActive() || componentsManager->IsDwGuiHovered())
			return;

		// TODO: rescale TouchReaction aet position
		auto keyboard = Keyboard::GetInstance();
		auto pos = Mouse::GetInstance()->GetRelativePosition();

		state->XPosition = (float)pos.x;
		state->YPosition = (float)pos.y;

		bool down = keyboard->IsDown(VK_LBUTTON);
		bool released = keyboard->IsReleased(VK_LBUTTON);

		state->ContactType = (down ? 0x2 : released ? 0x1 : 0x0);
		state->Pressure = (float)(state->ContactType != 0);
	}

	TouchPanelState* TouchPanelEmulator::GetTouchStatePtr(void *address)
	{
		return (TouchPanelState*)address;
	}
}
