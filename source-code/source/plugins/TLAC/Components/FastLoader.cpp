#include "FastLoader.h"
#include "../FileSystem/ConfigFile.h"
#include "../Constants.h"
#include <stdio.h>
#include "../framework.h"

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
		TLAC::FileSystem::ConfigFile componentsConfig(TLAC::framework::GetModuleDirectory(), COMPONENTS_CONFIG_FILE_NAME);
		bool success = componentsConfig.OpenRead();
		if (success)
		{
			int speed = componentsConfig.GetIntegerValue("fast_loader_speed");
			if (speed >= 2 && speed <= 1024) updatesPerFrame = speed;
		}
		printf("[Fast Loader] Speed: %d\n", updatesPerFrame);
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
