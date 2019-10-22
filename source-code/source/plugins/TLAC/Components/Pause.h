#pragma once
#include "../Constants.h"
#include "EmulatorComponent.h"
#include "PlayerData.h"
#include "Input/InputState.h"
#include "Input/TouchSliderState.h"
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
		static TouchSliderState* sliderState;

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
		static int mainMenuPos; // syncs to curMenuPos when curMenuSet == MENUSET_MAIN (used for restoring on back)
		static menusets curMenuSet;

		static std::chrono::time_point<std::chrono::high_resolution_clock> menuItemMoveTime; // used for animation

		static void mainmenu() { setMenuPos(MENUSET_MAIN, mainMenuPos); };
		static void unpause() { pause = false; };
		//static void restart() { *(uint8_t*)0x140d0b538 = 1; unpause(); }
		static void giveup() { giveUp = true; };

		static void sevolmenu() { setMenuPos(MENUSET_SEVOL, 1); };
		static void sevolplus() { setSEVolume(10); };
		static void sevolminus() { setSEVolume(-10); };

		// contents are in Pause.cpp because they can't be inline here for a static (const) array/vec
		static std::vector<menuSet> menu;

		static void setMenuPos(menusets set, int pos);

		static float getMenuAnimPos();
	};
}
