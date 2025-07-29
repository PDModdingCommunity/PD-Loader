#pragma once
#include "../Constants.h"
#include "EmulatorComponent.h"
#include "PlayerData.h"
#include "GameState.h"
#include "../Utilities/Drawing.h"
#include "Input/InputState.h"
#include "Input/TouchSliderState.h"
#include "Input/TouchPanelState.h"
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
		virtual void OnFocusLost() override;

		static bool pause; // set pause to change pause state
		static bool giveUp; // set give up to end current song

		static bool autoPause; // pause when window loses focus
		static bool ignoreAutoPause; // ignore pause on PV
		static bool autoReplay; // loop PV
	private:
		// this is a mess of static so that menuItems can work
		static bool isPauseKeyTapped();
		static bool isInGame();
		static std::vector<bool> streamPlayStates;
		static void InjectCode(void* address, const std::vector<uint8_t> data);

		static bool isPaused; // tracks internal state
		
		static void saveOldPatchOps();

		static std::vector<uint8_t> origAetMovOp;
		static uint8_t* aetMovPatchAddress;

		static std::vector<uint8_t> origFramespeedOp;
		static uint8_t* framespeedPatchAddress;
		static float_t origDeltaFrameHistory;
		static int32_t origDeltaFrameHistoryInt;
		static float_t* deltaFrameHistoryAddress;
		static int32_t* deltaFrameHistoryIntAddress;

		static std::vector<uint8_t> origAgeageHairOp;
		static uint8_t* ageageHairPatchAddress;

		static bool hookedGiveUpFunc(void* cls);
		static void hookedDivaPVEndFunc(uint64_t, char, char);
		static char hookedScriptCommandFunc(uint64_t, uint64_t, uint64_t, void*, void*, char, char);

		static void setSEVolume(int amount);

		static PlayerData* playerData;
		static InputState* inputState;
		static TouchSliderState* sliderState;
		static TouchPanelState* panelState;
		static ComponentsManager* componentsManager;

		static const JvsButtons allButtons = (JvsButtons)(JVS_START | JVS_TRIANGLE | JVS_SQUARE | JVS_CROSS | JVS_CIRCLE | JVS_L | JVS_R); // deliberately only has control panel buttons
		static JvsButtons filteredButtons;
		static int lastTouchType;

		static const int menuX = 640;
		static const int menuY = 360;
		static const int menuItemWidth = 150;
		static const int menuItemHeight = 36;
		static const int menuItemPadding = 12;
		static const int menuItemTotalHeight = menuItemHeight + menuItemPadding;
		static const int menuTextSize = 24;

		static const uint32_t bgLayer = 0x18;
		static const uint32_t contentLayer = 0x19; // same as startup screen
		
		static bool showUI;

		static int selResultAet1;
		static int selResultAet2;
		static int selResultAet3;
		static int selResultAet4;
		static int triangleAet;
		static int squareAet;
		static int crossAet;
		static int circleAet;
		
		enum menusets
		{
			MENUSET_MAIN = 0,
			MENUSET_SEVOL = 1,
			MENUSET_PV = 2,
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
		static std::vector<std::pair<menusets, int>> menuHistory; // used for implementing back button in menus
		static menusets curMenuSet;

		static std::chrono::time_point<std::chrono::high_resolution_clock> menuItemMoveTime; // used for animation

		// static void mainmenu() { setMenuPos(MENUSET_MAIN, 0); menuHistory.resize(0); };
		static void menuback()
		{
			if (menuHistory.size() == 0)
			{
				// mainmenu();
				unpause();
			}
			else
			{
				std::pair<menusets, int> menuPos = menuHistory.back();
				menuHistory.pop_back();

				setMenuPos(menuPos.first, menuPos.second, false);
			}
		}
		static void unpause() { pause = false; };
		static void restart()
		{
			// fix the percent calculation if restarted while in the challenge time
			bool isInChallengeTime = *(bool*)(0x140C94438 + 38);

			if (isInChallengeTime) {
				uint64_t baseA1 = 0x140cdd8d0;
				void(*doResetChallenge)(uint64_t, uint64_t, uint64_t) = (void(*)(uint64_t, uint64_t, uint64_t))0x14011B010;
				doResetChallenge(baseA1 + 180088, baseA1 + 180088, 0);

				// it is used in 14011BEA0 after the challenge time starts or ends
				// and looks like required to do the reset process properly
				*(bool*)(baseA1 + 180181) = 1;
			}

			/*
			140d0b510+2 = 0, 140d0b510+14 = 8 for restart

			also 140cdd8d0+8 (something less than 0x1a but idk what) -- controls a switch statement in FUN_1400fddc0

			0x18: PV seems good, audio good, chart and score broken? -- timing resets, notes don't
			0x14: PVs a little bugged (not major), chart and scoring fine, life still doesn't reset
			0x10: life resets now, but old graphics don't clear

			0x15: same as 0x14
			0x16: doesn't actually reset????
			0x17: crash
			0x12: like 0x10 but life isn't cleared

			case 0x11 seems to clear life
			case 0x15 seems to clear score (maybe note data)
			case 0x18 seems to fix timing

			patch 0x15 at 1401038cd (was 0x12), 0x18 at 140103b94 (was 0x16)
			then call with 0x11
			hopefully it'll work

			percentage doesn't fully reset by the looks of it????
			rip
			(actually was hold and slide scores -- fixed by manually clearing them now)
			*/

			// inject flow overrides to switch cases in FUN_1400fddc0
			InjectCode((void*)0x1401038cd, { 0x15 }); InjectCode((void*)0x140103b94, { 0x18 });

			bool hasTaskMovie[2] = { false, false };
			bool waitTaskMovie = false;
			size_t(*taskMovieGet)(int32_t) = (size_t(*)(int32_t))0x14041ED30;
			// reset movie position
			for (int i = 0; i < 2; i++) {
				size_t taskMovie = taskMovieGet(i);
				if (taskMovie) {
					size_t taskMoviePlayer = *(size_t*)(taskMovie + 0xB8);
					if (taskMoviePlayer) {
						size_t iPlayer = *(size_t*)(taskMoviePlayer + 0x10);
						if (iPlayer) {
							size_t iPlayerVtable = *(size_t*)(iPlayer + 0x00);
							int(*iPlayer__SetCurrentPosition)(size_t, double) = (int(*)(size_t, double)) * (size_t*)(iPlayerVtable + 0x60);
							iPlayer__SetCurrentPosition(iPlayer, 0.0);

							*(bool*)(taskMovie + 0xD1) = true;
							hasTaskMovie[i] = true;
							waitTaskMovie = true;
						}
					}
				}
			}

			// set parameters
			*(uint8_t*)PV_STATE_ADDRESS = 0; *(uint8_t*)PV_LOADING_STATE_ADDRESS = 8; *(int*)PV_INNER_LOADING_STATE_ADDRESS = 0x11;

			void(*doLoading)(uint64_t) = (void(*)(uint64_t))0x1400fddc0;

			// do loading until definitely done
			while (*(int*)PV_INNER_LOADING_STATE_ADDRESS < 0x18)
			{
				doLoading(0x140cdd8d0);
			}

			// wait till movie resets
			bool(*taskMovieCheckDisp)(size_t) = (bool(*)(size_t))0x14041F0E0;
			while (waitTaskMovie) {
				waitTaskMovie = false;

				for (int i = 0; i < 2; i++) {
					if (!hasTaskMovie[i])
						continue;

					size_t taskMovie = taskMovieGet(i);
					if (taskMovie && !taskMovieCheckDisp(taskMovie))
						waitTaskMovie = true;
				}
			}

			// for some reason the above doesn't reset all scoring stuff
			*(int*)0x140D0A9BC = 0; // total holds
			*(int*)0x140D0A9B8 = 0; // hold + multi
			*(int*)0x140D0A9C0 = 0; // slide
			*(uint8_t*)0x140D0A50C = 0; // not clear flag
			*(char*)0x140D0AA0F = 0; // chance time

			// revert patches and unpause
			InjectCode((void*)0x1401038cd, { 0x12 }); InjectCode((void*)0x140103b94, { 0x16 });
			unpause();
		}

		static void giveup()
		{
			ignoreAutoPause = false;
			autoReplay = false;
			giveUp = true;
		};

		static void sevolorpvmenu()
		{
			setMenuPos(isInPV() ? MENUSET_PV : MENUSET_SEVOL, 1);
		};

		static void pvloop() { autoReplay = !autoReplay; }
		static void pvignoreautopause() { ignoreAutoPause = !ignoreAutoPause; }
		static void sevolplus() { setSEVolume(10); };
		static void sevolminus() { setSEVolume(-10); };

		static bool isInPV(bool checkIsInGame = true)
		{
			if (checkIsInGame && !isInGame())
			{
				return false;
			}

			return *(char*)0x140C94438 == 2;
		}

		// contents are in Pause.cpp because they can't be inline here for a static (const) array/vec
		static std::vector<menuSet> menu;

		static void setMenuPos(menusets set, int pos, bool updateHistory = true);

		static float getMenuAnimPos();

		static TLAC::Utilities::Drawing::Point getMenuItemCoords(menusets set, int pos);
	};
}
