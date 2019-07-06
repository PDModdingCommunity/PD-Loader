#include "DirectInputMouse.h"

namespace TLAC::Input
{
	DirectInputMouse::DirectInputMouse()
	{
		HRESULT result = NULL;

		result = DI_CreateDevice(GUID_SysMouse);
		
		if (FAILED(result))
			return;

		result = DI_SetDataFormat(&c_dfDIMouse);
		result = DI_Acquire();
	}

	DirectInputMouse::~DirectInputMouse()
	{
		DI_Dispose();
	}

	bool DirectInputMouse::Poll()
	{
		if (!DirectInputInitialized())
			return FALSE;

		HRESULT result = NULL;

		result = DI_Poll();
		result = DI_GetDeviceState(sizeof(mouseState), &mouseState);

		return !FAILED(result);
	}
}