#pragma once
#include "DirectInputDevice.h"

namespace TLAC::Input
{
	class Controller : public DirectInputDevice
	{
	protected:
		Controller();
		~Controller();
	};
}