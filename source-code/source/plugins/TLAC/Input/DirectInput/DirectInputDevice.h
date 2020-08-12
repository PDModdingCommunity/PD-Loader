#pragma once
#include "DirectInput.h"

namespace TLAC::Input
{
	class DirectInputDevice
	{
	protected:
		IDirectInputDevice8 *directInputdevice;

		HRESULT DI_CreateDevice(const GUID& guid);
		HRESULT DI_SetDataFormat(LPCDIDATAFORMAT dataFormat);
		HRESULT DI_SetCooperativeLevel(HWND windowHandle, DWORD flags);
		HRESULT DI_Acquire();
		HRESULT DI_Unacquire();
		HRESULT DI_Release();
		HRESULT DI_Poll();
		HRESULT DI_GetDeviceState(DWORD size, LPVOID data);
		HRESULT DI_SetRange(LONG min, LONG max);
		HRESULT DI_SetRawMode(BOOL raw);

		void DI_Dispose();
	};
}