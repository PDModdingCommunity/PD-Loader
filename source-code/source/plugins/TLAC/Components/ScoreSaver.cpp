#include "ScoreSaver.h"
#include "../Constants.h"
#include "../framework.h"
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
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, ScoreSaver::configPath, 256);
	}

	ScoreSaver::~ScoreSaver()
	{
	}

	const char* ScoreSaver::GetDisplayName()
	{
		return "score_saver";
	}

	void(__stdcall* ScoreSaver::divaInitResults)(void* cls) = (void(__stdcall*)(void* cls))RESULTS_INIT_ADDRESS;
	void ScoreSaver::Initialize(ComponentsManager*)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)ScoreSaver::divaInitResults, (PVOID)(ScoreSaver::hookedInitResults));
		DetourTransactionCommit();
	}
  
  void ScoreSaver::hookedInitResults(void* cls)
  {
    divaInitResults(cls);
        
    int clearRank = *(int*)(RESULTS_BASE_ADDRESS + 0xe8);
    int insurance = *(int*)RESULTS_INSURANCE_ADDRESS;
    
    if (clearRank < 2 || insurance !=0)
      return;

	// get the base for this specific set of results
	uint64_t resultBase = *(uint64_t*)(RESULTS_BASE_ADDRESS + 0x100);
    
    int pvNum = *(int*)(resultBase + 0x2c);
    int pvDifficulty = *(int*)(resultBase + 0x34);
    int modifier = *(int*)(resultBase + 0x70);
    
    int* cntHitTypes = (int*)(resultBase + 0x158);
    int combo = *(int*)(resultBase + 0x180);
    int score = *(int*)(resultBase + 0x18c);
    int percent = *(int*)(resultBase + 0x190);
    
    int checkField = ((short)score ^ ((short)cntHitTypes[0] << 16) ^ ((short)cntHitTypes[1]) ^ ((short)percent << 16) ^ ((short)combo)) * clearRank;


	WCHAR keyBase[16]; // needs to be big enough to store pv.999.diff.4
	WCHAR key[32]; // needs to be big enough to store pv.999.diff.4.modifier
	WCHAR val[16]; // needs to be big enough to store any int

	const WCHAR section[] = L"scores";

	swprintf(keyBase, 16, L"pv.%03d.diff.%01d", pvNum, pvDifficulty);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
	int oldScore = GetPrivateProfileIntW(section, key, 0, ScoreSaver::configPath);

	if (score <= oldScore)
		return;

	swprintf(key, 32, L"%ls.%ls", keyBase, L"combo");
	swprintf(val, 16, L"%d", combo);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"cools");
	swprintf(val, 16, L"%d", cntHitTypes[0]);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"modifier");
	swprintf(val, 16, L"%d", modifier);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"percent");
	swprintf(val, 16, L"%d", percent);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"rank");
	swprintf(val, 16, L"%d", clearRank);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"score");
	swprintf(val, 16, L"%d", score);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);

	swprintf(key, 32, L"%ls.%ls", keyBase, L"check");
	swprintf(val, 16, L"%d", checkField);
	WritePrivateProfileStringW(section, key, val, ScoreSaver::configPath);
  }

	void ScoreSaver::Update()
	{
		return;
	}

	void ScoreSaver::UpdateInput()
	{
		return;
	}
}
