#include "ScoreSaver.h"
#include "../Constants.h"
#include "../framework.h"
#include "../Utilities/Operations.h"
#include <stdio.h>
#include <windows.h>
#include <detours.h>
#pragma comment(lib, "detours.lib")

namespace TLAC::Components
{
	static char emptySelHighScore[12] = "--------(-)";
	static char emptySelHighPct1[4] = "---";
	static char emptySelHighPct2[3] = "--";
	static char protectedSelHighScore[12] = "-PROTECTED-";
	static char protectedSelHighPct1[4] = "---";
	static char protectedSelHighPct2[3] = "--";
	char ScoreSaver::selHighScore[12];
	char ScoreSaver::selHighPct1[4];
	char ScoreSaver::selHighPct2[3];

	WCHAR ScoreSaver::configPath[256];
	ScoreSaver::ScoreSaver()
	{
		strcpy_s(selHighScore, sizeof(selHighScore), emptySelHighScore);
		strcpy_s(selHighPct1, sizeof(selHighPct1), emptySelHighPct1);
		strcpy_s(selHighPct2, sizeof(selHighPct2), emptySelHighPct2);

		std::string utf8path = TLAC::framework::GetModuleDirectory() + "/scores.ini";
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, configPath, 256);
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
		// replace code that loads personal high scores on pv select screen with code that loads custom strings

		InjectCode((void*)0x1405bfb23, { // MOV  R9, &ScoreSaver::selHighScore
			0x49,
			0xb9,
			(uint8_t)((uint64_t)&selHighScore & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 8) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 16) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 24) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 32) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 40) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 48) & 0xFF),
			(uint8_t)(((uint64_t)&selHighScore >> 56) & 0xFF),
		});
		InjectCode((void*)0x1405bfb2d, { 0x4c, 0x8d, 0x05, 0x28, 0x5e, 0x3e, 0x00 }); // LEA  R8, [0x1409a595c]
		InjectCode((void*)0x1405bfb34, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

		InjectCode((void*)0x1405bfc42, { // MOV  RDI, &selHighPct1
			0x48,
			0xbf,
			(uint8_t)((uint64_t)&selHighPct1 & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 8) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 16) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 24) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 32) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 40) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 48) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct1 >> 56) & 0xFF),
		});
		InjectCode((void*)0x1405bfc4c, { 0xeb, 0x09 }); // JMP  0x1405bfc57
		InjectCode((void*)0x1405bfc69, { 0x4c, 0x8b, 0xcf }); // MOV  R9, RDI
		InjectCode((void*)0x1405bfc6c, { 0x4c, 0x8d, 0x05, 0xe9, 0x5c, 0x3e, 0x00 }); // LEA  R8, [0x1409a595c]

		InjectCode((void*)0x1405bfd98, { // MOV  RSI, &selHighPct2
			0x48,
			0xbe,
			(uint8_t)((uint64_t)&selHighPct2 & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 8) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 16) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 24) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 32) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 40) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 48) & 0xFF),
			(uint8_t)(((uint64_t)&selHighPct2 >> 56) & 0xFF),
		});
		InjectCode((void*)0x1405bfda2, { 0xeb, 0x0c }); // JMP  0x1405bfdb0
		InjectCode((void*)0x1405bfdc2, { 0x4c, 0x8b, 0xce }); // MOV  R9, RSI
		InjectCode((void*)0x1405bfdc5, { 0x4c, 0x8d, 0x05, 0x90, 0x5b, 0x3e, 0x00 }); // LEA  R8, [0x1409a595c]

		// override font to song name font for larger character set
		InjectCode((void*)0x1405bfa9d, { 0x11 });
		//InjectCode((void*)0x1405bfb80, { 0x11 });
		//InjectCode((void*)0x1405bfb9c, { 0x11 });

		// move score text upwards because of shifted size
		// WARNING: HARDCODED POSITION
		InjectCode((void*)0x1405bfab4, { 0x48, 0xb8,  0x00, 0x00, 0x95, 0x44, 0x00, 0x00, 0x0e, 0x44 }); // MOV  RAX, 0x440f400044950000
		InjectCode((void*)0x1405bfabe, { 0x48, 0x89, 0x44, 0x25, 0xb4 }); // MOV  qword ptr [RBP + -0x4c], RAX
		InjectCode((void*)0x1405bfac3, { 0x90, 0x90, 0x90, 0x90, 0x90 });


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
		int pvNum = *(int*)SELPV_CURRENT_SONG_ADDRESS;
		int diff = *(int*)(GAME_INFO_ADDRESS);
		int diffIsEx = *(int*)(GAME_INFO_ADDRESS + 0x4);
		byte insurance = *(byte*)(GAME_INFO_ADDRESS + 0x14);

		if (pvNum == currentPv && diff == currentDifficulty && diffIsEx == currentDifficultyIsEx && insurance == currentInsurance)
			return;
		
		currentPv = pvNum;
		currentDifficulty = diff;
		currentDifficultyIsEx = diffIsEx;
		currentInsurance = insurance;

		if (insurance != 0)
		{
			strcpy_s(selHighScore, sizeof(selHighScore), protectedSelHighScore);
			strcpy_s(selHighPct1, sizeof(selHighPct1), protectedSelHighPct1);
			strcpy_s(selHighPct2, sizeof(selHighPct2), protectedSelHighPct2);
			return;
		}

		if (currentPv < 0 || currentDifficulty < 0 || currentDifficultyIsEx < 0 || currentPv > 999 || currentDifficulty > 3 || currentDifficultyIsEx > 1)
			return;

		CachedScoreInfo info = ScoreCache[currentPv][currentDifficulty][currentDifficultyIsEx];

		if (info.score < 0)
		{
			strcpy_s(selHighScore, sizeof(selHighScore), emptySelHighScore);
			strcpy_s(selHighPct1, sizeof(selHighPct1), emptySelHighPct1);
			strcpy_s(selHighPct2, sizeof(selHighPct2), emptySelHighPct2);
		}
		else
		{
			if (info.score > 99999999)
				info.score = 99999999;

			char allTimeRankLetter[2] = "-";
			switch (info.rank)
			{
			case 0: // misstake
				allTimeRankLetter[0] = '-';
				break;
			case 1: // cheap
				allTimeRankLetter[0] = '-';
				break;
			case 2: // clear
				allTimeRankLetter[0] = 'C';
				break;
			case 3: // great
				allTimeRankLetter[0] = 'G';
				break;
			case 4: // excellent
				allTimeRankLetter[0] = 'E';
				break;
			case 5: // perfect
				allTimeRankLetter[0] = 'P';
				break;
			default:
				allTimeRankLetter[0] = '-';
				break;
			}

			snprintf(selHighScore, sizeof(selHighScore), "%d(%s)", info.score, allTimeRankLetter);
			snprintf(selHighPct1, sizeof(selHighPct1), "%d", info.percent / 100);
			snprintf(selHighPct2, sizeof(selHighPct2), "%02d", info.percent % 100);
		}
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

	ScoreSaver::CachedScoreInfo ScoreSaver::ScoreCache[1000][4][2];
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

			ScoreCache[pvNum][diff][exDiff] = { score, percent, allTimeRank };
		}
		else
		{
			ScoreCache[pvNum][diff][exDiff] = { -1, -1, -1 };
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

		for (int pvNum = 0; pvNum < 1000; pvNum++)
		{
			for (int diff = 0; diff < 4; diff++)
			{
				CachedScoreInfo info = ScoreCache[pvNum][diff][0];
				if (info.rank > 1 && info.rank <= 5) // at least clear and no greater than perfect
				{
					counts[diff * 4 + info.rank - 2] += 1;
				}
			}

			// exex special case
			CachedScoreInfo info = ScoreCache[pvNum][3][1];
			if (info.rank > 1 && info.rank <= 5) // at least clear and no greater than perfect
			{
				counts[4 * 4 + info.rank - 2] += 1;
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
