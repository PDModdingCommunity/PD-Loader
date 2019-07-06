#pragma once

namespace TLAC::Components
{
	enum InputBufferType
	{
		InputBufferType_Tapped,
		InputBufferType_Released,
		InputBufferType_Down,
		InputBufferType_DoubleTapped,
		InputBufferType_IntervalTapped,
		InputBufferType_Max,
	};
}