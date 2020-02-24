#include "Pause.h"
#include "../Constants.h"
#include "GameState.h"
#include "ComponentsManager.h"
#include "../Utilities/Drawing.h"
#include "GL/glut.h"
#include "detours.h"
#include <windows.h>
#include <vector>
#include <chrono>
#include "../FileSystem/ConfigFile.h"
#include "../framework.h"

namespace TLAC::Components
{
	using TLAC::Utilities::Drawing;
	bool Pause::pause = false;
	bool Pause::isPaused = false;
	bool Pause::giveUp = false;
	bool Pause::autoPause = false;
	bool Pause::showUI = true;
	int Pause::selResultAet1 = 0;
	int Pause::selResultAet2 = 0;
	int Pause::selResultAet3 = 0;
	int Pause::selResultAet4 = 0;
	int Pause::triangleAet = 0;
	int Pause::squareAet = 0;
	int Pause::crossAet = 0;
	int Pause::circleAet = 0;
	int Pause::curMenuPos = 0;
	std::vector<std::pair<Pause::menusets, int>> Pause::menuHistory;
	Pause::menusets Pause::curMenuSet = MENUSET_MAIN;
	std::chrono::time_point<std::chrono::high_resolution_clock> Pause::menuItemMoveTime;
	std::vector<uint8_t> Pause::origAetMovOp;
	uint8_t* Pause::aetMovPatchAddress = (uint8_t*)0x1401703b3;
	std::vector<uint8_t> Pause::origFramespeedOp;
	uint8_t* Pause::framespeedPatchAddress = (uint8_t*)0x140192D50;
	std::vector<uint8_t> Pause::origAgeageHairOp;
	uint8_t* Pause::ageageHairPatchAddress = (uint8_t*)0x14054352c;
	std::vector<bool> Pause::streamPlayStates;
	bool(*divaGiveUpFunc)(void*) = (bool(*)(void* cls))GIVEUP_FUNC_ADDRESS;
	PlayerData* Pause::playerData;
	InputState* Pause::inputState;
	TouchSliderState* Pause::sliderState;
	TouchPanelState* Pause::panelState;
	ComponentsManager* Pause::componentsManager;
	JvsButtons Pause::filteredButtons;
	int Pause::lastTouchType = 0;
	std::vector<Pause::menuSet> Pause::menu = {
		{
			"PAUSED",
			{
				{ "RESUME", unpause, false },
				{ "RESTART", restart, false },
				{ "SE VOLUME", sevolmenu, false },
				{ "GIVE UP", giveup, false },
			}
		},
		{
			"SE VOLUME",
			{
				{ "+", sevolplus, true },
				{ "XX", menuback, false },
				{ "-", sevolminus, true },
			}
		},
	};

	Pause::Pause()
	{
	}

	Pause::~Pause()
	{
	}

	const char* Pause::GetDisplayName()
	{
		return "pause";
	}

	void Pause::saveOldPatchOps()
	{
		origAetMovOp.resize(8);
		memcpy(origAetMovOp.data(), aetMovPatchAddress, 8);

		origFramespeedOp.resize(4);
		memcpy(origFramespeedOp.data(), framespeedPatchAddress, 4);

		origAgeageHairOp.resize(3);
		memcpy(origAgeageHairOp.data(), ageageHairPatchAddress, 3);
	}

	void Pause::Initialize(ComponentsManager* manager)
	{
		inputState = (InputState*)(*(uint64_t*)INPUT_STATE_PTR_ADDRESS);
		playerData = (PlayerData*)PLAYER_DATA_ADDRESS;
		sliderState = (TouchSliderState*)SLIDER_CTRL_TASK_ADDRESS;
		panelState = (TouchPanelState*)TASK_TOUCH_ADDRESS;
		componentsManager = manager;

		TLAC::FileSystem::ConfigFile config(TLAC::framework::GetModuleDirectory(), "keyconfig.ini");
		config.OpenRead();
		autoPause = config.GetBooleanValue("autopause");

		saveOldPatchOps();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)divaGiveUpFunc, hookedGiveUpFunc);
		DetourTransactionCommit();
	}

	void Pause::Update()
	{

	}

	void Pause::UpdatePostInput()
	{
		if (pause)
		{
			// enter pause mode on state transition
			if (!isPaused)
			{
				((void(*)())DSC_PAUSE_FUNC_ADDRESS)();

				saveOldPatchOps();
				InjectCode(aetMovPatchAddress, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
				InjectCode(framespeedPatchAddress, { 0x0f, 0x57, 0xc0, 0xc3 }); // XORPS XMM0,XMM0; RET
				InjectCode(ageageHairPatchAddress, { 0x0f, 0x57, 0xdb }); // XORPS XMM3,XMM3

				uint64_t audioMixerAddr = *(uint64_t*)(AUDIO_MAIN_CLASS_ADDRESS + 0x70);
				uint64_t audioStreamsAddress = *(uint64_t*)(audioMixerAddr + 0x18);
				int nAudioStreams = *(uint64_t*)(audioMixerAddr + 0x20);
				for (int i = 0; i < nAudioStreams; i++)
				{
					uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
					if (i < streamPlayStates.size())
						streamPlayStates[i] = *playstate;
					else
						streamPlayStates.push_back(*playstate);

					*playstate = 0;
				}

				// block all buttons from being passed to game
				filteredButtons = allButtons;

				setMenuPos(MENUSET_MAIN, 0);
				menuHistory.resize(0);
				showUI = true;
				isPaused = true;
			}

			// always exit pause if key is tapped or no longer in game somehow
			if (isPauseKeyTapped() || !isInGame())
			{
				pause = false;
			}
			else
			{
				if (inputState->Tapped.Buttons & JVS_SQUARE)
				{
					showUI = !showUI;
					if (showUI)
					{
						menuItemMoveTime = std::chrono::high_resolution_clock::now(); // restart animations
					}
					else
					{
						// clear these from the screen too
						/*
						Drawing::destroyAetLayer(selResultAet1);
						Drawing::destroyAetLayer(selResultAet2);
						Drawing::destroyAetLayer(selResultAet3);
						Drawing::destroyAetLayer(selResultAet4);
						*/
						Drawing::destroyAetLayer(triangleAet);
						Drawing::destroyAetLayer(squareAet);
						Drawing::destroyAetLayer(crossAet);
						Drawing::destroyAetLayer(circleAet);
					}
				}

				/* for testing `-ss` stuff
				if (inputState->Tapped.Buttons & JVS_TRIANGLE)
				{
					void** ScreenShotImplAddr = (void**)0x1412016d0;
					Drawing::MsString path; // just a small struct from TLAC::Utilities::Drawing::MsString
					path.SetCharBuf("blahblahblah");

					((void(*)(void* impl, Drawing::MsString* path, int width, int height))0x140557210)(*ScreenShotImplAddr, &path, 1280, 720);
				}
				*/

				// only process menu events when UI is visible
				if (showUI)
				{
					if (inputState->Tapped.Buttons & JVS_L)
						setMenuPos(curMenuSet, curMenuPos - 1);

					if (inputState->Tapped.Buttons & JVS_R)
						setMenuPos(curMenuSet, curMenuPos + 1);

					if (inputState->Tapped.Buttons & JVS_CROSS)
					{
						if (curMenuSet == MENUSET_MAIN)
						{
							pause = false;
						}
						else
						{
							menuback();
						}
					}

					if (inputState->Tapped.Buttons & JVS_CIRCLE)
						menu[curMenuSet].items[curMenuPos].callback();

					if (!componentsManager->IsDwGuiActive()) // not sure if this check is necessary, but it doesn't hurt
					{
						if (panelState->ContactType == 2 && lastTouchType != 2) // down and was not down before
						{
							for (int i = 0; i < menu[curMenuSet].items.size(); i++)
							{
								Drawing::Point itemCoords = getMenuItemCoords(curMenuSet, i);

								Drawing::Point touchCoords = { panelState->XPosition, panelState->YPosition };
								touchCoords.x *= 1280.0f / *(int*)RESOLUTION_WIDTH_ADDRESS; // convert to 720p coords
								touchCoords.y *= 720.0f / *(int*)RESOLUTION_HEIGHT_ADDRESS;

								if (touchCoords.x >= itemCoords.x - menuItemWidth / 2 &&
									touchCoords.x <= itemCoords.x + menuItemWidth / 2 &&
									touchCoords.y >= itemCoords.y - menuItemHeight / 2 &&
									touchCoords.y <= itemCoords.y + menuItemHeight / 2)
								{
									setMenuPos(curMenuSet, i);
									menu[curMenuSet].items[i].callback();
									break;
								}
							}
						}
					}

					if (curMenuSet == MENUSET_SEVOL)
					{
						const char volformat[] = "%d";
						size_t size = snprintf(nullptr, 0, volformat, playerData->act_vol) + 1;
						char* buf = new char[size];
						snprintf(buf, size, volformat, playerData->act_vol);
						menu[MENUSET_SEVOL].items[1].name = buf;
						delete[] buf;
					}
				}
			}

			// no slider while paused
			// this is simpler than buttons because slider doesn't trigger on press, it triggers on movement
			// (therefore per-button blocking isn't needed)
			sliderState->ResetSensors();
		}
		else
		{
			// exit pause mode on state transition
			if (isPaused)
			{
				((void(*)())DSC_UNPAUSE_FUNC_ADDRESS)();

				InjectCode(aetMovPatchAddress, origAetMovOp);
				InjectCode(framespeedPatchAddress, origFramespeedOp);
				InjectCode(ageageHairPatchAddress, origAgeageHairOp);

				uint64_t audioMixerAddr = *(uint64_t*)(AUDIO_MAIN_CLASS_ADDRESS + 0x70);
				uint64_t audioStreamsAddress = *(uint64_t*)(audioMixerAddr + 0x18);
				int nAudioStreams = *(uint64_t*)(audioMixerAddr + 0x20);
				for (int i = 0; i < nAudioStreams; i++)
				{
					uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
					if (i < streamPlayStates.size())
						*playstate = streamPlayStates[i];
				}

				/*
				Drawing::destroyAetLayer(selResultAet1);
				Drawing::destroyAetLayer(selResultAet2);
				Drawing::destroyAetLayer(selResultAet3);
				Drawing::destroyAetLayer(selResultAet4);
				*/
				Drawing::destroyAetLayer(triangleAet);
				Drawing::destroyAetLayer(squareAet);
				Drawing::destroyAetLayer(crossAet);
				Drawing::destroyAetLayer(circleAet);

				isPaused = false;
			}

			// buttons that have been tapped no longer need to be filtered (doing this with down broke retriggering)
			filteredButtons = (JvsButtons)(filteredButtons & ~inputState->Tapped.Buttons);

			// only enter pause if in game
			if (isInGame())
			{
				if (isPauseKeyTapped())
				{
					pause = true;
				}
			}
			else
			{
				// ensure giveUp isn't retained after already quitting
				giveUp = false;
			}
		}

		// swallow filtered button inputs
		inputState->Tapped.Buttons = (JvsButtons)(inputState->Tapped.Buttons & ~filteredButtons);
		inputState->DoubleTapped.Buttons = (JvsButtons)(inputState->DoubleTapped.Buttons & ~filteredButtons);
		inputState->Down.Buttons = (JvsButtons)(inputState->Down.Buttons & ~filteredButtons);
		inputState->Released.Buttons = (JvsButtons)(inputState->Released.Buttons & ~filteredButtons);
		inputState->IntervalTapped.Buttons = (JvsButtons)(inputState->IntervalTapped.Buttons & ~filteredButtons);

		lastTouchType = panelState->ContactType;
	}

	// returns the midpoint of a menu button
	Drawing::Point Pause::getMenuItemCoords(menusets set, int pos)
	{
		const float slant = 35.0f / 199.0f;

		int menuOriginY = menuY - (menuItemTotalHeight * menu[set].items.size() - menuItemPadding) / 2 + menuItemHeight / 2;
		if (set != MENUSET_MAIN)
		{
			menuOriginY += menuItemTotalHeight * 1.2 / 2;
		}
		int menuOriginX = menuX + (menuY - menuOriginY) * slant;

		Drawing::Point out;
		out.x = menuOriginX - (menuItemTotalHeight * pos * slant);
		out.y = menuOriginY + menuItemTotalHeight * pos;

		return out;
	}

	void Pause::UpdateDraw2D()
	{
		if (isPaused && showUI)
		{
			// setup draw objects
			Drawing::FontInfo fontInfo(0x11);
			Drawing::DrawParams dtParams(&fontInfo);
			dtParams.layer = bgLayer;
			Drawing::Point aetScale = { 0.65, 0.65 };
			Drawing::Point aetLoc = { (1280 - aetScale.x * 1280) / 2, (720 - aetScale.y * 720) / 2 };

			// get aspect ratio
			float aspect = *(float*)UI_ASPECT_RATIO;


			// bg rect
			float bgWidth = aspect * 720 + 2; // add a couple of pixels to protect against rounding errors
			float bgLeft = -(bgWidth - 1280) / 2; // 0,0 is in the corner of a 720p view.. half of the extra over 1280 wide is the horizontal offset to centre the bg
			Drawing::RectangleBounds rect;
			rect = { bgLeft, 0, bgWidth, 720 };
			dtParams.colour = 0xc0000000;
			dtParams.fillColour = dtParams.colour;
			Drawing::fillRectangle(&dtParams, rect);


			// pause icon
			/*
			const int pauseWidth = 80;
			const int pauseHeight = 110;
			const int pauseGap = 20;
			const int pausePosX = 32;
			const int pausePosY = 32;
			const int pausePartWidth = (pauseWidth - pauseGap) / 2;

			const int pauseX1 = pausePosX;
			const int pauseX2 = pausePosX + pausePartWidth + pauseGap;
			const int pauseY1 = pausePosY;

			dtParams.colour = 0x80ffffff;
			dtParams.fillColour = 0x80ffffff;
			rect = { pauseX1, pauseY1, pausePartWidth, pauseHeight };
			Drawing::fillRectangle(&dtParams, &rect);
			rect = { pauseX2, pauseY1, pausePartWidth, pauseHeight };
			Drawing::fillRectangle(&dtParams, &rect);
			*/


			dtParams.layer = contentLayer;

			// bg box
			/*
			static int selResultFile = 0;
			if (selResultFile == 0)
			{
				//keyWinFile = Drawing::findAetFileId("AET_KEY_WIN_MAIN");
				selResultFile = Drawing::findAetFileId("AET_SEL_RESULT_MAIN");
				//printf("keyWinFile: %d\n", keyWinFile);
			}

			Drawing::destroyAetLayer(selResultAet1);
			Drawing::destroyAetLayer(selResultAet2);
			Drawing::destroyAetLayer(selResultAet3);
			Drawing::destroyAetLayer(selResultAet4);
			if (selResultFile != -1)
			{
				//keyWinAet = Drawing::createAetLayer(keyWinFile, dtParams.layer, CREATEAET_20000, "win_in", aetLoc, 0, nullptr, nullptr, 3.7, 3.7, aetScale, 0);
				aetScale = { 0.47f, 0.47f };
				aetLoc = { (1280 - aetScale.x * 1280) / 2, (720 - aetScale.y * 720) / 2 + 72 * aetScale.y };
				float animPos = 7.2;
				selResultAet1 = Drawing::createAetLayer(selResultFile, dtParams.layer, CREATEAET_20000, "window_in", aetLoc, 0, nullptr, nullptr, animPos, animPos, aetScale, 0);
				selResultAet2 = Drawing::createAetLayer(selResultFile, dtParams.layer, CREATEAET_20000, "window_in", aetLoc, 0, nullptr, nullptr, animPos, animPos, aetScale, 0);
				selResultAet3 = Drawing::createAetLayer(selResultFile, dtParams.layer, CREATEAET_20000, "window_in", aetLoc, 0, nullptr, nullptr, animPos, animPos, aetScale, 0);
				selResultAet4 = Drawing::createAetLayer(selResultFile, dtParams.layer, CREATEAET_20000, "window_in", aetLoc, 0, nullptr, nullptr, animPos, animPos, aetScale, 0);
			}
			*/
			
			dtParams.colour = 0xffffff00;
			Drawing::drawPolyline(&dtParams, {
				{ 537, 255 },
				{ 794, 255 },
				{ 759, 454 },
				{ 744, 466 },
				{ 487, 466 },
				{ 522, 267 },
				{ 537, 255 },
			});
			Drawing::drawPolyline(&dtParams, {
				{ 537 - .7f, 255 - 2 },
				{ 794 + 2.38f, 255 - 2 },
				{ 759 + 2.1f, 454 + 1.15f },
				{ 744 + .7f, 466 + 2 },
				{ 487 - 2.38f, 466 + 2 },
				{ 522 - 2.1f, 267 - 1.15f },
				{ 537 - .7f, 255 - 2 },
			});
			Drawing::drawPolyline(&dtParams, {
				{ 548, 258 },
				{ 768, 258 },
				{ 774, 265 },
				{ 770, 286 },
				{ 764, 286 },
				{ 737, 435 },
				{ 743, 435 },
				{ 739, 456 },
				{ 733, 463 },
				{ 513, 463 },
				{ 507, 456 },
				{ 511, 435 },
				{ 517, 435 },
				{ 544, 286 },
				{ 538, 286 },
				{ 542, 265 },
				{ 548, 258 },
			});


			Drawing::Point menuCoords = getMenuItemCoords(curMenuSet, curMenuPos);
			// selection cursor
			const float selectBoxWidth = menuItemWidth;
			const float selectBoxHeight = menuItemHeight;
			const float selectBoxThickness = 2;

			float selectBoxX = menuCoords.x - selectBoxWidth / 2;
			float selectBoxY = menuCoords.y - menuItemHeight / 2;

			dtParams.colour = 0xc0ffff00;
			dtParams.fillColour = 0xc0ffff00;
			//rect = { selectBoxX, selectBoxY, selectBoxWidth, selectBoxHeight };
			//Drawing::drawRectangle(&dtParams, rect, selectBoxThickness);

			const float slant = 35.0f / 199.0f;
			Drawing::drawPolyline(&dtParams, {
				{ selectBoxX + (selectBoxHeight - 8) * slant + 8 * 15 / 12, selectBoxY },
				{ selectBoxX + selectBoxWidth, selectBoxY },
				{ selectBoxX + selectBoxWidth - (selectBoxHeight - 8) * slant, selectBoxY + selectBoxHeight - 8 },
				{ selectBoxX + selectBoxWidth - (selectBoxHeight - 8) * slant - 8 * 15 / 12, selectBoxY + selectBoxHeight },
				{ selectBoxX, selectBoxY + selectBoxHeight },
				{ selectBoxX + (selectBoxHeight - 8) * slant, selectBoxY + 8 },
				{ selectBoxX + (selectBoxHeight - 8) * slant + 8 * 15 / 12, selectBoxY },
			});
			Drawing::drawPolyline(&dtParams, {
				{ selectBoxX + (selectBoxHeight - 8) * slant + 8 * 15 / 12 - .7f, selectBoxY - 2 },
				{ selectBoxX + selectBoxWidth + 2.38f, selectBoxY - 2 },
				{ selectBoxX + selectBoxWidth - (selectBoxHeight - 8) * slant + 2.1f, selectBoxY + selectBoxHeight - 8 + 1.15f },
				{ selectBoxX + selectBoxWidth - (selectBoxHeight - 8) * slant - 8 * 15 / 12 + .7f, selectBoxY + selectBoxHeight + 2 },
				{ selectBoxX - 2.38f, selectBoxY + selectBoxHeight + 2 },
				{ selectBoxX + (selectBoxHeight - 8) * slant - 2.1f, selectBoxY + 8 - 1.15f },
				{ selectBoxX + (selectBoxHeight - 8) * slant + 8 * 15 / 12 - .7f, selectBoxY - 2 },
			});


			// menu
			fontInfo.setSize(menuTextSize, menuTextSize);

			if (curMenuSet != MENUSET_MAIN)
			{
				dtParams.textCurrentLoc = { menuX + 90 * slant, menuY - 90 };
				dtParams.lineOriginLoc.y = dtParams.textCurrentLoc.y;
				dtParams.colour = 0xffffffff;
				Drawing::drawText(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ALIGN_CENTRE | Drawing::DRAWTEXT_STROKE), menu[curMenuSet].name);
			}
			
			for (int i = 0; i < menu[curMenuSet].items.size(); i++)
			{
				menuCoords = getMenuItemCoords(curMenuSet, i);
				dtParams.textCurrentLoc = { menuCoords.x, menuCoords.y - menuTextSize / 2 };
				dtParams.lineOriginLoc = dtParams.textCurrentLoc;
				if (i == curMenuPos)
				{
					uint8_t alpha = (cosf(getMenuAnimPos() * 6.283185f) * 0.15 + 0.85) * 255;
					dtParams.colour = 0x00ffff00 | (alpha << 24);
				}
				else
				{
					dtParams.colour = 0xffffffff;
				}

				Drawing::drawText(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ALIGN_CENTRE), menu[curMenuSet].items[i].name);
			}
						

			// key legend
			Drawing::destroyAetLayer(triangleAet);
			Drawing::destroyAetLayer(squareAet);
			Drawing::destroyAetLayer(crossAet);
			Drawing::destroyAetLayer(circleAet);

			float textLeft;
			if (aspect > 16.0f / 9.0f)
				textLeft = 32;
			else
				textLeft = (1280 - bgWidth) / 2 + 32; // 0,0 is in the corner of a 720p view.. half of the difference to 1280 wide is the horizontal offset to the window corner

			fontInfo.setSize(18, 18);
			dtParams.textCurrentLoc = { textLeft, 720 - 40 };
			dtParams.lineOriginLoc = dtParams.textCurrentLoc;
			const float spriteSize = 18;
			const float halfSpriteSize = spriteSize / 2;
			aetLoc = { 0, dtParams.textCurrentLoc.y + halfSpriteSize }; // the aets are centered on their location, so fudge this a little
			aetScale = { spriteSize / 64, spriteSize / 64 }; // just approximated

			dtParams.colour = 0xffffffff;
			Drawing::drawTextW(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ENABLE_XADVANCE), L"↑↓/ＬＲ:Move　");
			
			aetLoc.x = dtParams.textCurrentLoc.x + halfSpriteSize;
			squareAet = Drawing::createAetLayer(3, dtParams.layer, Drawing::CREATEAET_20000, "button_shikaku", aetLoc, 0, nullptr, nullptr, 0, 0, aetScale, nullptr);
			dtParams.textCurrentLoc.x += spriteSize;
			Drawing::drawTextW(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ENABLE_XADVANCE), L":Hide Menu　");

			aetLoc.x = dtParams.textCurrentLoc.x + halfSpriteSize;
			crossAet = Drawing::createAetLayer(3, dtParams.layer, Drawing::CREATEAET_20000, "button_batsu", aetLoc, 0, nullptr, nullptr, 0, 0, aetScale, nullptr);
			dtParams.textCurrentLoc.x += spriteSize;
			if (menuHistory.size() == 0)
				Drawing::drawTextW(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ENABLE_XADVANCE), L":Close　");
			else
				Drawing::drawTextW(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ENABLE_XADVANCE), L":Back　");
			
			aetLoc.x = dtParams.textCurrentLoc.x + halfSpriteSize;
			circleAet = Drawing::createAetLayer(3, dtParams.layer, Drawing::CREATEAET_20000, "button_maru", aetLoc, 0, nullptr, nullptr, 0, 0, aetScale, nullptr);
			dtParams.textCurrentLoc.x += spriteSize;
			Drawing::drawTextW(&dtParams, (Drawing::drawTextFlags)(Drawing::DRAWTEXT_ENABLE_XADVANCE), L":Select");
		}
	}

	void Pause::OnFocusLost()
	{
		if (autoPause && isInGame())
			pause = true;
	}

	bool Pause::isPauseKeyTapped()
	{
		return inputState->Tapped.Buttons & JVS_START;
	}

	bool Pause::isInGame()
	{
		return *(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN && *(uint8_t*)PV_STATE_ADDRESS == 1;
	}

	bool Pause::hookedGiveUpFunc(void* cls)
	{
		if (giveUp)
		{
			giveUp = false;
			pause = false;
			return true;
		}
		else
		{
			if (divaGiveUpFunc(cls))
			{
				pause = false;
				return true;
			}
		}
		return false;
	}

	void Pause::setSEVolume(int amount)
	{
		playerData->act_vol += amount;
		if (playerData->act_vol < 0) playerData->act_vol = 0;
		if (playerData->act_vol > 100) playerData->act_vol = 100;
		playerData->act_slide_vol = playerData->act_vol;
	}

	void Pause::setMenuPos(menusets set, int pos, bool updateHistory)
	{
		menusets newMenuSet;

		if (set >= 0 && set < menu.size())
			newMenuSet = set;
		else
			newMenuSet = MENUSET_MAIN;

		if (updateHistory && newMenuSet != curMenuSet)
			menuHistory.push_back(std::pair<menusets, int>(curMenuSet, curMenuPos));

		curMenuSet = newMenuSet;

		if (pos < 0)
			pos = menu[curMenuSet].items.size() - 1;
		else if (pos >= menu[curMenuSet].items.size())
			pos = 0;

		curMenuPos = pos;

		menuItemMoveTime = std::chrono::high_resolution_clock::now(); // restart animations
	}

	float Pause::getMenuAnimPos()
	{
		const int duration = 1500000000; // 1.5s
		return (float)((std::chrono::high_resolution_clock::now() - menuItemMoveTime) % std::chrono::nanoseconds(duration)).count() / (float)duration;
	}

	void Pause::InjectCode(void* address, const std::vector<uint8_t> data)
	{
		const size_t byteCount = data.size() * sizeof(uint8_t);

		DWORD oldProtect;
		VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
		memcpy(address, data.data(), byteCount);
		VirtualProtect(address, byteCount, oldProtect, nullptr);
	}
}
