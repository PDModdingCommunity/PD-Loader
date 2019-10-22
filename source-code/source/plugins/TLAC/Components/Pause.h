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
		virtual void UpdateDraw2D() override;

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

		static const JvsButtons allButtons = (JvsButtons)(JVS_START | JVS_TRIANGLE | JVS_SQUARE | JVS_CROSS | JVS_CIRCLE | JVS_L | JVS_R); // deliberately only has control panel buttons
		static JvsButtons filteredButtons;

		static const int menuX = 640;
		static const int menuY = 360;
		static const int menuItemHeight = 36;
		static const int menuTextSize = 24;

		static const uint32_t bgLayer = 0x18;
		static const uint32_t contentLayer = 0x19; // same as startup screen
		
		static bool showUI;

		int triangleAet;
		int squareAet;
		int crossAet;
		int circleAet;

		static std::chrono::time_point<std::chrono::high_resolution_clock> menuItemMoveTime; // used for animation

		enum menusets
		{
			MENUSET_MAIN = 0,
			MENUSET_SEVOL = 1,
		};

		struct menuItem
		{
			std::string name;
			void(*callback)();
			bool keyRepeat; // unimplemented

			menuItem(std::string _name, void(*_callback)(), bool _keyRepeat)
			{
				name = _name;
				callback = _callback;
				keyRepeat = _keyRepeat;
			}
		};

		struct menuSet
		{
			std::string name;
			std::vector<menuItem> items;
		};

		static int curMenuPos;
		static int mainMenuPos; // syncs to menuPos when menuSet == menuset_main (used for restoring on back)
		static menusets curMenuSet;

		static void mainmenu() { curMenuSet = MENUSET_MAIN; curMenuPos = mainMenuPos; menuItemMoveTime = std::chrono::high_resolution_clock::now(); };
		static void unpause() { pause = false; };
		//static void restart() { *(uint8_t*)0x140d0b538 = 1; unpause(); }
		static void giveup() { giveUp = true; };

		static void sevolmenu() { curMenuSet = MENUSET_SEVOL; curMenuPos = 1; menuItemMoveTime = std::chrono::high_resolution_clock::now(); };
		static void sevolplus() { setSEVolume(10); };
		static void sevolminus() { setSEVolume(-10); };


		// defined as an array now so submenus could be added
		// not sure how to make this static for consistency so whatever
		menuSet menu[2] = {
			{
				"PAUSED",
				{
					{ "RESUME", unpause, false },
					//{ "RESTART", restart, false },
					{ "SE VOLUME", sevolmenu, false },
					{ "GIVE UP", giveup, false },
				}
			},
			{
				"SE VOLUME",
				{
					{ "+", sevolplus, true },
					{ "XX", mainmenu, false },
					{ "-", sevolminus, true },
				}
			},
		};

		void setMenuPos(menusets set, int pos)
		{
			if (set >= 0 && set < _countof(menu))
				curMenuSet = set;
			else
				curMenuSet = MENUSET_MAIN;

			if (pos < 0)
				pos = menu[curMenuSet].items.size() - 1;
			else if (pos >= menu[curMenuSet].items.size())
				pos = 0;

			curMenuPos = pos;

			if (curMenuSet == MENUSET_MAIN)
				mainMenuPos = curMenuPos;

			menuItemMoveTime = std::chrono::high_resolution_clock::now();
		}
	};
}
