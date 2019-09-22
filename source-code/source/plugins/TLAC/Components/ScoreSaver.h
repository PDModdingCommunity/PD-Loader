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

		struct DivaScore
		{
			int pvNum;
			int exDifficulty;
			byte padding08[0xa4]; // ???
			int clearRank;        // +0xac: clear rank
			int score;            // +0xb0: score
			int percent;          // +0xb4: best percent
			byte paddingB8[0x28]; // ???
			byte optionA;         // +0xe0: gamemode options
			byte optionB;
			byte optionC;
			byte paddingE3;       // total length is 0xe4

			inline bool operator==(const DivaScore& cmp)
			{
				return cmp.pvNum == pvNum && cmp.exDifficulty == exDifficulty;
			}
		};


		static std::vector<ScoreSaver::DivaScore> ScoreCache[4]; // * 4 difficulties

		static void UpdateScoreCache();
		static void UpdateSingleScoreCacheEntry(int pvNum, int diff, int exDiff);
		static DivaScore* GetCachedScore(int pvNum, int diff, int exDiff);
		static void UpdateClearCounts();

	private:
		static bool(__stdcall* divaInitResults)(void* cls);
		static bool hookedInitResults(void* cls);
		static void InjectCode(void* address, const std::vector<uint8_t> data);
		static bool checkExistingScoreValid(int pv, int difficulty, int isEx);

		static WCHAR configPath[256];
	};
}
