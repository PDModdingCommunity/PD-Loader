#pragma once

namespace TLAC::Input
{
	class IInputDevice
	{
	public:
		virtual bool PollInput() = 0;
	};
}
