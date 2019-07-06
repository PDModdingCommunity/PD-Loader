#pragma once
#include "../EmulatorComponent.h"
#include "TouchPanelState.h"

namespace TLAC::Components
{
	class TouchPanelEmulator : public EmulatorComponent
	{
	public:
		TouchPanelEmulator();
		~TouchPanelEmulator();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

	private:
		ComponentsManager* componentsManager;

		TouchPanelState* state;
		TouchPanelState* GetTouchStatePtr(void *address);
	};
}

