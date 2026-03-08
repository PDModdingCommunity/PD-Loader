#pragma once
#include "IInputBinding.h"
#include "../DirectInput/GenericUsb/GenericUsbInput.h"

namespace TLAC::Input
{
	class GuBinding : public IInputBinding
	{
	public:
		GuButton Button;

		GuBinding(GuButton button);
		~GuBinding();

		bool BypassTransferCheck() override;

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}