#include "DirectInputDevice.h"

namespace TLAC::Input
{
	HRESULT DirectInputDevice::DI_CreateDevice(const GUID &guid)
	{
		if (!DirectInputInitialized())
			return DIERR_NOTINITIALIZED;

		HRESULT result = IDirectInputInstance->CreateDevice(guid, &directInputdevice, NULL);
		return result;
	}

	HRESULT DirectInputDevice::DI_SetDataFormat(LPCDIDATAFORMAT dataFormat)
	{
		HRESULT result = directInputdevice->SetDataFormat(dataFormat);
		return result;
	}

	HRESULT DirectInputDevice::DI_SetCooperativeLevel(HWND windowHandle, DWORD flags)
	{
		HRESULT result = directInputdevice->SetCooperativeLevel(windowHandle, flags);
		return result;
	}

	HRESULT DirectInputDevice::DI_Acquire()
	{
		HRESULT result = directInputdevice->Acquire();
		return result;
	}

	HRESULT DirectInputDevice::DI_Unacquire()
	{
		HRESULT result = directInputdevice->Unacquire();
		return result;
	}

	HRESULT DirectInputDevice::DI_Release()
	{
		HRESULT result = directInputdevice->Release();
		return result;
	}

	HRESULT DirectInputDevice::DI_Poll()
	{
		HRESULT result = directInputdevice->Poll();
		return result;
	}

	HRESULT DirectInputDevice::DI_GetDeviceState(DWORD size, LPVOID data)
	{
		HRESULT result = directInputdevice->GetDeviceState(size, data);
		return result;
	}

	HRESULT DirectInputDevice::DI_SetRange(LONG min, LONG max)
	{
		DIPROPRANGE propData;
		propData.lMin = min;
		propData.lMax = max;

		propData.diph.dwSize = sizeof(DIPROPRANGE);
		propData.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		propData.diph.dwHow = DIPH_DEVICE;
		propData.diph.dwObj = 0;

		HRESULT result = directInputdevice->SetProperty(DIPROP_RANGE, &propData.diph);
		return result;
	}

	HRESULT DirectInputDevice::DI_SetRawMode(BOOL raw)
	{
		// extra unacquire/acquire logic so this can be called on an active device
		BOOL wasAcquired = DI_Unacquire() == DI_OK;

		DIPROPDWORD propData;
		propData.dwData = raw ? DIPROPCALIBRATIONMODE_RAW : DIPROPCALIBRATIONMODE_COOKED;

		propData.diph.dwSize = sizeof(DIPROPDWORD);
		propData.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		propData.diph.dwHow = DIPH_DEVICE;
		propData.diph.dwObj = 0;

		HRESULT result = NULL;

		if (FAILED(result = directInputdevice->SetProperty(DIPROP_CALIBRATIONMODE, &propData.diph)))
			return result;

		if (wasAcquired)
			result = DI_Acquire();

		return result;
	}

	void DirectInputDevice::DI_Dispose()
	{
		if (directInputdevice == nullptr)
			return;

		HRESULT result = NULL;

		result = DI_Unacquire();
		result = DI_Release();
	}
}