#pragma once

namespace TLAC::Components
{
	struct TouchPanelState
	{
		int Padding00[0x1E];
		int ConnectionState;
		int Padding01[0x06];
		float XPosition;
		float YPosition;
		float Pressure;
		int ContactType;
	};
}
