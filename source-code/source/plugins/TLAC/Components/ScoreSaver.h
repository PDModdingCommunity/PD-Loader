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
			byte paddingB8[0x18]; // ???
			int rival_score;      // +0xd0: rival score
			int rival_percent;    // +0xd4: rival percent
			byte paddingD8[0x8];  // ???
			byte optionA;         // +0xe0: gamemode options
			byte optionB;
			byte optionC;
			byte paddingE3;       // total length is 0xe4

			inline bool operator==(const DivaScore& cmp)
			{
				return cmp.pvNum == pvNum && cmp.exDifficulty == exDifficulty;
			}

			DivaScore(int pv, int exDiff)
			{
				// this is just copied from 140113510
				// no clue what most of it is
				pvNum = pv;
				exDifficulty = exDiff;
				*(int*)((uint64_t)this + 0x08) = 0;
				*(int*)((uint64_t)this + 0x0c) = 0;
				*(int*)((uint64_t)this + 0x10) = 0;
				*(int*)((uint64_t)this + 0x14) = 0;
				*(int*)((uint64_t)this + 0x18) = 0;
				*(int*)((uint64_t)this + 0x1c) = 0;
				*(long long*)((uint64_t)this + 0x20) = -1;
				*(long long*)((uint64_t)this + 0x28) = -1;
				*(long long*)((uint64_t)this + 0x30) = -1;
				*(long long*)((uint64_t)this + 0x38) = -1;
				*(long long*)((uint64_t)this + 0x40) = -1;
				*(long long*)((uint64_t)this + 0x48) = -1;
				*(long long*)((uint64_t)this + 0x50) = -1;
				*(long long*)((uint64_t)this + 0x58) = -1;
				*(long long*)((uint64_t)this + 0x60) = -1;
				*(long long*)((uint64_t)this + 0x68) = -1;
				*(long long*)((uint64_t)this + 0x70) = -1;
				*(long long*)((uint64_t)this + 0x78) = -1;
				*(int*)((uint64_t)this + 0x80) = 0x01010101;
				*(int*)((uint64_t)this + 0x84) = 0x01010101;
				*(int*)((uint64_t)this + 0x88) = 0x01010101;
				*(int*)((uint64_t)this + 0x8c) = 0x01010101;
				*(int*)((uint64_t)this + 0x90) = 0x01010101;
				*(int*)((uint64_t)this + 0x94) = 0x01010101;
				*(int*)((uint64_t)this + 0x98) = 0;
				*(long long*)((uint64_t)this + 0x9c) = -1;
				*(long long*)((uint64_t)this + 0xa4) = -1;
				*(int*)((uint64_t)this + 0xac) = -1;
				*(long long*)((uint64_t)this + 0xb0) = 0;
				*(int*)((uint64_t)this + 0xb8) = -1;
				*(byte*)((uint64_t)this + 0xbc) = 0;
				*(long long*)((uint64_t)this + 0xc0) = -1;
				*(byte*)((uint64_t)this + 0xc8) = 0;
				*(int*)((uint64_t)this + 0xcc) = -1;
				*(long long*)((uint64_t)this + 0xd0) = 0;
				*(int*)((uint64_t)this + 0xd8) = -1;
				*(byte*)((uint64_t)this + 0xdc) = 0;
				*(byte*)((uint64_t)this + 0xdd) = 0;
				*(byte*)((uint64_t)this + 0xde) = 0;
				*(byte*)((uint64_t)this + 0xdf) = 0;
				*(byte*)((uint64_t)this + 0xe0) = 0;
				*(byte*)((uint64_t)this + 0xe1) = 0;
				*(byte*)((uint64_t)this + 0xe2) = 0;
				*(byte*)((uint64_t)this + 0xe3) = 0;
			}
		};


		static std::vector<ScoreSaver::DivaScore> ScoreCache[4]; // * 4 difficulties

		static void UpdateScoreCache();
		static void UpdateSingleScoreCacheEntry(int pvNum, int diff, int exDiff);
		static void UpdateSingleScoreCacheRivalEntry(int pvNum, int diff, int exDiff);
		static DivaScore* GetCachedScore(int pvNum, int diff, int exDiff);
		static void UpdateClearCounts();

	private:
		static bool(__stdcall* divaInitResults)(void* cls);
		static bool hookedInitResults(void* cls);
		static void InjectCode(void* address, const std::vector<uint8_t> data);
		static bool checkExistingScoreValid(int pv, int difficulty, int isEx);

		static WCHAR configPath[256];
		static WCHAR rival_configPath[256];
	};
}
