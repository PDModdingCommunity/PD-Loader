#include "TouchPanelEmulator.h"
#include <iostream>
#include "../ComponentsManager.h"
#include "../../Constants.h"
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
