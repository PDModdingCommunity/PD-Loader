#pragma once
#include "IInputBinding.h"
#include "../DirectInput/Ds4/DualShock4.h"

namespace TLAC::Input
{
	class Ds4Binding : public IInputBinding
	{
	public:
		Ds4Button Button;

		Ds4Binding(Ds4Button button);
		~Ds4Binding();

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}