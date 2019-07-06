#pragma once
#include "EmulatorComponent.h"
#include "GameState.h"
#include "../Input/Keyboard/Keyboard.h"

namespace TLAC::Components
{
	class DebugComponent : public EmulatorComponent
	{
	public:
		DebugComponent();
		~DebugComponent();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

	private:
		const float aetSpeedUpFactor = 4.0f;
		
		bool dataTestMain = false;
		bool printDataTestMain = false;
		int selectionIndex = SUB_DATA_TEST_MISC;

		const struct { BYTE KeyCode; GameState State; } gameStateKeyMappings[5] =
		{
			{ VK_F4, GS_ADVERTISE },
			{ VK_F5, GS_GAME },
			{ VK_F6, GS_DATA_TEST },
			{ VK_F7, GS_TEST_MODE },
			{ VK_F8, GS_APP_ERROR },
		};

		void InjectPatches();
		void SetConsoleForeground();
		void PrintDataTestMain();
		void InternalChangeGameState(GameState state);
		void UpdateDataTestMain();
		void InjectCode(void* address, const std::initializer_list<uint8_t> &data);
	};
}

