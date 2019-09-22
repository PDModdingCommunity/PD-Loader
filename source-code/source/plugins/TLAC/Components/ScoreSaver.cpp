#include "ScoreSaver.h"
#include "../Constants.h"
#include "../framework.h"
#include "../Utilities/Operations.h"
#include <stdio.h>
#include <windows.h>
#include <detours.h>
#include <algorithm>
#pragma comment(lib, "detours.lib")

namespace TLAC::Components
{
	WCHAR ScoreSaver::configPath[256];
	WCHAR ScoreSaver::rival_configPath[256];
	ScoreSaver::ScoreSaver()
	{
		std::string utf8path = TLAC::framework::GetModuleDirectory() + "/scores.ini";
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, configPath, 256);
		utf8path = TLAC::framework::GetModuleDirectory() + "/rivalscores.ini";
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, rival_configPath, 256);
	}

	ScoreSaver::~ScoreSaver()
	{
	}

	const char* ScoreSaver::GetDisplayName()
	{
		return "score_saver";
	}

	bool(__stdcall* ScoreSaver::divaInitResults)(void* cls) = (bool(__stdcall*)(void* cls))RESULTS_INIT_ADDRESS;
	void ScoreSaver::Initialize(ComponentsManager*)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)ScoreSaver::divaInitResults, (PVOID)(ScoreSaver::hookedInitResults));
		DetourTransactionCommit();


		// build the score cache
		UpdateScoreCache();
		UpdateClearCounts();
	}

	bool ScoreSaver::checkExistingScoreValid(int pv, int difficulty, int isEx)
	{
		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.4.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.challengescore

		const WCHAR section[] = L"scores";

		if (isEx == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pv, difficulty);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pv, difficulty);


		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int score = GetPrivateProfileIntW(section, key, 0, configPath);
		
		swprintf(key, 32, L"%ls.%ls", keyBase, L"notecounts");
		WCHAR countsBufW[32];
		char countsBufA[32];
		int cntHitTypes[2] = { 0, 0 };
		GetPrivateProfileStringW(section, key, L"", countsBufW, 32, configPath);
		WideCharToMultiByte(CP_UTF8, 0, countsBufW, -1, countsBufA, 32, NULL, NULL);
		std::vector<std::string> countToks = TLAC::Utilities::Split(countsBufA, ",");
		if (countToks.size() >= 2)
		{
			cntHitTypes[0] = atoi(countToks[0].c_str());
			cntHitTypes[1] = atoi(countToks[1].c_str());
		}

		swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
		int percent = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"combo");
		int combo = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
		int allTimeRank = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
		int clearRank = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
		int expectedCheck = GetPrivateProfileIntW(section, key, -1, configPath);


		return expectedCheck == (int)(((short)score ^ ((short)cntHitTypes[0] << 16) ^ ((short)cntHitTypes[1]) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank + (allTimeRank * clearRank));
	}
	
	bool ScoreSaver::hookedInitResults(void* cls)
	{
		bool result = divaInitResults(cls);
				
		int clearRank = *(int*)(RESULTS_BASE_ADDRESS + 0xe8);
		byte insurance = *(byte*)(GAME_INFO_ADDRESS + 0x14);
		
		if (clearRank < 2 || insurance != 0)
			return result;

		// get the base for this specific set of results
		uint64_t resultBase = *(uint64_t*)(RESULTS_BASE_ADDRESS + 0x100);
		
		int pvNum = *(int*)(resultBase + 0x2c);
		int pvDifficulty = *(int*)(resultBase + 0x34);
		int pvDifficultyIsEx = *(int*)(resultBase + 0x44);
		int modifier = *(int*)(resultBase + 0x70);
		
		int* cntHitTypes = (int*)(resultBase + 0x158);
		int* pctHitTypes = (int*)(resultBase + 0x16c);
		int combo = *(int*)(resultBase + 0x180);
		int challengeScore = *(int*)(resultBase + 0x184);
		int holdScore = *(int*)(resultBase + 0x188);
		int score = *(int*)(resultBase + 0x18c);
		int percent = *(int*)(resultBase + 0x190);
		int slideScore = *(int*)(resultBase + 0x194);
		
		WCHAR songName[256];
		//std::string utf8song = *(std::string*)(CURRENT_SONG_NAME_ADDRESS);
		uint64_t songnamelen = *(uint64_t*)(CURRENT_SONG_NAME_ADDRESS + 0x18);
		char* utf8song = songnamelen < 0x10 ? (char*)CURRENT_SONG_NAME_ADDRESS : *(char**)CURRENT_SONG_NAME_ADDRESS;
		MultiByteToWideChar(CP_UTF8, 0, utf8song, -1, songName, 256);


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.4.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.challengescore
		WCHAR val[32]; // needs to be big enough to store five <=four digit ints (with comma separators)

		const WCHAR section[] = L"scores";

		if (pvDifficultyIsEx == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, pvDifficulty);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, pvDifficulty);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int oldScore = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
		int oldAlltimeRank = GetPrivateProfileIntW(section, key, 0, configPath);
		int allTimeRank = clearRank > oldAlltimeRank ? clearRank : oldAlltimeRank;

		bool oldScoreValid = true;
		if (!checkExistingScoreValid(pvNum, pvDifficulty, pvDifficultyIsEx))
		{
			oldScore = 0; // ensure new score is written
			allTimeRank = clearRank; // don't trust old all-time clear rank, use a new one
			oldScoreValid = false;
		}

		if (score > oldScore)
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
			swprintf(val, 32, L"%d", allTimeRank);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"challengescore");
			swprintf(val, 32, L"%d", challengeScore);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"combo");
			swprintf(val, 32, L"%d", combo);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"holdscore");
			swprintf(val, 32, L"%d", holdScore);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"modifier");
			swprintf(val, 32, L"%d", modifier);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"name");
			WritePrivateProfileStringW(section, key, songName, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"notecounts");
			swprintf(val, 32, L"%d,%d,%d,%d,%d", cntHitTypes[0], cntHitTypes[1], cntHitTypes[2], cntHitTypes[3], cntHitTypes[4]);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"notepercents");
			swprintf(val, 32, L"%d,%d,%d,%d,%d", pctHitTypes[0], pctHitTypes[1], pctHitTypes[2], pctHitTypes[3], pctHitTypes[4]);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
			swprintf(val, 32, L"%d", percent);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
			swprintf(val, 32, L"%d", clearRank);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
			swprintf(val, 32, L"%d", score);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"slidescore");
			swprintf(val, 32, L"%d", slideScore);
			WritePrivateProfileStringW(section, key, val, configPath);

			int checkField = ((short)score ^ ((short)cntHitTypes[0] << 16) ^ ((short)cntHitTypes[1]) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank + (allTimeRank * clearRank);
			swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
			swprintf(val, 32, L"%d", checkField);
			WritePrivateProfileStringW(section, key, val, configPath);
		}
		else if (oldScoreValid && allTimeRank > oldAlltimeRank)
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
			swprintf(val, 32, L"%d", allTimeRank);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
			int oldClearRank = GetPrivateProfileIntW(section, key, 0, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
			int oldCheck = GetPrivateProfileIntW(section, key, -1, configPath);
			
			swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
			swprintf(val, 32, L"%d", oldCheck - (oldAlltimeRank * oldClearRank) + (allTimeRank * oldClearRank));
			WritePrivateProfileStringW(section, key, val, configPath);
		}

		UpdateSingleScoreCacheEntry(pvNum, pvDifficulty, pvDifficultyIsEx);
		UpdateClearCounts();

		return result;
	}

	void ScoreSaver::Update()
	{
		return;
	}

	void ScoreSaver::UpdateInput()
	{
		return;
	}

	void ScoreSaver::InjectCode(void* address, const std::vector<uint8_t> data)
	{
		const size_t byteCount = data.size() * sizeof(uint8_t);

		DWORD oldProtect;
		VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(address, data.data(), byteCount);
		VirtualProtect(address, byteCount, oldProtect, nullptr);
	}

	std::vector<ScoreSaver::DivaScore> ScoreSaver::ScoreCache[4] = { // * 4 difficulties
		{},
		{},
		{},
		{}
	};
	ScoreSaver::DivaScore* ScoreSaver::GetCachedScore(int pvNum, int diff, int exDiff)
	{
		ScoreSaver::DivaScore* outptr = nullptr;

		if (pvNum < 0 || diff < 0 || exDiff < 0 || pvNum > 999 || diff > 3 || exDiff > 1)
			return outptr;
		
		for (DivaScore &scoreinfo : ScoreCache[diff])
		{
			if (scoreinfo.pvNum == pvNum && scoreinfo.exDifficulty == exDiff)
				return &scoreinfo;
		}

		return nullptr;
	}

	void ScoreSaver::UpdateSingleScoreCacheEntry(int pvNum, int diff, int exDiff)
	{
		if (pvNum < 0 || diff < 0 || exDiff < 0 || pvNum > 999 || diff > 3 || exDiff > 1)
			return;


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.4.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.challengescore

		const WCHAR section[] = L"scores";

		if (exDiff == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, diff);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, diff);


		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int score = GetPrivateProfileIntW(section, key, -1, configPath);

		if (score >= 0 && checkExistingScoreValid(pvNum, diff, exDiff))
		{
			if (score > 99999999)
				score = 99999999;

			swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
			int percent = GetPrivateProfileIntW(section, key, 0, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
			int allTimeRank = GetPrivateProfileIntW(section, key, -1, configPath);

			if (allTimeRank == -1) // fallback for old scores without alltimerank
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
				allTimeRank = GetPrivateProfileIntW(section, key, 0, configPath);
			}

			DivaScore* cachedScore = GetCachedScore(pvNum, diff, exDiff);
			if (cachedScore == nullptr)
			{
				ScoreCache[diff].push_back(DivaScore(pvNum, exDiff));
				cachedScore = GetCachedScore(pvNum, diff, exDiff);
			}
			cachedScore->score = score;
			cachedScore->percent = percent;
			cachedScore->clearRank = allTimeRank;
		}
		else
		{
			DivaScore* cachedScore = GetCachedScore(pvNum, diff, exDiff);
			if (cachedScore != nullptr && cachedScore->rival_score <= 0) // only remove it if no rival score is set too
				ScoreCache[diff].erase(std::remove(ScoreCache[diff].begin(), ScoreCache[diff].end(), *cachedScore), ScoreCache[diff].end());
		}

		// update begin and end vars from game
		if (ScoreCache[diff].size() > 0)
		{
			ScoreCache[diff].shrink_to_fit();
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = &*ScoreCache[diff].begin();
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = &*ScoreCache[diff].end();
		}
		else
		{
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = nullptr;
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = nullptr;
		}
	}

	void ScoreSaver::UpdateSingleScoreCacheRivalEntry(int pvNum, int diff, int exDiff)
	{
		if (pvNum < 0 || diff < 0 || exDiff < 0 || pvNum > 999 || diff > 3 || exDiff > 1)
			return;


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.4.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.challengescore

		const WCHAR section[] = L"scores";

		if (exDiff == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, diff);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, diff);


		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int score = GetPrivateProfileIntW(section, key, -1, rival_configPath);

		if (score >= 0)
		{
			if (score > 99999999)
				score = 99999999;

			swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
			int percent = GetPrivateProfileIntW(section, key, 0, rival_configPath);

			DivaScore* cachedScore = GetCachedScore(pvNum, diff, exDiff);
			if (cachedScore == nullptr)
			{
				ScoreCache[diff].push_back(DivaScore(pvNum, exDiff));
				cachedScore = GetCachedScore(pvNum, diff, exDiff);
			}
			cachedScore->rival_score = score;
			cachedScore->rival_percent = percent;
		}
		else
		{
			DivaScore* cachedScore = GetCachedScore(pvNum, diff, exDiff);
			if (cachedScore != nullptr && cachedScore->score <= 0) // only remove it if no regular score is set too
				ScoreCache[diff].erase(std::remove(ScoreCache[diff].begin(), ScoreCache[diff].end(), *cachedScore), ScoreCache[diff].end());
		}

		// update begin and end vars from game
		if (ScoreCache[diff].size() > 0)
		{
			ScoreCache[diff].shrink_to_fit();
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = &*ScoreCache[diff].begin();
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = &*ScoreCache[diff].end();
		}
		else
		{
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = nullptr;
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = nullptr;
		}
	}

	void ScoreSaver::UpdateScoreCache()
	{
		for (int pvNum = 0; pvNum < 1000; pvNum++)
		{
			for (int diff = 0; diff < 4; diff++)
			{
				for (int exDiff = 0; exDiff < 2; exDiff++)
				{
					UpdateSingleScoreCacheEntry(pvNum, diff, exDiff);
					UpdateSingleScoreCacheRivalEntry(pvNum, diff, exDiff);
				}
			}
		}
	}

	void ScoreSaver::UpdateClearCounts()
	{
		int* counts = (int*)SONG_CLEAR_COUNTS_ADDRESS;
		for (int i = 0; i < 20; i++)
		{
			counts[i] = 0;
		}

		for (int diff = 0; diff < 4; diff++)
		{
			for (DivaScore &scoreinfo : ScoreCache[diff])
			{
				if (scoreinfo.clearRank > 1 && scoreinfo.clearRank <= 5 && scoreinfo.exDifficulty == 0) // at least clear and no greater than perfect and not ex
				{
					counts[diff * 4 + scoreinfo.clearRank - 2] += 1;
				}
			}
		}

		// exex special case
		for (DivaScore &scoreinfo : ScoreCache[3])
		{
			if (scoreinfo.clearRank > 1 && scoreinfo.clearRank <= 5 && scoreinfo.exDifficulty == 1) // at least clear and no greater than perfect and IS ex
			{
				counts[4 * 4 + scoreinfo.clearRank - 2] += 1;
			}
		}

		// perfects count as clears for <= excellent, etc
		for (int diff = 0; diff < 5; diff++)
		{
			counts[diff * 4 + 2] += counts[diff * 4 + 3];
			counts[diff * 4 + 1] += counts[diff * 4 + 2];
			counts[diff * 4 + 0] += counts[diff * 4 + 1];
		}
	}
}
