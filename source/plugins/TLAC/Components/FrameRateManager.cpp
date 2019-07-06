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
		DWORD oldProtect;
		VirtualProtect((void*)AET_FRAME_DURATION_ADDRESS, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
	}

	void FrameRateManager::Update()
	{
		float frameRate = 0.0f;
		frameRate = RoundFrameRate(GetGameFrameRate());

		*aetFrameDuration = 1.0f / frameRate;
		*pvFrameRate = frameRate;

		bool inGame = *(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME;

		if (inGame)
		{
			// During the GAME state the frame rate will be handled by the PvFrameRate instead

			constexpr float defaultFrameSpeed = 1.0f;
			constexpr float defaultFrameRate = 60.0f;

			// This PV struct creates a copy of the PvFrameRate & PvFrameSpeed during the loading screen
			// so we'll make sure to keep updating it as well.
			// Each new motion also creates its own copy of these values but keeping track of the active motions is annoying
			// and they usually change multiple times per PV anyway so this should suffice for now
			float* pvStructPvFrameRate = (float*)(0x0000000140CDD978 + 0x2BF98);
			float* pvStructPvFrameSpeed = (float*)(0x0000000140CDD978 + 0x2BF9C);

			*pvStructPvFrameRate = *pvFrameRate;
			*pvStructPvFrameSpeed = (defaultFrameRate / *pvFrameRate);

			*frameSpeed = defaultFrameSpeed;
		}
		else
		{
			*frameSpeed = *aetFrameDuration / defaultAetFrameDuration;
		}
	}

	float FrameRateManager::RoundFrameRate(float frameRate)
	{
		constexpr float roundingThreshold = 4.0f;

		for (int i = 0; i < sizeof(commonRefreshRates) / sizeof(float); i++)
		{
			float refreshRate = commonRefreshRates[i];

			if (frameRate > refreshRate - roundingThreshold && frameRate < refreshRate + roundingThreshold)
				frameRate = refreshRate;
		}

		return frameRate;
	}
}
