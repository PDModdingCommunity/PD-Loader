#pragma once
#include "../Constants.h"
#include "EmulatorComponent.h"
#include "PlayerData.h"
#include "Input/InputState.h"
#include <string>
#include <vector>
#include <chrono>

namespace TLAC::Components
{
	class Pause : public EmulatorComponent
	{
	public:
		Pause();
		~Pause();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdatePostInput() override;
		virtual void UpdateDrawSprites() override;

		static bool pause; // set pause to change pause state
		static bool giveUp; // set give up to end current song
	private:
		// this is a mess of static so that menuItems can work
		static bool isPauseKeyTapped();
		static std::vector<bool> streamPlayStates;
		static void InjectCode(void* address, const std::vector<uint8_t> data);

		static bool isPaused; // tracks internal state

		static void saveOldPatchOps();

		static std::vector<uint8_t> origAetMovOp;
		static uint8_t* aetMovPatchAddress;

		static std::vector<uint8_t> origFramespeedOp;
		static uint8_t* framespeedPatchAddress;

		static bool hookedGiveUpFunc(void* cls);

		static void setSEVolume(int amount);

		static PlayerData* playerData;
		static InputState* inputState;

		static const int menuX = 640;
		static const int menuY = 360;
		static const int menuItemHeight = 36;
		static const int menuTextSize = 24;
		
		static bool showUI;
		static unsigned int menuPos;
		static unsigned int mainMenuPos; // syncs to menuPos when menuSet == menuset_main (used for restoring on back)
		static unsigned int menuSet;

		static std::chrono::time_point<std::chrono::high_resolution_clock> menuItemSelectTime;

		enum menusets
		{
			MENUSET_MAIN = 0,
			MENUSET_SEVOL = 1,
		};

		static void mainmenu() { menuSet = MENUSET_MAIN; menuPos = mainMenuPos; menuItemSelectTime = std::chrono::high_resolution_clock::now(); };
		static void unpause() { pause = false; };
		static void giveup() { giveUp = true; };

		static void sevolmenu() { menuSet = MENUSET_SEVOL; menuPos = 1; menuItemSelectTime = std::chrono::high_resolution_clock::now(); };
		static void sevolplus() { setSEVolume(10); };
		static void sevolminus() { setSEVolume(-10); };


		// defined as an array now so submenus could be added
		std::vector<std::pair<std::string, void(*)()>> menuItems[2] = { 
			{
				std::pair<std::string, void(*)()>(std::string("RESUME"), &unpause),
				std::pair<std::string, void(*)()>(std::string("SE VOLUME"), &sevolmenu),
				std::pair<std::string, void(*)()>(std::string("GIVE UP"), &giveup),
			},
			{
				std::pair<std::string, void(*)()>(std::string("+"), &sevolplus),
				std::pair<std::string, void(*)()>(std::string("XX"), &mainmenu),
				std::pair<std::string, void(*)()>(std::string("-"), &sevolminus),
			}
		};
	};
}
