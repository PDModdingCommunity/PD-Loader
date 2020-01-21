#pragma once
#include "EmulatorComponent.h"
#include "GameState.h"
#include <string>

namespace TLAC::Components
{
	class FastLoader : public EmulatorComponent
	{
	public:
		FastLoader();
		~FastLoader();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

	private:
		const std::string COMPONENTS_CONFIG_FILE_NAME = "components.ini";
		int updatesPerFrame = 39;
		
		GameState currentGameState;
		GameState previousGameState;
		bool dataInitialized = false;
	};
}
