#include "SysTimer.h"
#include "../Constants.h"

namespace TLAC::Components
{
	SysTimer::SysTimer()
	{
	}

	SysTimer::~SysTimer()
	{
	}

	const char* SysTimer::GetDisplayName()
	{
		return "sys_timer";
	}

	void SysTimer::Initialize(ComponentsManager*)
	{
		selPvTime = GetSysTimePtr((void*)SEL_PV_TIME_ADDRESS);
	}

	void SysTimer::Update()
	{
		// account for the decrement that occures during this frame
		*selPvTime = SEL_PV_FREEZE_TIME * SYS_TIME_FACTOR + 1;
	}

	int* SysTimer::GetSysTimePtr(void *address)
	{
		return (int*)address;
	}
}
