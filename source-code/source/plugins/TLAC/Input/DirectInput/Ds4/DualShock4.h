#pragma once
#include "../Controller.h"
#include "../../IInputDevice.h"
#include "Ds4State.h"

namespace TLAC::Input
{
	// Controller GUIDs:
	const GUID GUID_Ds4[] = 
	{ 
		// First Generation:  {05C4054C-0000-0000-0000-504944564944}
		{ 0x05C4054C, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// Second Generation: {09CC054C-0000-0000-0000-504944564944}
		{ 0x09CC054C, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// PS5 DualSense: {0CE6054C-0000-0000-0000-504944564944}
		{ 0x0CE6054C, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// GAMO2 DIVALLER: {11140E8F-0000-0000-0000-504944564944}
		{ 0x11140E8F, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// Hori HATSUNE MIKU X: {00A50F0D-0000-0000-0000-504944564944}
		{ 0x00A50F0D, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// Hori F2nd Miniboard (Hatsune Miku Sho): {00490F0D-0000-0000-0000-504944564944}
		{ 0x00490F0D, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// Hori FT Senyou: {013C0F0D-0000-0000-0000-504944564944}
		{ 0x013C0F0D, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } },
		// ipega Controller: {01192563-0000-0000-0000-504944564944}
		{ 0x01192563, 0x0000, 0x0000, { 0x00, 0x00, 0x50, 0x49, 0x44, 0x56, 0x49, 0x44 } }
	};

	class DualShock4 : public Controller, public IInputDevice
	{
	public:
		DualShock4();
		~DualShock4();

		static bool TryInitializeInstance();

		bool Initialize();
		bool SetRawMode(bool raw);
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

		static bool rawMode; // kinda dodgy, used to set raw mode for all instances without waiting for a single one

	private:
		static DualShock4* instance;

		Ds4State lastState;
		Ds4State currentState;

		const float triggerThreshold = 0.5f;
		const float joystickThreshold = 0.5f;
		const float dpadThreshold = 0.5f;
	
		inline float NormalizeTrigger(long value) { return (float)value / UCHAR_MAX; };
		inline float NormalizeStick(long value) { return (float)value / UCHAR_MAX * 2.0f - 1.0f; };
		inline Joystick NormalizeStick(long x, long y) { return Joystick(NormalizeStick(x), NormalizeStick(y)); };

		void UpdateInternalDs4State(Ds4State &state);

		bool MatchesDirection(Joystick joystick, Direction directionEnum, float threshold);
		bool GetButtonState(Ds4State &state, Ds4Button button);
	};
}