#pragma once

namespace TLAC::Input
{
	class IInputBinding
	{
	public:
		virtual bool IsDown() = 0;
		virtual bool IsTapped() = 0;
		virtual bool IsReleased() = 0;
	};
}
