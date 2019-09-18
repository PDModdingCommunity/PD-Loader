#pragma once
#include "EmulatorComponent.h"
#include "../Constants.h"
#include <windows.h>
#include <vector>

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
		static void InjectCode(void* address, const std::vector<uint8_t> data);

		static WCHAR configPath[256];
		static char selHighScore[9];
		static char selHighPct1[4];
		static char selHighPct2[3];

		int currentPv;
		int currentDifficulty;
		int currentDifficultyIsEx;
	};
}
