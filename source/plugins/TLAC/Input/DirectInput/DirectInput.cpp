#include "DirectInput.h"

namespace TLAC::Input
{
	IDirectInput8 *IDirectInputInstance = nullptr;

	HRESULT InitializeDirectInput(HMODULE module)
	{
		HRESULT result = DirectInput8Create(module, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&IDirectInputInstance, nullptr);
		return result;
	}

	bool DirectInputInitialized()
	{
		return IDirectInputInstance != nullptr;
	}

	void DisposeDirectInput()
	{
		if (IDirectInputInstance == nullptr)
			return;

		IDirectInputInstance->Release();
		IDirectInputInstance = nullptr;
	}
}