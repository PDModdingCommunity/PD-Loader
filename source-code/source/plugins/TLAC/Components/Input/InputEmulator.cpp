#include <iostream>
#include "windows.h"
#include "InputEmulator.h"
#include "../ComponentsManager.h"
#include "../../Constants.h"
#include "../../framework.h"
#include "../../Input/Bindings/KeyboardBinding.h"
#include "../../Input/Bindings/XinputBinding.h"
#include "../../Input/Bindings/MouseBinding.h"
#include "../../Input/Bindings/Ds4Binding.h"
#include "../../Input/KeyConfig/Config.h"
#include "../../Utilities/Operations.h"
#include "../../Utilities/EnumBitwiseOperations.h"
#include "../../FileSystem/ConfigFile.h"
#include "../GameState.h"
#include "../Pause.h"
#include "../../Utils.h"

const std::string KEY_CONFIG_FILE_NAME = "keyconfig.ini";

using namespace TLAC::Input;
using namespace TLAC::Input::KeyConfig;
using namespace TLAC::Utilities;

namespace TLAC::Components
{

	InputEmulator::InputEmulator()
	{

	}

	InputEmulator::~InputEmulator()
	{
		delete TestBinding;
		delete ServiceBinding;

		delete StartBinding;
		delete SankakuBinding;
		delete ShikakuBinding;
		delete BatsuBinding;
		delete MaruBinding;

		delete LeftBinding;
		delete RightBinding;

		delete MenuLBinding;
		delete MenuRBinding;
		delete MenuCircleBinding;
	}

	const char* InputEmulator::GetDisplayName()
	{
		return "input_emulator";
	}

	void InputEmulator::Initialize(ComponentsManager* manager)
	{
		componentsManager = manager;
		componentsManager->SetIsInputEmulatorUsed(true);

		inputState = GetInputStatePtr((void*)INPUT_STATE_PTR_ADDRESS);
		inputState->HideCursor();

		TestBinding = new Binding();
		ServiceBinding = new Binding();
		Sw1Binding = new Binding();
		Sw2Binding = new Binding();
		StartBinding = new Binding();

		SankakuBinding = new Binding();
		ShikakuBinding = new Binding();
		BatsuBinding = new Binding();
		MaruBinding = new Binding();

		LeftBinding = new Binding();
		RightBinding = new Binding();

		MenuLBinding = new Binding();
		MenuRBinding = new Binding();

		MenuCircleBinding = new Binding();

		CoinBinding = new Binding();
		ToonBinding = new Binding();

		FileSystem::ConfigFile configFile(framework::GetModuleDirectory(), KEY_CONFIG_FILE_NAME);
		configFile.OpenRead();

		Config::BindConfigKeys(configFile.ConfigMap, "JVS_TEST", *TestBinding, { "F1" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_SERVICE", *ServiceBinding, { "F2" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_SW1", *Sw1Binding, { "F11" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_SW2", *Sw2Binding, { "F12" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_START", *StartBinding, { "Enter" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_TRIANGLE", *SankakuBinding, { "W", "I" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_SQUARE", *ShikakuBinding, { "A", "J" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_CROSS", *BatsuBinding, { "S", "K" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_CIRCLE", *MaruBinding, { "D", "L" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_LEFT", *LeftBinding, { "Q", "U" });
		Config::BindConfigKeys(configFile.ConfigMap, "JVS_RIGHT", *RightBinding, { "E", "O" });
		Config::BindConfigKeys(configFile.ConfigMap, "MENU_L", *MenuLBinding, { "Left", "Up" });
		Config::BindConfigKeys(configFile.ConfigMap, "MENU_R", *MenuRBinding, { "Down", "Right" });
		Config::BindConfigKeys(configFile.ConfigMap, "MENU_CIRCLE", *MenuCircleBinding, { "D", "L", "Spacebar" });
		Config::BindConfigKeys(configFile.ConfigMap, "TOON", *ToonBinding, { "F9" });
		Config::BindConfigKeys(configFile.ConfigMap, "COIN", *CoinBinding, { "F10" });

		mouseScrollPvSelection = configFile.GetBooleanValue("mouse_scroll_pv_selection");
	}

	void InputEmulator::Update()
	{
		return;
	}

	void InputEmulator::OnFocusLost()
	{
		// to prevent buttons from being "stuck"
		inputState->ClearState();
		inputState->HideCursor();
	}

	void InputEmulator::UpdateInput()
	{
		if (!componentsManager->GetUpdateGameInput())
			return;

		if (!componentsManager->IsDwGuiActive())
		{
			UpdateJvsInput();

			if (mouseScrollPvSelection && !componentsManager->IsDwGuiHovered())
				UpdateMousePvScroll();
		}

		UpdateDwGuiInput();
	}

	void InputEmulator::UpdateJvsInput()
	{
		auto tappedFunc = [](void* binding) { return ((Binding*)binding)->AnyTapped(); };
		auto releasedFunc = [](void* binding) { return ((Binding*)binding)->AnyReleased(); };
		auto downFunc = [](void* binding) { return ((Binding*)binding)->AnyDown(); };

		lastDownState = inputState->Down.Buttons;

		inputState->Tapped.Buttons = GetJvsButtonsState(tappedFunc);
		inputState->Released.Buttons = GetJvsButtonsState(releasedFunc);
		inputState->Down.Buttons = GetJvsButtonsState(downFunc);
		inputState->DoubleTapped.Buttons = GetJvsButtonsState(tappedFunc);
		inputState->IntervalTapped.Buttons = GetJvsButtonsState(tappedFunc);

		// if not interacting with L and R, use values from the slider instead
		if ( ( (inputState->Tapped.Buttons | inputState->Released.Buttons | inputState->Down.Buttons)
			 & (JVS_L | JVS_R) ) == 0)
		{
			UpdateSliderLR();
		}

		SetMetaButtons();

		UpdateHoldState();
		heldButtons = GetButtonFromHold();

		if ((lastDownState &= inputState->Tapped.Buttons) != 0)
		{
			inputState->Down.Buttons ^= inputState->Tapped.Buttons;
			if (IsHold() && !TargetInspector::IsAnyRepress())
				inputState->Down.Buttons |= heldButtons;
		}

		// repress held down buttons to not block input
		//inputState->Down.Buttons ^= inputState->Tapped.Buttons;

		if (CoinBinding->AnyTapped())
			addCoin();

		if (ToonBinding->AnyTapped())
			toggleNpr1();
	}

	HoldState InputEmulator::GetHoldState()
	{
		return (HoldState) * ((int*)HOLD_STATE_ADDRESS);
	}

	int InputEmulator::GetMaxHoldState()
	{
		return *(int*)MAX_HOLD_STATE_ADDRESS;
	}

	bool InputEmulator::IsHold()
	{
		return ((holdState != HOLD_NONE) && (GetMaxHoldState() != 1));
	}

	void InputEmulator::UpdateHoldState()
	{
		holdState = GetHoldState();

		for (int i = 0; i < 4; ++i)
		{
			int holdId = 1 << (i + 6);

			if ((holdId & holdState) != 0)
				holdTbl[i] = 1;
			else
				holdTbl[i] = 0;
		}
	}

	void InputEmulator::UpdateDwGuiInput()
	{
		auto keyboard = Keyboard::GetInstance();
		auto mouse = Mouse::GetInstance();

		auto pos = mouse->GetRelativePosition();
		inputState->MouseX = (int)pos.x;
		inputState->MouseY = (int)pos.y;

		auto deltaPos = mouse->GetDeltaPosition();
		inputState->MouseDeltaX = (int)deltaPos.x;
		inputState->MouseDeltaY = (int)deltaPos.y;

		inputState->Key = GetKeyState();

		for (int i = 0; i < sizeof(keyBits) / sizeof(KeyBit); i++)
			UpdateInputBit(keyBits[i].Bit, keyBits[i].KeyCode);

		for (int i = InputBufferType_Tapped; i < InputBufferType_Max; i++)
		{
			inputState->SetBit(scrollUpBit, mouse->GetIsScrolledUp(), (InputBufferType)i);
			inputState->SetBit(scrollDownBit, mouse->GetIsScrolledDown(), (InputBufferType)i);
		}
	}

	void InputEmulator::UpdateMousePvScroll()
	{
		// I originally wanted to use a MouseBinding set to JVS_LEFT / JVS_RIGHT
		// but that ended up being too slow because a PV slot can only be scrolled to once the scroll animation has finished playing
		int* slotsToScroll = (int*)PV_SEL_SLOTS_TO_SCROLL;
		int* modulesToScroll = (int*)MODULE_SEL_SLOTS_TO_SCROLL;

		auto mouse = Mouse::GetInstance();
		if (mouse->GetIsScrolledUp())
		{
			if (*(int*)PV_SEL_SLOTS_CONST < 26) *slotsToScroll -= 1;
			if (*(int*)MODULE_IS_RECOMMENDED == 0) *modulesToScroll -= 1;
		}
		if (mouse->GetIsScrolledDown())
		{
			if (*(int*)PV_SEL_SLOTS_CONST < 26) *slotsToScroll += 1;
			if (*(int*)MODULE_IS_RECOMMENDED == 0) *modulesToScroll += 1;
		}
	}

	InputState* InputEmulator::GetInputStatePtr(void* address)
	{
		return (InputState*)(*(uint64_t*)address);
	}

	JvsButtons InputEmulator::GetJvsButtonsState(bool(*buttonTestFunc)(void*))
	{
		JvsButtons buttons = JVS_NONE;

		if ((Pause::pause || !(*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN)) &&
			buttonTestFunc(MenuLBinding))
		{
			buttons |= JVS_L;
			return buttons;
		}
		if ((Pause::pause || !(*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN)) &&
			buttonTestFunc(MenuRBinding))
		{
			buttons |= JVS_R;
			return buttons;
		}
		if ((Pause::pause || !(*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN)) &&
			buttonTestFunc(MenuCircleBinding))
		{
			buttons |= JVS_CIRCLE;
			return buttons;
		}

		if (buttonTestFunc(TestBinding))
			buttons |= JVS_TEST;
		if (buttonTestFunc(ServiceBinding))
			buttons |= JVS_SERVICE;
		if (buttonTestFunc(Sw1Binding))
			buttons |= JVS_SW1;
		if (buttonTestFunc(Sw2Binding))
			buttons |= JVS_SW2;

		if (buttonTestFunc(StartBinding))
			buttons |= JVS_START;

		if (buttonTestFunc(SankakuBinding))
			buttons |= JVS_TRIANGLE;
		if (buttonTestFunc(ShikakuBinding))
			buttons |= JVS_SQUARE;
		if (buttonTestFunc(BatsuBinding))
			buttons |= JVS_CROSS;
		if (buttonTestFunc(MaruBinding))
			buttons |= JVS_CIRCLE;

		if (buttonTestFunc(LeftBinding))
			buttons |= JVS_L;
		if (buttonTestFunc(RightBinding))
			buttons |= JVS_R;

		return buttons;
	}

	JvsButtons InputEmulator::GetButtonFromHold()
	{
		JvsButtons buttons = JVS_NONE;

		if (holdTbl[0])
			buttons |= JVS_TRIANGLE;
		if (holdTbl[1])
			buttons |= JVS_CIRCLE;
		if (holdTbl[2])
			buttons |= JVS_CROSS;
		if (holdTbl[3])
			buttons |= JVS_SQUARE;

		return buttons;
	}

	char InputEmulator::GetKeyState()
	{
		auto keyboard = Keyboard::GetInstance();

		bool upper = keyboard->IsDown(VK_SHIFT);
		constexpr char caseDifference = 'A' - 'a';

		char inputKey = 0x00;

		for (char key = '0'; key < 'Z'; key++)
		{
			if (keyboard->IsIntervalTapped(key))
				inputKey = (upper || key < 'A') ? key : (key - caseDifference);
		}

		if (keyboard->IsIntervalTapped(VK_BACK))
			inputKey = 0x08;

		if (keyboard->IsIntervalTapped(VK_TAB))
			inputKey = 0x09;

		if (keyboard->IsIntervalTapped(VK_SPACE))
			inputKey = 0x20;

		if (keyboard->IsDoubleTapped(VK_ESCAPE))
			*(bool*)SHOULD_EXIT_BOOL_ADDRESS = true;

		return inputKey;
	}

	void InputEmulator::UpdateInputBit(uint32_t bit, uint8_t keycode)
	{
		auto keyboard = Keyboard::GetInstance();

		inputState->SetBit(bit, keyboard->IsTapped(keycode), InputBufferType_Tapped);
		inputState->SetBit(bit, keyboard->IsReleased(keycode), InputBufferType_Released);
		inputState->SetBit(bit, keyboard->IsDown(keycode), InputBufferType_Down);
		inputState->SetBit(bit, keyboard->IsDoubleTapped(keycode), InputBufferType_DoubleTapped);
		inputState->SetBit(bit, keyboard->IsIntervalTapped(keycode), InputBufferType_IntervalTapped);

	}

	typedef uint8_t getSliderSensorFunc(void*, int);
	void InputEmulator::UpdateSliderLR()
	{
		getSliderSensorFunc* getSliderSensorTapped = (getSliderSensorFunc*)GET_SLIDER_TAPPED_ADDRESS;
		getSliderSensorFunc* getSliderSensorReleased = (getSliderSensorFunc*)GET_SLIDER_RELEASED_ADDRESS;
		getSliderSensorFunc* getSliderSensorDown = (getSliderSensorFunc*)GET_SLIDER_DOWN_ADDRESS;

		if (getSliderSensorTapped((void*)SLIDER_CTRL_TASK_ADDRESS, 36))
			inputState->Tapped.Buttons |= JVS_L;
		else
			inputState->Tapped.Buttons &= ~JVS_L;

		if (getSliderSensorReleased((void*)SLIDER_CTRL_TASK_ADDRESS, 36))
			inputState->Released.Buttons |= JVS_L;
		else
			inputState->Released.Buttons &= ~JVS_L;
		
		if (getSliderSensorDown((void*)SLIDER_CTRL_TASK_ADDRESS, 36))
			inputState->Down.Buttons |= JVS_L;
		else
			inputState->Down.Buttons &= ~JVS_L;


		if (getSliderSensorTapped((void*)SLIDER_CTRL_TASK_ADDRESS, 37))
			inputState->Tapped.Buttons |= JVS_R;
		else
			inputState->Tapped.Buttons &= ~JVS_R;

		if (getSliderSensorReleased((void*)SLIDER_CTRL_TASK_ADDRESS, 37))
			inputState->Released.Buttons |= JVS_R;
		else
			inputState->Released.Buttons &= ~JVS_R;

		if (getSliderSensorDown((void*)SLIDER_CTRL_TASK_ADDRESS, 37))
			inputState->Down.Buttons |= JVS_R;
		else
			inputState->Down.Buttons &= ~JVS_R;
	}

	void InputEmulator::SetMetaButtons()
	{
		// bit 0x6e is used to skip a bunch of screens
		if ((inputState->Down.Buttons & (JVS_L | JVS_R)) == 0) // ask sega, okay? idk why this is needed
		{
			if ((inputState->Tapped.Buttons & (JVS_START | JVS_TRIANGLE | JVS_SQUARE | JVS_CROSS | JVS_CIRCLE)) != 0)
				inputState->SetBit(0x6e, true, InputBufferType_Tapped);
			else
				inputState->SetBit(0x6e, false, InputBufferType_Tapped);

			if ((inputState->Down.Buttons & (JVS_START | JVS_TRIANGLE | JVS_SQUARE | JVS_CROSS | JVS_CIRCLE)) != 0)
				inputState->SetBit(0x6e, true, InputBufferType_Down);
			else
				inputState->SetBit(0x6e, false, InputBufferType_Down);

			if ((inputState->Released.Buttons & (JVS_START | JVS_TRIANGLE | JVS_SQUARE | JVS_CROSS | JVS_CIRCLE)) != 0)
				inputState->SetBit(0x6e, true, InputBufferType_Released);
			else
				inputState->SetBit(0x6e, false, InputBufferType_Released);
		}
	}

	void InputEmulator::addCoin()
	{
		printf("[TLAC] Adding coin\n");
		unsigned char* credits = (unsigned char*)0x14CD93788;
		if (*credits < 9)
		{
			*(unsigned char*)0x14CD93A9C = 1;
			(*credits)++;
			(*(unsigned char*)0x14CD93A98)++;
			if ((*(unsigned char*)0x14CD93A98) == 0) (*(unsigned char*)0x14CD93A98)++;
		}
	}

	void InputEmulator::toggleNpr1()
	{
		switch(*(byte*)0x0000000140502FC6)
		{
		case 0xC3: // default -> force on
			InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
			InjectCode((void*)0x0000000140502FC6, { 0x01, 0x00, 0x00, 0x00, 0xC3 });
			*(byte*)0x00000001411AD638 = 1;

			printf("[TLAC] NPR1 forced\n");
			break;
		case 0x01: // default -> force off
			InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
			InjectCode((void*)0x0000000140502FC6, { 0x00, 0x00, 0x00, 0x00, 0xC3 });
			*(byte*)0x00000001411AD638 = 0;

			printf("[TLAC] NPR1 disabled\n");
			break;
		default: // force off -> default
			InjectCode((void*)0x0000000140502FC0, { 0x89, 0x0D, 0x72 });
			InjectCode((void*)0x0000000140502FC6, { 0xC3, 0xCC, 0xCC, 0xCC, 0xCC });

			printf("[TLAC] NPR1 restored\n");
		}
	}
}