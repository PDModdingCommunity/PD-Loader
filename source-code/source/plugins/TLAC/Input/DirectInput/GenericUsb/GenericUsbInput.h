#pragma once
#include "../Controller.h"
#include "../../IInputDevice.h"
#include "../../../Input/KeyConfig/Config.h"
#include "GuState.h"
#include <unordered_map>

namespace TLAC::Input
{
	class GenericUsbInput : public Controller, public IInputDevice
	{
	public:
		GenericUsbInput();
		~GenericUsbInput();

		static bool TryInitializeInstance();

		bool Initialize();
		bool PollInput() override;

		bool IsDown(GuButton button);
		bool IsUp(GuButton button);
		bool IsTapped(GuButton button);
		bool IsReleased(GuButton button);
		bool WasDown(GuButton button);
		bool WasUp(GuButton button);

		inline GuJoystick GetLeftStick() { return currentState.LeftStick; };
		inline GuJoystick GetRightStick() { return currentState.RightStick; };
		inline GuJoystick GetDpad() { return currentState.Dpad.Stick; };

		static inline bool InstanceInitialized() { return instance != nullptr; };
		static inline GenericUsbInput* GetInstance() { return instance; };
		static inline void DeleteInstance() { delete instance; instance = nullptr; };

	private:
		static GenericUsbInput* instance;

		GuState lastState;
		GuState currentState;

		std::unordered_map<std::string, std::string> customKeyMapping;
		
		std::vector<std::string> buttonNames = { "GU_BUTTON1",
												 "GU_BUTTON2",
												 "GU_BUTTON3",
												 "GU_BUTTON4",
												 "GU_BUTTON5",
												 "GU_BUTTON6",
												 "GU_BUTTON7",
												 "GU_BUTTON8",
												 "GU_BUTTON9",
												 "GU_BUTTON10",
												 "GU_BUTTON11",
												 "GU_BUTTON12",
												 "GU_BUTTON13",
												 "GU_DPAD_UP",
												 "GU_DPAD_RIGHT",
												 "GU_DPAD_DOWN",
												 "GU_DPAD_LEFT",
												 "GU_L_STICK_UP",
												 "GU_L_STICK_RIGHT",
												 "GU_L_STICK_DOWN",
												 "GU_L_STICK_LEFT",
												 "GU_R_STICK_UP",
												 "GU_R_STICK_RIGHT",
												 "GU_R_STICK_DOWN",
												 "GU_R_STICK_LEFT",
												 "GU_BUTTON_MAX"
		                                       };

		const float triggerThreshold = 0.5f;
		const float joystickThreshold = 0.5f;
		const float dpadThreshold = 0.5f;
	
		inline float NormalizeTrigger(long value) { return (float)value / USHRT_MAX; };
		inline float NormalizeStick(long value) { return (float)value / USHRT_MAX * 2.0f - 1.0f; };
		inline GuJoystick NormalizeStick(long x, long y) { return GuJoystick(NormalizeStick(x), NormalizeStick(y)); };

		void UpdateInternalGuState(GuState &state);

		bool MatchesDirection(GuJoystick joystick, GuDirection directionEnum, float threshold);
		bool GetButtonState(GuState &state, GuButton button);
	};
}