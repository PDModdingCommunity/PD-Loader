#include "FrameRateManager.h"
#include "../Constants.h"
#include "GameState.h"
#include <stdio.h>
#include <windows.h>
#include "../framework.h"
#include <vector>

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
		DWORD oldProtect;
		VirtualProtect((void*)AET_FRAME_DURATION_ADDRESS, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);


		// fix auto speed for high fps
		InjectCode((void*)0x140192d7b, { 0x90, 0x90, 0x90 });


		// fix AETs
		InjectCode((void*)0x140170394, { 0xF3, 0x0F, 0x5E, 0x05, 0x34, 0xA3, 0xD6, 0x00 }); // DIVSS XMM0, dword ptr [0x140eda6d0] (framerate)

		// fix edit PV AETs (thanks lyb)
		InjectCode((void*)0x140192d30, { 0xF3, 0x0F, 0x10, 0x05, 0x5C, 0x02, 0x00, 0x00 }); // MOVSS XMM0, dword ptr [0x140192f94]

		// fix ageage hair effect
		InjectCode((void*)0x14054352f, { 0xE8, 0x1C, 0xF8, 0xC4, 0xFF }); // CALL 0x140192d50 (getFrameSpeed)
		InjectCode((void*)0x140543534, { 0xF3, 0x0F, 0x59, 0xD8 });       // MULSS XMM3, XMM0
		InjectCode((void*)0x140543538, { 0xEB, 0xB6 });                   // JMP 0x1405434f0

		// fix wind effect
		InjectCode((void*)0x14053ca71, { 0xEB, 0x3F });                                     // JMP 0x14053cab2
		InjectCode((void*)0x14053cab2, { 0xF3, 0x0F, 0x10, 0x05, 0xFA, 0x53, 0x46, 0x00 }); // MOVSS XMM0, dword ptr [0x1409a1eb4] (60.0f)
		InjectCode((void*)0x14053caba, { 0xE9, 0x42, 0xFE, 0xFF, 0xFF });                   // JMP 0x14053c901
		InjectCode((void*)0x14053c901, { 0xF3, 0x0F, 0x5E, 0x05, 0xC7, 0xDD, 0x99, 0x00 }); // DIVSS XMM0, dword ptr [0x140eda6d0] (framerate)
		InjectCode((void*)0x14053c909, { 0xE9, 0x68, 0x01, 0x00, 0x00 });                   // JMP 0x14053ca76
		
		
		// below are patches for the alternate method (breaks physics)

		//// run motions at motionSpeedMultiplier x rate
		//	// skip unncessary code to free code space
		//  InjectCode((void*)0x140194b2b, { 0xEB, 0x24 }); // JMP 0x140194b51

		//  InjectCode((void*)0x140194b2d, { 0xE8, 0x1E, 0xE2, 0xFF, 0xFF });  // CALL 0x140192d50
		//  InjectCode((void*)0x140194b32, {                                   // MOV ECX, &motionSpeedMultiplier
		//  	0x48,
		//  	0xB9,
		//  	(uint8_t)((uint64_t)&motionSpeedMultiplier & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 8) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 16) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 24) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 32) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 40) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 48) & 0xFF),
		//  	(uint8_t)(((uint64_t)&motionSpeedMultiplier >> 56) & 0xFF),
		//  });
		//  InjectCode((void*)0x140194b3c, { 0x66, 0x67, 0x0F, 0x6E, 0x09 }); // MOVD XMM1, dword ptr [ECX]
		//  InjectCode((void*)0x140194b41, { 0xF3, 0x0F, 0x59, 0xC1 });       // MULSS XMM0, XMM1
		//  InjectCode((void*)0x140194b45, { 0xEB, 0xA1 });                   // JMP 0x140194ae8

		//	// jump to new code
		//  InjectCode((void*)0x140194ae3, { 0xEB, 0x48 }); // JMP 0x140194b2d


		////	// trying to fix physics (fail)
		////    InjectCode((void*)0x14011d537, { 0xF3, 0x0F, 0x10, 0x05, 0xB1, 0xFF, 0x9E, 0x00, 0x90 }); // change source to dword ptr [0x140b0d4f0]
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
		// *aetFrameDuration = 1.0f / GetGameFrameRate();

		if (*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME)
		{
			*pvFrameRate = 60.0f * motionSpeedMultiplier;
		}
		else
		{
			*pvFrameRate = 60.0f;
		}

		if (*(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN || *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_DEMO)
		{
			// enable dynamic framerate
			*(bool*)0x140eda79c = true;

			// target framerate
			*(float*)0x140eda6cc = *pvFrameRate;

			// trying to fix meltdown's water
			//if ((uint64_t*)0x1411943f8 != nullptr)
				//*(float*)(*(uint64_t*)0x1411943f8 + 0x1c) = (60.0f / GetGameFrameRate()) / 15.0f;
				//*(float*)(*(uint64_t*)0x1411943f8 + 0x10) = *(float*)(*(uint64_t*)0x1411943f8);
		}
		else
		{
			// enable dynamic framerate
			*(bool*)0x140eda79c = true;

			// target framerate
			*(float*)0x140eda6cc = 60.0f;
		}
	}

	void FrameRateManager::InjectCode(void* address, const std::vector<uint8_t> data)
	{
		const size_t byteCount = data.size() * sizeof(uint8_t);

		DWORD oldProtect;
		VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(address, data.data(), byteCount);
		VirtualProtect(address, byteCount, oldProtect, nullptr);
	}
}
