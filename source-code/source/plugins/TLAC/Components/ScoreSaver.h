#pragma once
#include "EmulatorComponent.h"
#include "../Constants.h"
#include <windows.h>
#include <vector>
#include <thread>

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
			int per_module_equip[6];
			int other_module_shit[30]; // ??? -- seems to be 6*6 ints (0x90 bytes) for modules + 5 ints (0x14 bytes) for skin and button se
			int per_skin_equip;
			int per_btn_se_equip;
			int per_slide_se_equip;
			int per_chainslide_se_equip;
			int per_slidertouch_se_equip;
			int clearRank;        // +0xac: clear rank
			int score;            // +0xb0: score
			int percent;          // +0xb4: best percent
			byte paddingB8[0x14]; // ??? -- challenge_kind, chain_challenge_flag, chain_challenge_num, chain_challenge_max, challenge_ng_flag
			int rival_clearRank;  // +0xcc: rival clear rank?
			int rival_score;      // +0xd0: rival score
			int rival_percent;    // +0xd4: rival percent
			byte paddingD8[0x9];  // ??? -- interim_ranking int?
			byte optionA;         // +0xe1: gamemode options
			byte optionB;
			byte optionC;

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
			DivaScore()
			{
				DivaScore(0, 0);
			}
		};


		static std::vector<ScoreSaver::DivaScore> ScoreCache[4]; // * 4 difficulties

		static void UpdateScoreCache();
		static void UpdateSingleScoreCacheEntry(int pvNum, int diff, int exDiff, bool doDefaultsReset);
		static void UpdateSingleScoreCacheRivalEntry(int pvNum, int diff, int exDiff);
		static void UpdateSingleScoreCacheModulesEntry(int pvNum, int diff, int exDiff);
		static void UpdateSingleScoreCacheSkinsEntry(int pvNum, int diff, int exDiff);
		static void UpdateSingleScoreCacheSFXEntry(int pvNum, int diff, int exDiff);
		static void FixScoreCacheAddresses(int diff); // only call this from the main thread
		static DivaScore* GetCachedScore(int pvNum, int diff, int exDiff);
		static void UpdateClearCounts();
		static void ModuleCheck(int pvNum, int diff, int exDiff);

	private:
		static bool(__stdcall* divaInitResults)(void* cls);
		static bool hookedInitResults(void* cls);
		static void InjectCode(void* address, const std::vector<uint8_t> data);
		static void initCache();
		static bool checkExistingScoreValid(int pv, int difficulty, int isEx);
		static int calculateCheck(int score, int cntCools, int cntFines, int percent, int combo, int clearRank, int allTimeRank, int allTimeModifiers, int allTimePercent);

		static WCHAR configPath[256];
		static WCHAR rival_configPath[256];
		static WCHAR modules_configPath[256];
		static WCHAR skins_configPath[256];
		static WCHAR sfx_configPath[256];
		static std::thread initThread;

		int currentPv;
		int currentDifficulty;
		int currentDifficultyIsEx;
		byte currentInsurance;

		static bool initCacheFinished;
		static bool didInitialAddressUpdate;
	};
}
