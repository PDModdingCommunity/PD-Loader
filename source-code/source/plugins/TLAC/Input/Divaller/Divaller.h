#pragma once
#include "../IInputDevice.h"
#include "DivallerState.h"
#include "DivallerButton.h"
#include <SetupAPI.h>
#include <initguid.h>
#include <usbiodef.h>
#include <winusb.h>

namespace TLAC::Input
{

	class Divaller : public IInputDevice
	{
	public:
		Divaller();
		~Divaller();

		static bool TryInitializeInstance();

		bool Initialize();
		bool PollInput() override;
		bool SetLED();
		bool IsDown(DivallerButton button);
		bool IsUp(DivallerButton button);
		bool IsTapped(DivallerButton button);
		bool IsReleased(DivallerButton button);

		bool WasDown(DivallerButton button);
		bool WasUp(DivallerButton button);
		uint32_t GetSlider();
		static inline bool InstanceInitialized() { return instance != nullptr; };
		static inline Divaller *GetInstance() { return instance; };
		static inline void DeleteInstance()
		{
			delete instance;
			instance = nullptr;
		};

	private:
		static Divaller *instance;
		DivallerState lastState;
		DivallerState currentState;
		WINUSB_INTERFACE_HANDLE hInterfaceHandle;
		HANDLE hDeviceHandle;
		UCHAR outputBuffer[100];
		int *buttonLed = reinterpret_cast<int *>(0x14119b950);
		uint64_t *partionLed = reinterpret_cast<uint64_t *>(*(uint64_t *)0x140eda330 + 0xc9);
		uint64_t *sliderLedInit = reinterpret_cast<uint64_t *>((uint64_t *)0x14cc5dea8);
		UCHAR *sliderLedData = nullptr;
	};
}
