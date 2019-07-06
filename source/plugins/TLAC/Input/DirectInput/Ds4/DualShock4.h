#pragma once
#include "../Controller.h"
#include "../../IInputDevice.h"
#include "Ds4State.h"

namespace TLAC::Input
{
	// DualShock 4 Wireless Controller Product GUIDs:
	const GUID GUID_Ds4[2] = 
	{ 
		// First Generation:  {05C4054C-0000-0000-0000-504944564944}
		{ 0x05C4054C, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// Second Generation: {09CC054C-0000-0000-0000-504944564944}
		{ 0x09CC054C, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
	};

	class DualShock4 : public Controller, public IInputDevice
	{
	public:
		DualShock4();
		~DualShock4();

		static bool TryInitializeInstance();

		bool Initialize();
		bool PollInput() override;

		bool IsDown(Ds4Button button);
		bool IsUp(Ds4Button button);
		bool IsTapped(Ds4Button button);
		bool IsReleased(Ds4Button button);
		bool WasDown(Ds4Button button);
		bool WasUp(Ds4Button button);

		inline Joystick GetLeftStick() { return currentState.LeftStick; };
		inline Joystick GetRightStick() { return currentState.RightStick; };
		inline Joystick GetDpad() { return currentState.Dpad.Stick; };

		static inline bool InstanceInitialized() { return instance != nullptr; };
		static inline DualShock4* GetInstance() { return instance; };
		static inline void DeleteInstance() { delete instance; instance = nullptr; };

	private:
		static DualShock4* instance;

		Ds4State lastState;
		Ds4State currentState;

		const float triggerThreshold = 0.5f;
		const float joystickThreshold = 0.5f;
		const float dpadThreshold = 0.5f;
	
		inline float NormalizeTrigger(long value) { return (float)value / USHRT_MAX; };
		inline float NormalizeStick(long value) { return (float)value / USHRT_MAX * 2.0f - 1.0f; };
		inline Joystick NormalizeStick(long x, long y) { return Joystick(NormalizeStick(x), NormalizeStick(y)); };

		void UpdateInternalDs4State(Ds4State &state);

		bool MatchesDirection(Joystick joystick, Direction directionEnum, float threshold);
		bool GetButtonState(Ds4State &state, Ds4Button button);
	};
}