#pragma once
#include <functional>
#include <Windows.h>
#include "InputState.h"
#include "../EmulatorComponent.h"
#include "../../Input/Bindings/Binding.h"
#include "../GameTargets/TargetTypes.h"
#include "../GameTargets/TargetHitStates.h"
#include "../GameTargets/HoldState.h"
#include "../GameTargets/TargetInspector.h"

namespace TLAC::Components
{
	struct KeyBit
	{
		uint32_t Bit;
		uint8_t KeyCode;
	};

	class InputEmulator : public EmulatorComponent
	{
	public:
		Input::Binding* TestBinding;
		Input::Binding* ServiceBinding;
		Input::Binding* Sw1Binding;
		Input::Binding* Sw2Binding;

		Input::Binding* StartBinding;
		Input::Binding* SankakuBinding;
		Input::Binding* ShikakuBinding;
		Input::Binding* BatsuBinding;
		Input::Binding* MaruBinding;

		Input::Binding* LeftBinding;
		Input::Binding* RightBinding;

		Input::Binding* MenuLBinding;
		Input::Binding* MenuRBinding;
		Input::Binding* MenuCircleBinding;

		Input::Binding* CoinBinding;
		Input::Binding* ToonBinding;

		InputEmulator();
		~InputEmulator();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

		virtual void OnFocusLost() override;

	private:
		ComponentsManager* componentsManager;

		bool mouseScrollPvSelection = false;
		const uint32_t scrollUpBit = 99;
		const uint32_t scrollDownBit = 100;

		KeyBit keyBits[20] =
		{
			{ 5, VK_LEFT },
			{ 6, VK_RIGHT },

			{ 29, VK_SPACE },
			{ 39, 'A' },
			{ 43, 'E' },
			{ 42, 'D' },
			{ 55, 'Q' },
			{ 57, 'S' }, // unsure
			{ 61, 'W' },
			{ 63, 'Y' },
			{ 84, 'L' }, // unsure

			{ 80, VK_RETURN },
			{ 81, VK_SHIFT },
			{ 82, VK_CONTROL },
			{ 83, VK_MENU },

			{ 91, VK_UP },
			{ 93, VK_DOWN },

			{ 96, MK_LBUTTON },
			{ 97, VK_MBUTTON },
			{ 98, MK_RBUTTON },
		};

		InputState* inputState;
		JvsButtons lastDownState;
		JvsButtons heldButtons;
		
		int holdTbl[4];
		HoldState holdState;

		void UpdateJvsInput();
		void UpdateDwGuiInput();
		void UpdateMousePvScroll();
		void UpdateHoldState();
		InputState* GetInputStatePtr(void* address);
		JvsButtons GetJvsButtonsState(bool(*buttonTestFunc)(void*));
		JvsButtons GetButtonFromHold();
		char GetKeyState();
		HoldState GetHoldState();
		int GetMaxHoldState();
		bool IsHold();

		void UpdateInputBit(uint32_t bit, uint8_t keycode);
		void UpdateSliderLR();
		void SetMetaButtons();

		void addCoin();
		void toggleNpr1();
	};
}
