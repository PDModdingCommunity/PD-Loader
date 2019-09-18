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
	WCHAR ScoreSaver::configPath[256];
	ScoreSaver::ScoreSaver()
	{
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

	char ScoreSaver::selHighScore[9] = "39393939";
	char ScoreSaver::selHighPct1[4] = "39";
	char ScoreSaver::selHighPct2[3] = "39";
	void(__stdcall* ScoreSaver::divaInitResults)(void* cls) = (void(__stdcall*)(void* cls))RESULTS_INIT_ADDRESS;
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


		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)ScoreSaver::divaInitResults, (PVOID)(ScoreSaver::hookedInitResults));
		DetourTransactionCommit();
	}
	
	void ScoreSaver::hookedInitResults(void* cls)
	{
		divaInitResults(cls);
				
		int clearRank = *(int*)(RESULTS_BASE_ADDRESS + 0xe8);
		int insurance = *(int*)(GAME_INFO_ADDRESS + 0x14);
		
		if (clearRank < 2 || insurance !=0)
			return;

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
		
		int checkField = ((short)score ^ ((short)cntHitTypes[0] << 16) ^ ((short)cntHitTypes[1]) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank;

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

		if (score <= oldScore)
			return;

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

		swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
		swprintf(val, 32, L"%d", checkField);
		WritePrivateProfileStringW(section, key, val, configPath);
	}

	void ScoreSaver::Update()
	{
		int pvNum = *(int*)SELPV_CURRENT_SONG_ADDRESS;
		int diff = *(int*)(GAME_INFO_ADDRESS);
		int diffIsEx = *(int*)(GAME_INFO_ADDRESS + 0x4);

		if (pvNum == currentPv && diff == currentDifficulty && diffIsEx == currentDifficultyIsEx)
			return;
		
		currentPv = pvNum;
		currentDifficulty = diff;
		currentDifficultyIsEx = diffIsEx;


		WCHAR keyBase[32]; // needs to be big enough to store pv.999.diff.4.ex
		WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.challengescore

		const WCHAR section[] = L"scores";

		if (currentDifficultyIsEx == 0)
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d", currentPv, currentDifficulty);
		else
			swprintf(keyBase, 32, L"pv.%03d.diff.%01d.ex", currentPv, currentDifficulty);


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

		swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
		int expectedCheck = GetPrivateProfileIntW(section, key, -1, configPath);


		if (expectedCheck == (int)(((short)score ^ ((short)cntHitTypes[0] << 16) ^ ((short)cntHitTypes[1]) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank))
		{
			if (score > 99999999)
				score = 99999999;

			snprintf(selHighScore, sizeof(selHighScore), "%d", score);
			snprintf(selHighPct1, sizeof(selHighPct1), "%d", percent / 100);
			snprintf(selHighPct2, sizeof(selHighPct2), "%d", percent % 100);
		}
		else
		{
			snprintf(selHighScore, sizeof(selHighScore), "--------");
			snprintf(selHighPct1, sizeof(selHighPct1), "---");
			snprintf(selHighPct2, sizeof(selHighPct2), "--");
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
}