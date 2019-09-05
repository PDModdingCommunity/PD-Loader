#include "FrameRateManager.h"
#include "../Constants.h"
#include "GameState.h"
#include <stdio.h>
#include <windows.h>
#include "../framework.h"

namespace TLAC::Components
{
	FrameRateManager::FrameRateManager()
	{
	}

	FrameRateManager::~FrameRateManager()
	{
	}

	const char* FrameRateManager::GetDisplayName()
	{
		return "frame_rate_manager";
	}

	void FrameRateManager::Initialize(ComponentsManager*)
	{
		pvFrameRate = (float*)PV_FRAME_RATE_ADDRESS;
		frameSpeed = (float*)FRAME_SPEED_ADDRESS;
		aetFrameDuration = (float*)AET_FRAME_DURATION_ADDRESS;

		// The default is expected to be 1.0 / 60.0
		defaultAetFrameDuration = *aetFrameDuration;

		// This const variable is stored inside a data segment so we don't want to throw any access violations
		DWORD oldProtect, bck;
		VirtualProtect((void*)AET_FRAME_DURATION_ADDRESS, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);


		// fix auto speed for high fps
		VirtualProtect((BYTE*)0x140192d7b, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
		*((BYTE*)0x140192d7b + 0) = 0x90;
		*((BYTE*)0x140192d7b + 1) = 0x90;
		*((BYTE*)0x140192d7b + 2) = 0x90;
		VirtualProtect((BYTE*)0x140192d7b, 3, oldProtect, &bck);


		// fix edit PV AETs (thanks lyb)
		VirtualProtect((BYTE*)0x140192D30, 8, PAGE_EXECUTE_READWRITE, &oldProtect); // MOVSS XMM0, dword ptr [0x140192f94]
		*((BYTE*)0x140192D30 + 0) = 0xF3;
		*((BYTE*)0x140192D30 + 1) = 0x0F;
		*((BYTE*)0x140192D30 + 2) = 0x10;
		*((BYTE*)0x140192D30 + 3) = 0x05;
		*((BYTE*)0x140192D30 + 4) = 0x5C;
		*((BYTE*)0x140192D30 + 5) = 0x02;
		*((BYTE*)0x140192D30 + 6) = 0x00;
		*((BYTE*)0x140192D30 + 7) = 0x00;
		VirtualProtect((BYTE*)0x140192D30, 8, oldProtect, &bck);


		// fix ageage hair effect
			VirtualProtect((BYTE*)0x14054352f, 5, PAGE_EXECUTE_READWRITE, &oldProtect); // CALL 0x140192d50 (getFrameSpeed)
			*((BYTE*)0x14054352f + 0) = 0xE8;
			*((BYTE*)0x14054352f + 1) = 0x1C;
			*((BYTE*)0x14054352f + 2) = 0xF8;
			*((BYTE*)0x14054352f + 3) = 0xC4;
			*((BYTE*)0x14054352f + 4) = 0xFF;
			VirtualProtect((BYTE*)0x14054352f, 5, oldProtect, &bck);

			VirtualProtect((BYTE*)0x140543534, 4, PAGE_EXECUTE_READWRITE, &oldProtect); // MULSS XMM3, XMM0
			*((BYTE*)0x140543534 + 0) = 0xF3;
			*((BYTE*)0x140543534 + 1) = 0x0F;
			*((BYTE*)0x140543534 + 2) = 0x59;
			*((BYTE*)0x140543534 + 3) = 0xD8;
			VirtualProtect((BYTE*)0x140543534, 4, oldProtect, &bck);

			VirtualProtect((BYTE*)0x140543538, 2, PAGE_EXECUTE_READWRITE, &oldProtect); // JMP 0x1405434f0
			*((BYTE*)0x140543538 + 0) = 0xEB;
			*((BYTE*)0x140543538 + 1) = 0xB6;
			VirtualProtect((BYTE*)0x140543538, 2, oldProtect, &bck);

		
		
		// below are patches for the alternate method (breaks physics)

		//// run motions at motionSpeedMultiplier x rate
		//	// skip unncessary code to free code space
		//	VirtualProtect((BYTE*)0x140194b2b, 2, PAGE_EXECUTE_READWRITE, &oldProtect); // JMP 0x140194b51
		//	*((BYTE*)0x140194b2b + 0) = 0xEB;
		//	*((BYTE*)0x140194b2b + 1) = 0x24;
		//	VirtualProtect((BYTE*)0x140194b2b, 2, oldProtect, &bck);


		//	VirtualProtect((BYTE*)0x140194b2d, 5, PAGE_EXECUTE_READWRITE, &oldProtect); // CALL 0x140192d50
		//	*((BYTE*)0x140194b2d + 0) = 0xE8;
		//	*((BYTE*)0x140194b2d + 1) = 0x1E;
		//	*((BYTE*)0x140194b2d + 2) = 0xE2;
		//	*((BYTE*)0x140194b2d + 3) = 0xFF;
		//	*((BYTE*)0x140194b2d + 4) = 0xFF;
		//	VirtualProtect((BYTE*)0x140194b2d, 5, oldProtect, &bck);

		//	VirtualProtect((BYTE*)0x140194b32, 10, PAGE_EXECUTE_READWRITE, &oldProtect); // MOV ECX, &motionSpeedMultiplier
		//	*((BYTE*)0x140194b32 + 0) = 0x48;
		//	*((BYTE*)0x140194b32 + 1) = 0xB9;
		//	*((BYTE*)0x140194b32 + 2) = (uint64_t)&motionSpeedMultiplier & 0xFF;
		//	*((BYTE*)0x140194b32 + 3) = ((uint64_t)&motionSpeedMultiplier >> 8) & 0xFF;
		//	*((BYTE*)0x140194b32 + 4) = ((uint64_t)&motionSpeedMultiplier >> 16) & 0xFF;
		//	*((BYTE*)0x140194b32 + 5) = ((uint64_t)&motionSpeedMultiplier >> 24) & 0xFF;
		//	*((BYTE*)0x140194b32 + 6) = ((uint64_t)&motionSpeedMultiplier >> 32) & 0xFF;
		//	*((BYTE*)0x140194b32 + 7) = ((uint64_t)&motionSpeedMultiplier >> 40) & 0xFF;
		//	*((BYTE*)0x140194b32 + 8) = ((uint64_t)&motionSpeedMultiplier >> 48) & 0xFF;
		//	*((BYTE*)0x140194b32 + 9) = ((uint64_t)&motionSpeedMultiplier >> 56) & 0xFF;
		//	VirtualProtect((BYTE*)0x140194b32, 10, oldProtect, &bck);

		//	VirtualProtect((BYTE*)0x140194b3c, 5, PAGE_EXECUTE_READWRITE, &oldProtect); // MOVD XMM1, dword ptr [ECX]
		//	*((BYTE*)0x140194b3c + 0) = 0x66;
		//	*((BYTE*)0x140194b3c + 1) = 0x67;
		//	*((BYTE*)0x140194b3c + 2) = 0x0F;
		//	*((BYTE*)0x140194b3c + 3) = 0x6E;
		//	*((BYTE*)0x140194b3c + 4) = 0x09;
		//	VirtualProtect((BYTE*)0x140194b3c, 5, oldProtect, &bck);

		//	VirtualProtect((BYTE*)0x140194b41, 4, PAGE_EXECUTE_READWRITE, &oldProtect); // MULSS XMM0, XMM1
		//	*((BYTE*)0x140194b41 + 0) = 0xF3;
		//	*((BYTE*)0x140194b41 + 1) = 0x0F;
		//	*((BYTE*)0x140194b41 + 2) = 0x59;
		//	*((BYTE*)0x140194b41 + 3) = 0xC1;
		//	VirtualProtect((BYTE*)0x140194b41, 4, oldProtect, &bck);

		//	VirtualProtect((BYTE*)0x140194b45, 2, PAGE_EXECUTE_READWRITE, &oldProtect); // JMP 0x140194ae8
		//	*((BYTE*)0x140194b45 + 0) = 0xEB;
		//	*((BYTE*)0x140194b45 + 1) = 0xA1;
		//	VirtualProtect((BYTE*)0x140194b45, 2, oldProtect, &bck);


		//	// jump to new code
		//	VirtualProtect((BYTE*)0x140194ae3, 2, PAGE_EXECUTE_READWRITE, &oldProtect); // JMP 0x140194b2d
		//	*((BYTE*)0x140194ae3 + 0) = 0xEB;
		//	*((BYTE*)0x140194ae3 + 1) = 0x48;
		//	VirtualProtect((BYTE*)0x140194ae3, 2, oldProtect, &bck);


		////	// trying to fix physics (fail)
		////	VirtualProtect((BYTE*)0x14011d537, 9, PAGE_EXECUTE_READWRITE, &oldProtect); // change source to dword ptr [0x140b0d4f0]
		////	*((BYTE*)0x14011d537 + 0) = 0xF3;
		////	*((BYTE*)0x14011d537 + 1) = 0x0F;
		////	*((BYTE*)0x14011d537 + 2) = 0x10;
		////	*((BYTE*)0x14011d537 + 3) = 0x05;
		////	*((BYTE*)0x14011d537 + 4) = 0xB1;
		////	*((BYTE*)0x14011d537 + 5) = 0xFF;
		////	*((BYTE*)0x14011d537 + 6) = 0x9E;
		////	*((BYTE*)0x14011d537 + 7) = 0x00;
		////	*((BYTE*)0x14011d537 + 8) = 0x90;
		////	VirtualProtect((BYTE*)0x14011d537, 9, oldProtect, &bck);
	}

	void FrameRateManager::Update()
	{
		// this alterante way makes physics slowmo, but overall may be better if that's fixed

		//// target framerate
		//*(float*)0x140eda6cc = 60.0f;

		//// enable dynamic framerate
		//*(bool*)0x140eda79c = true;

		//*pvFrameRate = 60.0f;

		//*aetFrameDuration = 1.0f / GetGameFrameRate();

		//bool inGame = *(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME;
		//if (inGame)
		//{
		//	// This PV struct creates a copy of the PvFrameRate & PvFrameSpeed during the loading screen
		//	// so we'll make sure to keep updating it as well.
		//	// Each new motion also creates its own copy of these values but keeping track of the active motions is annoying
		//	// and they usually change multiple times per PV anyway so this should suffice for now
		//	float* pvStructPvFrameRate = (float*)(0x0000000140CDD978 + 0x2BF98);
		//	float* pvStructPvFrameSpeed = (float*)(0x0000000140CDD978 + 0x2BF9C);

		//	*pvStructPvFrameRate = 60.0; // ?
		//	*pvStructPvFrameSpeed = 1.0 / motionSpeedMultiplier;
		//}




		// this way breaks some anim speeds, but they're fixable

		*pvFrameRate = 60.0f * motionSpeedMultiplier;

		// target framerate
		*(float*)0x140eda6cc = *pvFrameRate;

		*aetFrameDuration = 1.0f / GetGameFrameRate();

		bool inGame = *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN || *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_DEMO;
		if (inGame)
		{
			// enable dynamic framerate
			*(bool*)0x140eda79c = true;
		}
		else
		{
			// disable dynamic framerate
			*(bool*)0x140eda79c = false;
		}
	}
}
