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
		for (int diff = 0; diff < 4; diff++)
		{
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = 0;
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = 0;
		}
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

		// update score begin and end vars from game
		for (int diff = 0; diff < 4; diff++)
		{
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d0) = &ScoreCache[diff][0][0];
			*(DivaScore**)(PLAYER_DATA_ADDRESS + diff * 0x18 + 0x5d8) = &ScoreCache[diff][1000][0]; // deliberately use 1000 to get past end of cache
		}
	}

	bool ScoreSaver::checkExistingScoreValid(int pv, int difficulty, int isEx)
	{
		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.3.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.3.alltimemodifiers

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

		swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
		int clearRank = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
		int allTimeRank = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimemodifiers");
		int allTimeModifiers = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
		int allTimePercent = GetPrivateProfileIntW(section, key, 0, configPath);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
		int expectedCheck = GetPrivateProfileIntW(section, key, -1, configPath);

		return expectedCheck == calculateCheck(score, cntHitTypes[0], cntHitTypes[1], percent, combo, clearRank, allTimeRank, allTimeModifiers, allTimePercent);
	}

	int ScoreSaver::calculateCheck(int score, int cntCools, int cntFines, int percent, int combo, int clearRank, int allTimeRank, int allTimeModifiers, int allTimePercent)
	{
		int checkField = ((short)score ^ ((short)cntCools << 16) ^ ((short)cntFines) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank;
		checkField += (allTimeRank * clearRank); // first extension
		checkField ^= (allTimeModifiers * allTimePercent);
		return checkField;
	}
	
	bool ScoreSaver::hookedInitResults(void* cls)
	{
		bool result = divaInitResults(cls);
				
		int clearRank = *(int*)(RESULTS_BASE_ADDRESS + 0xe8);
		byte insurance = *(byte*)(GAME_INFO_ADDRESS + 0x14);
		
		if (insurance != 0)
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


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.3.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.3.alltimemodifiers
		WCHAR val[32]; // needs to be big enough to store five <=four digit ints (with comma separators)

		const WCHAR section[] = L"scores";

		if (pvDifficultyIsEx == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, pvDifficulty);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, pvDifficulty);

		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int oldScore = GetPrivateProfileIntW(section, key, 0, configPath);


		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
		int oldAlltimeRank = GetPrivateProfileIntW(section, key, -1, configPath);

		if (oldAlltimeRank == -1) // fallback for old scores without alltimerank
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
			oldAlltimeRank = GetPrivateProfileIntW(section, key, 0, configPath);
		}
		int allTimeRank = clearRank > oldAlltimeRank ? clearRank : oldAlltimeRank;


		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimemodifiers");
		int oldAlltimeModifiers = GetPrivateProfileIntW(section, key, -1, configPath);

		if (oldAlltimeModifiers == -1) // fallback for old scores without alltimemodifiers
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"modifier");
			oldAlltimeModifiers = GetPrivateProfileIntW(section, key, 0, configPath);
			oldAlltimeModifiers = oldAlltimeModifiers > 0 ? 1 << (oldAlltimeModifiers - 1) : 0;
		}
		int allTimeModifiers = oldAlltimeModifiers | ((clearRank > 1 && modifier > 0) ? (1 << (modifier - 1)) : 0); // use clearRank to not update if didn't clear


		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
		int oldAlltimePercent = GetPrivateProfileIntW(section, key, -1, configPath);
		if (oldAlltimePercent == -1) // fallback for old scores without alltimepercent
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
			oldAlltimePercent = GetPrivateProfileIntW(section, key, 0, configPath);
		}
		int allTimePercent = percent > oldAlltimePercent ? percent : oldAlltimePercent;

		bool oldScoreValid = true;
		if (!checkExistingScoreValid(pvNum, pvDifficulty, pvDifficultyIsEx))
		{
			oldScore = 0; // ensure new score is written
			allTimeRank = clearRank; // don't trust old all-time clear rank, use a new one
			allTimeModifiers = ((clearRank > 1 && modifier > 0) ? (1 << (modifier - 1)) : 0); // don't trust old all-time modifiers, use a new one
			allTimePercent = percent; // don't trust old all-time percent, use a new one
			oldScoreValid = false;
		}

		if (clearRank > 1 && score > oldScore)
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimemodifiers");
			swprintf(val, 32, L"%d", allTimeModifiers);
			WritePrivateProfileStringW(section, key, val, configPath);

			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
			swprintf(val, 32, L"%d", allTimePercent);
			WritePrivateProfileStringW(section, key, val, configPath);

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

			int checkField = calculateCheck(score, cntHitTypes[0], cntHitTypes[1], percent, combo, clearRank, allTimeRank, allTimeModifiers, allTimePercent);
			swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
			swprintf(val, 32, L"%d", checkField);
			WritePrivateProfileStringW(section, key, val, configPath);
		}
		else // if (oldScoreValid)
		{
			if (allTimeRank != oldAlltimeRank) // first extension
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
				swprintf(val, 32, L"%d", allTimeRank);
				WritePrivateProfileStringW(section, key, val, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
				int oldClearRank = GetPrivateProfileIntW(section, key, 0, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
				int oldCheck = GetPrivateProfileIntW(section, key, 0, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
				swprintf(val, 32, L"%d", oldCheck - (oldAlltimeRank * oldClearRank) + (allTimeRank * oldClearRank));
				WritePrivateProfileStringW(section, key, val, configPath);
			}
			if (allTimeModifiers != oldAlltimeModifiers || allTimePercent != oldAlltimePercent) // second extension
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimemodifiers");
				swprintf(val, 32, L"%d", allTimeModifiers);
				WritePrivateProfileStringW(section, key, val, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
				swprintf(val, 32, L"%d", allTimePercent);
				WritePrivateProfileStringW(section, key, val, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
				int oldCheck = GetPrivateProfileIntW(section, key, 0, configPath);

				swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
				swprintf(val, 32, L"%d", oldCheck ^ (oldAlltimeModifiers * oldAlltimePercent) ^ (allTimeModifiers * allTimePercent));
				WritePrivateProfileStringW(section, key, val, configPath);
			}
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

	ScoreSaver::DivaScore ScoreSaver::ScoreCache[4][1000][2]; // 4 difficulties * 1000 pvs * extra or not
	void ScoreSaver::UpdateSingleScoreCacheEntry(int pvNum, int diff, int exDiff)
	{
		if (pvNum < 0 || diff < 0 || exDiff < 0 || pvNum > 999 || diff > 3 || exDiff > 1)
			return;


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.3.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.3.alltimemodifiers

		const WCHAR section[] = L"scores";

		if (exDiff == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, diff);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, diff);

		
		if (checkExistingScoreValid(pvNum, diff, exDiff))
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
			int score = GetPrivateProfileIntW(section, key, 0, configPath);

			if (score > 99999999)
				score = 99999999;

			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
			int percent = GetPrivateProfileIntW(section, key, -1, configPath);

			if (percent == -1) // fallback for old scores without alltimepercent
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
				percent = GetPrivateProfileIntW(section, key, 0, configPath);
			}

			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
			int allTimeRank = GetPrivateProfileIntW(section, key, -1, configPath);

			if (allTimeRank == -1) // fallback for old scores without alltimerank
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
				allTimeRank = GetPrivateProfileIntW(section, key, -1, configPath);
			}

			swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimemodifiers");
			int modifiers = GetPrivateProfileIntW(section, key, -1, configPath);

			if (modifiers == -1) // fallback for old scores without alltimemodifiers
			{
				swprintf(key, 32, L"%ls.%ls", keyBase, L"modifier");
				modifiers = GetPrivateProfileIntW(section, key, 0, configPath);
				modifiers = modifiers > 0 ? 1 << (modifiers - 1) : 0;
			}

			DivaScore* cachedScore = &ScoreCache[diff][pvNum][exDiff];
			cachedScore->score = score;
			cachedScore->percent = percent;
			cachedScore->clearRank = allTimeRank;
			if (modifiers & 1) cachedScore->optionA = 1;
			if (modifiers & 2) cachedScore->optionB = 1;
			if (modifiers & 4) cachedScore->optionC = 1;
		}
	}

	void ScoreSaver::UpdateSingleScoreCacheRivalEntry(int pvNum, int diff, int exDiff)
	{
		if (pvNum < 0 || diff < 0 || exDiff < 0 || pvNum > 999 || diff > 3 || exDiff > 1)
			return;


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.3.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.3.alltimemodifiers

		const WCHAR section[] = L"scores";

		if (exDiff == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", pvNum, diff);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", pvNum, diff);


		swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
		int score = GetPrivateProfileIntW(section, key, 0, rival_configPath);

		if (score > 99999999)
			score = 99999999;

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimepercent");
		int percent = GetPrivateProfileIntW(section, key, -1, rival_configPath);

		if (percent == -1) // fallback for old scores without alltimepercent
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
			percent = GetPrivateProfileIntW(section, key, 0, rival_configPath);
		}

		swprintf(key, 32, L"%ls.%ls", keyBase, L"alltimerank");
		int allTimeRank = GetPrivateProfileIntW(section, key, -1, rival_configPath);

		if (allTimeRank == -1) // fallback for old scores without alltimerank
		{
			swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
			allTimeRank = GetPrivateProfileIntW(section, key, -1, rival_configPath);
		}

		DivaScore* cachedScore = &ScoreCache[diff][pvNum][exDiff];
		cachedScore->rival_clearRank = allTimeRank;
		cachedScore->rival_score = score;
		cachedScore->rival_percent = percent;
	}

	void ScoreSaver::UpdateScoreCache()
	{
		for (int pvNum = 0; pvNum < 1000; pvNum++)
		{
			for (int diff = 0; diff < 4; diff++)
			{
				for (int exDiff = 0; exDiff < 2; exDiff++)
				{
					ScoreCache[diff][pvNum][exDiff] = DivaScore(pvNum, exDiff);
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
			for (int pvNum = 0; pvNum < 1000; pvNum++)
			{
				DivaScore &scoreinfo = ScoreCache[diff][pvNum][0];
				if (scoreinfo.clearRank > 1 && scoreinfo.clearRank <= 5 && scoreinfo.exDifficulty == 0) // at least clear and no greater than perfect and not ex
				{
					counts[diff * 4 + scoreinfo.clearRank - 2] += 1;
				}
			}
		}

		// exex special case
		for (int pvNum = 0; pvNum < 1000; pvNum++)
		{
			DivaScore &scoreinfo = ScoreCache[3][pvNum][1];
			if (scoreinfo.clearRank > 1 && scoreinfo.clearRank <= 5 && scoreinfo.exDifficulty == 1) // at least clear and no greater than perfect and not ex
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
