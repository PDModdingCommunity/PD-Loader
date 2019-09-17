#pragma once
#include "EmulatorComponent.h"
#include "../Constants.h"
#include <windows.h>

namespace TLAC::Components
{
	class ScoreSaver : public EmulatorComponent
	{
	public:
		ScoreSaver();
		~ScoreSaver();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

	private:
		static void(__stdcall* divaInitResults)(void* cls);
		static void hookedInitResults(void* cls);

		static WCHAR configPath[256];
	};
}
