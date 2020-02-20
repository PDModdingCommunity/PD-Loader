#include "FrameRateManager.h"
#include "../Constants.h"
#include "GameState.h"
#include <stdio.h>
#include <windows.h>
#include "../framework.h"
#include <vector>
#include "detours.h"

namespace TLAC::Components
{
	FrameRateManager::FrameRateManager()
	{
		std::string utf8path = TLAC::framework::GetModuleDirectory() + "/config.ini";
		WCHAR utf16buf[256];
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, utf16buf, 256);

		float nMotionRate = GetPrivateProfileIntW(L"graphics", L"frm.motion.rate", 300, utf16buf);
		motionSpeedMultiplier = nMotionRate / 60.0f;
	}

	FrameRateManager::~FrameRateManager()
	{
	}

	const char* FrameRateManager::GetDisplayName()
	{
		return "frame_rate_manager";
	}


	float fspeed_error = 0; // compensation value for use in this frame
	float fspeed_error_next = 0; // save a compensation value to be used in the next frame

	float(*divaGetFrameSpeed)() = (float(*)())0x140192D50;

	// a version of the original function that tries to round the output to more closely match chara motion timings
	float hookedGetFrameSpeed()
	{
		float frameSpeed = divaGetFrameSpeed();

		// below is somewhat based (in concept) on 140194ad0 (motion quantisation thingy func)

		// add the error compensation from last frame
		frameSpeed += fspeed_error;

		// separate whole and fractional parts of speed
		// float speed_rounded = floorf(frameSpeed);
		// float speed_remainder = frameSpeed - speed_rounded;
		float speed_rounded;
		float speed_remainder = modff(frameSpeed, &speed_rounded);

		// save the remainder as error compensation for next frame
		if (fspeed_error_next == 0)
			fspeed_error_next = speed_remainder;
			

		return speed_rounded;
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

		// fix frame speed slider initial value (should ignore effect of auto speed)
		InjectCode((void*)0x140338f2f, { 0xf3, 0x0f, 0x10, 0x0d, 0x61, 0x18, 0xba, 0x00 }); // MOVSS XMM1, dword ptr [0x140eda798] (raw framespeed)
		InjectCode((void*)0x140338f37, { 0x48, 0x8b, 0x8f, 0x80, 0x01, 0x00, 0x00 });       // MOV RCX, qword ptr [0x180 + RDI]

		InjectCode((void*)0x140338ebe, { 0xf3, 0x0f, 0x10, 0x0d, 0xd2, 0x18, 0xba, 0x00 }); // MOVSS XMM1, dword ptr [0x140eda798] (raw framespeed)
		InjectCode((void*)0x140338ec6, { 0x48, 0x8b, 0x05, 0xfb, 0xb1, 0xe5, 0x00 });       // MOV RAX, qword ptr [0x1411940c8]
		InjectCode((void*)0x140338ecd, { 0x48, 0x8b, 0x88, 0x80, 0x01, 0x00, 0x00 });       // MOV RCX, qword ptr [0x180 + RAX]


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


		// replace divaGetFrameSpeed with a version that rounds the output to fix issues
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaGetFrameSpeed, hookedGetFrameSpeed);
		DetourTransactionCommit();
	}

	void FrameRateManager::Update()
	{
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
			*(bool*)USE_AUTO_FRAMESPEED_ADDRESS = true;

			// target framerate
			*(float*)AUTO_FRAMESPEED_TARGET_FRAMERATE_ADDRESS = *pvFrameRate;

			// trying to fix meltdown's water
			//if ((uint64_t*)0x1411943f8 != nullptr)
				//*(float*)(*(uint64_t*)0x1411943f8 + 0x1c) = (60.0f / GetGameFrameRate()) / 15.0f;
				//*(float*)(*(uint64_t*)0x1411943f8 + 0x10) = *(float*)(*(uint64_t*)0x1411943f8);
		}
		else
		{
			// enable dynamic framerate
			*(bool*)USE_AUTO_FRAMESPEED_ADDRESS = true;

			// target framerate
			*(float*)AUTO_FRAMESPEED_TARGET_FRAMERATE_ADDRESS = 60.0f;
		}
	}

	void FrameRateManager::UpdateDraw2D()
	{
		// cycle the framespeed timing error once per frame
		fspeed_error = fspeed_error_next;
		fspeed_error_next = 0;
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
