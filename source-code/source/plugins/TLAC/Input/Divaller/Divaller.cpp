#include "Divaller.h"
#include "../../framework.h"
#include "DivallerButton.h"
#include "DivallerState.h"
#include <stdio.h>
#include <string>
#include <iostream>

namespace TLAC::Input
{
	Divaller *Divaller::instance;

	Divaller::Divaller()
	{
		memset(outputBuffer, 0, sizeof(outputBuffer));
		outputBuffer[0] = 0x44;
		outputBuffer[1] = 0x4c;
		outputBuffer[2] = 0x61;
	}

	Divaller::~Divaller()
	{
		CloseHandle(hDeviceHandle);
	}

	bool Divaller::TryInitializeInstance()
	{
		if (InstanceInitialized())
			return true;

		Divaller *divaller = new Divaller();

		BOOL success = divaller->Initialize();
		instance = success ? divaller : nullptr;

		if (!success)
			delete divaller;

		return success;
	}

	bool Divaller::Initialize()
	{
		HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if (hDevInfo == INVALID_HANDLE_VALUE)
		{
			printf("[TLAC] Divaller::Initialize(): SetupDiGetClassDevs returned INVALID_HANDLE_VALUE\n");
			return false;
		}
		LPCWSTR devPath = NULL;
		for (size_t i = 0;; i++)
		{
			SP_DEVICE_INTERFACE_DATA device_interface_data = {0};
			device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
			BOOL success = SetupDiEnumDeviceInterfaces(
					hDevInfo,
					NULL,
					&GUID_DEVINTERFACE_USB_DEVICE,
					(DWORD)i,
					&device_interface_data);
			if (!success)
			{
				break;
			}
			ULONG required_length = 0;
			success = SetupDiGetDeviceInterfaceDetail(
					hDevInfo,
					&device_interface_data,
					NULL,
					0,
					&required_length,
					NULL);

			UINT8 *interface_data = (UINT8 *)calloc(required_length, sizeof(UINT8));

			PSP_DEVICE_INTERFACE_DETAIL_DATA device_interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)interface_data;
			device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

			ULONG actual_length = required_length;
			success = SetupDiGetDeviceInterfaceDetail(
					hDevInfo,
					&device_interface_data,
					device_interface_detail_data,
					actual_length,
					&required_length,
					NULL);

			if (!success)
			{
				continue;
			}
			if (
					wcsstr(device_interface_detail_data->DevicePath, L"vid_0e8f") == 0 ||
					wcsstr(device_interface_detail_data->DevicePath, L"pid_2213") == 0)
			{
				continue;
			}
			devPath = device_interface_detail_data->DevicePath;
			break;
		}
		if (devPath == NULL)
		{
			SetupDiDestroyDeviceInfoList(hDevInfo);
			return false;
		}
		hDeviceHandle = CreateFile(
				devPath,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,
				NULL);
		if (hDeviceHandle == INVALID_HANDLE_VALUE)
		{
			printf("[TLAC] Divaller::Initialize(): Open device failed.\n");
			SetupDiDestroyDeviceInfoList(hDevInfo);
			return false;
		}

		if (!WinUsb_Initialize(hDeviceHandle, &hInterfaceHandle))
		{
			printf("[TLAC] Divaller::Initialize(): WinUsb_Initialize failed.\n");
			std::cout << GetLastError() << std::endl;
			SetupDiDestroyDeviceInfoList(hDevInfo);
			CloseHandle(hDeviceHandle);
			return false;
		}

		USB_INTERFACE_DESCRIPTOR USBInterfaceDescriptor;
		if (!WinUsb_QueryInterfaceSettings(hInterfaceHandle, 0, &USBInterfaceDescriptor))
		{
			std::cout << GetLastError() << std::endl;
			SetupDiDestroyDeviceInfoList(hDevInfo);
			CloseHandle(hDeviceHandle);
			return false;
		}
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return true;
	}

	bool Divaller::PollInput()
	{
		lastState = currentState;
		ULONG len = 0;
		memset(currentState.state, 0, sizeof(currentState.state));
		BOOL bResult = WinUsb_ReadPipe(hInterfaceHandle, (UCHAR)132, currentState.state, _countof(currentState.state), &len, nullptr);
		if (!bResult || currentState.state[0] != 0x42 || currentState.state[1] != 0x56 || currentState.state[2] != 0x5a)
		{
			printf("[TLAC] Divaller::PollInput(): WinUsb_ReadPipe failed.\n");
			std::cout << GetLastError() << std::endl;
		}
		SetLED();
		return bResult;
	}

	bool Divaller::SetLED()
	{
		outputBuffer[3] &= 0x0F;															 // Reset Button LED
		outputBuffer[3] |= (!(*buttonLed >> 1 & 0x0001)) << 4; // SQUARE
		outputBuffer[3] |= (!(*buttonLed >> 3 & 0x0001)) << 5; // CIRCLE
		outputBuffer[3] |= (!(*buttonLed & 0x0001)) << 6;			 // TRIANGLE
		outputBuffer[3] |= (!(*buttonLed >> 2 & 0x0001)) << 7; // CROSS
		if (*sliderLedInit && !sliderLedData)
		{
			sliderLedData = reinterpret_cast<UCHAR *>(*(uint64_t *)(0x14cc5de40 + 0x68) + 0x189c);
		}
		if (sliderLedData)
		{
			for (int n = 0; n < 32; n++)
			{
				// LED number + 3 bytes header + color offset
				outputBuffer[(31 - n) * 3 + 3 + 3] = sliderLedData[n * 3 + 1]; // BLUE
				outputBuffer[(31 - n) * 3 + 3 + 2] = sliderLedData[n * 3 + 2]; // RED
				outputBuffer[(31 - n) * 3 + 3 + 1] = sliderLedData[n * 3 + 3]; // GREEN
			}
		}

		ULONG len = 0;
		BOOL bResult = WinUsb_WritePipe(hInterfaceHandle, (UCHAR)0x03, outputBuffer, 100, &len, NULL);
		if (!bResult)
		{
			printf("[TLAC] Divaller::SetLED(): WinUsb_WritePipe failed.\n");
			std::cout << GetLastError() << std::endl;
		}
		return bResult;
	}

	bool Divaller::IsDown(DivallerButton button)
	{
		switch (button)
		{
		case DivallerButton::DVL_L1:
			return (currentState.state[5] >> 0) & 1;
		case DivallerButton::DVL_L2:
			return (currentState.state[4] >> 7) & 1;
		case DivallerButton::DVL_L3:
			return (currentState.state[4] >> 6) & 1;
		case DivallerButton::DVL_FN:
			return (currentState.state[3] >> 1) & 1;
		case DivallerButton::DVL_TRIANGLE:
			return (currentState.state[3] >> 4) & 1;
		case DivallerButton::DVL_SQUARE:
			return (currentState.state[3] >> 2) & 1;
		case DivallerButton::DVL_CROSS:
			return (currentState.state[3] >> 5) & 1;
		case DivallerButton::DVL_CIRCLE:
			return (currentState.state[3] >> 3) & 1;
		}
		return false;
	}

	bool Divaller::IsUp(DivallerButton button)
	{
		return !IsDown(button);
	}

	bool Divaller::IsTapped(DivallerButton button)
	{
		return IsDown(button) && WasUp(button);
	}

	bool Divaller::IsReleased(DivallerButton button)
	{
		return IsUp(button) && WasDown(button);
	}

	bool Divaller::WasDown(DivallerButton button)
	{
		switch (button)
		{
		case DivallerButton::DVL_L1:
			return (lastState.state[5] >> 0) & 1;
		case DivallerButton::DVL_L2:
			return (lastState.state[4] >> 7) & 1;
		case DivallerButton::DVL_L3:
			return (lastState.state[4] >> 6) & 1;
		case DivallerButton::DVL_FN:
			return (lastState.state[3] >> 1) & 1;
		case DivallerButton::DVL_TRIANGLE:
			return (lastState.state[3] >> 4) & 1;
		case DivallerButton::DVL_SQUARE:
			return (lastState.state[3] >> 2) & 1;
		case DivallerButton::DVL_CROSS:
			return (lastState.state[3] >> 5) & 1;
		case DivallerButton::DVL_CIRCLE:
			return (lastState.state[3] >> 3) & 1;
		}
		return false;
	}

	bool Divaller::WasUp(DivallerButton button)
	{
		return !WasDown(button);
	}

	uint32_t Divaller::GetSlider()
	{
		uint32_t i = 0;
		i |= currentState.state[5] >> 4;
		i |= currentState.state[6] << 4;
		i |= currentState.state[7] << 12;
		i |= currentState.state[8] << 20;
		i |= currentState.state[9] << 28;
		// Reverse the bit order of uint32
		i = (i & 0xaaaaaaaa) >> 1 | (i & 0x55555555) << 1;
		i = (i & 0xcccccccc) >> 2 | (i & 0x33333333) << 2;
		i = (i & 0xf0f0f0f0) >> 4 | (i & 0x0f0f0f0f) << 4;
		i = (i & 0xff00ff00) >> 8 | (i & 0x00ff00ff) << 8;
		return i >> 16 | i << 16;
	};
}
