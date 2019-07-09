#pragma once
#include "EmulatorComponent.h"
#include "PlayerData.h"
#include "CustomPlayerData.h"
#include <string>

namespace TLAC::Components
{
	class PlayerDataManager : public EmulatorComponent
	{
	public:
		PlayerDataManager();
		~PlayerDataManager();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;

	private:
		PlayerData* playerData;
		CustomPlayerData* customPlayerData;
		int lastPvId = -1;
		bool initPvId = false;
		bool pvModuleLoaded = false;
		int lastModState = 0;
		void ApplyPatch();
		void LoadConfig();
		void ApplyCustomData();
	};
}

