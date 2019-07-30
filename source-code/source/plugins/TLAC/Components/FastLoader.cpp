#include "FastLoader.h"
#include "../Constants.h"
#include <stdio.h>

namespace TLAC::Components
{
	FastLoader::FastLoader()
	{
	}

	FastLoader::~FastLoader()
	{
	}

	const char* FastLoader::GetDisplayName()
	{
		return "fast_loader";
	}

	void FastLoader::Initialize(ComponentsManager*)
	{
	}

	void FastLoader::Update()
	{
		if (dataInitialized)
			return;

		previousGameState = currentGameState;
		currentGameState = *(GameState*)CURRENT_GAME_STATE_ADDRESS;

		if (currentGameState == GS_STARTUP)
		{
			typedef void UpdateTask();
			UpdateTask* updateTask = (UpdateTask*)UPDATE_TASKS_ADDRESS;

			// speed up TaskSystemStartup
			for (int i = 0; i < updatesPerFrame; i++)
				updateTask();

			constexpr int DATA_INITIALIZED = 3;

			// skip TaskDataInit
			*(int*)(DATA_INIT_STATE_ADDRESS) = DATA_INITIALIZED;

			// skip TaskWarning
			*(int*)(SYSTEM_WARNING_ELAPSED_ADDRESS) = 3939;
		}
		else if (previousGameState == GS_STARTUP)
		{
			dataInitialized = true;
			printf("[TLAC] FastLoader::Update(): Data Initialized\n");
		}
	}

	void FastLoader::UpdateInput()
	{
		return;
	}
}
