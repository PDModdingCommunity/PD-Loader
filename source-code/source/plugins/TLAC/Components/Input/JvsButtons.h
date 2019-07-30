#pragma once
#include <stdint.h>

namespace TLAC::Components
{
	enum JvsButtons : uint32_t
	{
		JVS_NONE		= 0 << 0x00, // 0x0

		JVS_TEST		= 1 << 0x00, // 0x1
		JVS_SERVICE		= 1 << 0x01, // 0x2

		JVS_START		= 1 << 0x02, // 0x4
		JVS_TRIANGLE	= 1 << 0x07, // 0x80
		JVS_SQUARE		= 1 << 0x08, // 0x100
		JVS_CROSS		= 1 << 0x09, // 0x200
		JVS_CIRCLE		= 1 << 0x0A, // 0x400
		JVS_L			= 1 << 0x0B, // 0x800
		JVS_R			= 1 << 0x0C, // 0x1000

		JVS_SW1			= 1 << 0x12, // 0x40000
		JVS_SW2			= 1 << 0x13, // 0x80000
	};
}