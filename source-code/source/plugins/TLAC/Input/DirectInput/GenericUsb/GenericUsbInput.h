#pragma once
#include "../Controller.h"
#include "../../IInputDevice.h"
#include "GuState.h"

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