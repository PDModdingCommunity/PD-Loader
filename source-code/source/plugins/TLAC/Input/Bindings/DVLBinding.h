#pragma once
#include "IInputBinding.h"
#include "../Divaller/Divaller.h"

namespace TLAC::Input
{
	class DivallerBinding : public IInputBinding
	{
	public:
		DivallerButton Button;

		DivallerBinding(DivallerButton button);
		~DivallerBinding();

		bool IsDown() override;
		bool IsTapped() override;
		bool IsReleased() override;
	};
}