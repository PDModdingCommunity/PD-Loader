#pragma once
#include "EmulatorComponent.h"

namespace TLAC::Components
{
	class SysTimer : public EmulatorComponent
	{
		const int SYS_TIME_FACTOR = 60;
		const int SEL_PV_FREEZE_TIME = 39;

	public:
		SysTimer();
		~SysTimer();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;

	private:
		int* selPvTime;
		int* GetSysTimePtr(void *address);
	};
}

