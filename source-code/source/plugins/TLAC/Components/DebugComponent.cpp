#include "DebugComponent.h"
#include "../Constants.h"

namespace TLAC::Components
{
	const char* GameStateNames[] =
	{
		"STARTUP",
		"ADVERTISE",
		"GAME",
		"DATA_TEST",
		"TEST_MODE",
		"APP_ERROR",
		"MAX",
	};

	const char* SubGameStateNames[] =
	{
		"DATA_INITIALIZE",
		"SYSTEM_STARTUP",
		"SYSTEM_STARTUP_ERROR",
		"WARNING",
		"LOGO",
		"RATING",
		"DEMO",
		"TITLE",
		"RANKING",
		"SCORE_RANKING",
		"CM",
		"PHOTO_MODE_DEMO",
		"SELECTOR",
		"GAME_MAIN",
		"GAME_SEL",
		"STAGE_RESULT",
		"SCREEN_SHOT_SEL",
		"SCREEN_SHOT_RESULT",
		"GAME_OVER",
		"DATA_TEST_MAIN",
		"DATA_TEST_MISC",
		"DATA_TEST_OBJ",
		"DATA_TEST_STG",
		"DATA_TEST_MOT",
		"DATA_TEST_COLLISION",
		"DATA_TEST_SPR",
		"DATA_TEST_AET",
		"DATA_TEST_AUTH_3D",
		"DATA_TEST_CHR",
		"DATA_TEST_ITEM",
		"DATA_TEST_PERF",
		"DATA_TEST_PVSCRIPT",
		"DATA_TEST_PRINT",
		"DATA_TEST_CARD",
		"DATA_TEST_OPD",
		"DATA_TEST_SLIDER",
		"DATA_TEST_GLITTER",
		"DATA_TEST_GRAPHICS",
		"DATA_TEST_COLLECTION_CARD",
		"TEST_MODE_MAIN",
		"APP_ERROR",
		"MAX",
	};

	const char* DataTestNames[] =
	{
		"MAIN TEST",
		"MISC TEST",
		"OBJECT TEST",
		"STAGE TEST",
		"MOTION TEST",
		"COLLISION TEST",
		"SPRITE TEST",
		"2DAUTH TEST",
		"3DAUTH TEST",
		"CHARA TEST",
		"ITEM TEST",
		"PERFORMANCE TEST",
		"PVSCRIPT TEST",
		"PRINT TEST",
		"CARD TEST",
		"OPD TEST",
		"SLIDER TEST",
		"GLITTER TEST",
		"GRAPHICS TEST",
		"COLLECTION CARD TEST",
	};

	typedef void ChangeGameState(GameState);
	ChangeGameState* changeGameState = (ChangeGameState*)CHANGE_MODE_ADDRESS;

	typedef void ChangeSubState(GameState, SubGameState);
	ChangeSubState* changeSubState = (ChangeSubState*)CHANGE_SUB_MODE_ADDRESS;

	DebugComponent::DebugComponent()
	{
	}

	DebugComponent::~DebugComponent()
	{
	}

	const char* DebugComponent::GetDisplayName()
	{
		return "debug_component";
	}

	void DebugComponent::Initialize(ComponentsManager*)
	{
		printf("[TLAC] DebugComponent::Initialize(): Initialized\n");

		InjectPatches();

		HWND consoleHandle = GetConsoleWindow();
		ShowWindow(consoleHandle, SW_SHOW);

		// In case the FrameRateManager isn't enabled
		DWORD oldProtect;
		VirtualProtect((void*)AET_FRAME_DURATION_ADDRESS, sizeof(float), PAGE_EXECUTE_READWRITE, &oldProtect);
	}

	void DebugComponent::Update()
	{
		if (dataTestMain)
		{
			Input::Keyboard::GetInstance()->PollInput();
			UpdateDataTestMain();
		}
	}

	void DebugComponent::UpdateInput()
	{
		auto keyboard = Input::Keyboard::GetInstance();

		// fast forward menus
		if (keyboard->IsDown(VK_SHIFT))
		{
			float* frameDuration = (float*)AET_FRAME_DURATION_ADDRESS;

			if (keyboard->IsDown(VK_TAB))
				*frameDuration = 1.0f / (GetGameFrameRate() / aetSpeedUpFactor);
			else if (keyboard->IsReleased(VK_TAB))
				*frameDuration = 1.0f / 60.0f;
		}

		for (size_t i = 0; i < _countof(gameStateKeyMappings); i++)
		{
			if (keyboard->IsTapped(gameStateKeyMappings[i].KeyCode))
				InternalChangeGameState(gameStateKeyMappings[i].State);
		}
	}

	void DebugComponent::InjectPatches()
	{
		const struct { void* Address; std::initializer_list<uint8_t> Data; } patches[] =
		{
			// Prevent the DATA_TEST game state from exiting on the first frame
			{ (void*)0x0000000140284B01, { 0x00 } },
			// Enable dw_gui sprite draw calls
			{ (void*)0x0000000140192601, { 0x00 } },
			// Update the dw_gui display
			{ (void*)0x0000000140302600, { 0xB0, 0x01 } },
			// Draw the dw_gui display
			{ (void*)0x0000000140302610, { 0xB0, 0x01 } },
			// Enable the dw_gui widgets
			{ (void*)0x0000000140192D00, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 } },
		};

		for (size_t i = 0; i < _countof(patches); i++)
			InjectCode(patches[i].Address, patches[i].Data);

		if (*(int*)0x000000014019341B == 0x00)
		{
			// Disable debug cursor
			InjectCode((void*)0x00000001403012b5, { 0xeb });
		}
	}

	void DebugComponent::SetConsoleForeground()
	{
		HWND consoleHandle = GetConsoleWindow();
		ShowWindow(consoleHandle, SW_SHOW);
		
		if (consoleHandle == NULL)
			return;

		WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
		GetWindowPlacement(consoleHandle, &place);

		switch (place.showCmd)
		{
		case SW_SHOWMAXIMIZED:
			ShowWindow(consoleHandle, SW_SHOWMAXIMIZED);
			break;
		case SW_SHOWMINIMIZED:
			ShowWindow(consoleHandle, SW_RESTORE);
			break;
		default:
			ShowWindow(consoleHandle, SW_NORMAL);
			break;
		}

		SetWindowPos(0, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
		SetForegroundWindow(consoleHandle);
	}

	void DebugComponent::PrintDataTestMain()
	{
		system("cls");
		printf(" DATA TEST MAIN:\n\n");

		for (int i = SUB_DATA_TEST_MISC; i <= SUB_DATA_TEST_COLLECTION_CARD; i++)
			printf("%s  %s\n", i == selectionIndex ? "->" : "  ", DataTestNames[i - SUB_DATA_TEST_MAIN]);

		printf("\n");
		SetConsoleForeground();
	}

	void DebugComponent::InternalChangeGameState(GameState state)
	{
		changeGameState(state);
		printDataTestMain = dataTestMain = (state == GS_DATA_TEST);
	}

	void DebugComponent::UpdateDataTestMain()
	{
		auto keyboard = Input::Keyboard::GetInstance();

		if (keyboard->IsIntervalTapped(VK_UP))
		{
			selectionIndex--;
			printDataTestMain = true;
		}

		if (keyboard->IsIntervalTapped(VK_DOWN))
		{
			selectionIndex++;
			printDataTestMain = true;
		}

		if (selectionIndex > SUB_DATA_TEST_COLLECTION_CARD)
			selectionIndex = SUB_DATA_TEST_MISC;

		if (selectionIndex < SUB_DATA_TEST_MISC)
			selectionIndex = SUB_DATA_TEST_COLLECTION_CARD;

		if (keyboard->IsTapped(VK_RETURN))
		{
			dataTestMain = false;

			printf("[%s] -> [%s]\n", SubGameStateNames[SUB_DATA_TEST_MAIN], SubGameStateNames[selectionIndex]);
			changeSubState(GS_DATA_TEST, (SubGameState)selectionIndex);
		}

		if (printDataTestMain)
		{
			PrintDataTestMain();
			printDataTestMain = false;
		}
	}

	void DebugComponent::InjectCode(void* address, const std::initializer_list<uint8_t> &data)
	{
		const size_t byteCount = data.size() * sizeof(uint8_t);

		DWORD oldProtect;
		VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);

		memcpy(address, data.begin(), byteCount);
	}
}
