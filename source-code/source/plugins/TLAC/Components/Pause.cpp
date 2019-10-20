#include "Pause.h"
#include "../Constants.h"
#include "GameState.h"
#include "Drawing.h"
#include "Input/InputState.h"
#include "GL/glut.h"
#include "detours.h"
#include <windows.h>
#include <vector>

namespace TLAC::Components
{
	bool Pause::isPaused = false;
	bool Pause::giveUp = false;
	bool Pause::showUI = true;
	unsigned int Pause::menuPos = 0;
	std::vector<uint8_t> Pause::origAetMovOp;
	std::vector<uint8_t> Pause::origFramespeedOp;
	std::vector<bool> Pause::streamPlayStates;
	bool(*divaGiveUpFunc)(void*) = (bool(*)(void* cls))GIVEUP_FUNC_ADDRESS;

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

	void Pause::Initialize(ComponentsManager*)
	{
		origAetMovOp.resize(8);
		memcpy(&origAetMovOp[0], aetMovPatchAddress, 8);

		origFramespeedOp.resize(4);
		memcpy(&origFramespeedOp[0], framespeedPatchAddress, 4);

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
		if (isPaused)
		{
			// exit pause if key is tapped or no longer in game somehow
			if (isUnpauseKeyTapped() || *(GameState*)CURRENT_GAME_STATE_ADDRESS != GS_GAME || *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS != SUB_GAME_MAIN)
			{
				setPaused(false);
			}
			else
			{
				InputState* inputState = (InputState*)(*(uint64_t*)INPUT_STATE_PTR_ADDRESS);

				if (inputState->Tapped.Buttons & JVS_SQUARE)
					showUI = !showUI;

				if (inputState->Tapped.Buttons & JVS_L)
					menuPos -= 1;

				if (inputState->Tapped.Buttons & JVS_R)
					menuPos += 1;

				menuPos %= menuItems.size();

				// actually use released when unpausing from a face button so it doesn't trigger input
				// (this can trigger an unpause too)
				if (inputState->Released.Buttons & JVS_CIRCLE)
					menuItems[menuPos].second();


				// swallow all button inputs if paused
				inputState->Tapped.Buttons = JVS_NONE;
				inputState->DoubleTapped.Buttons = JVS_NONE;
				inputState->Down.Buttons = JVS_NONE;
				inputState->Released.Buttons = JVS_NONE;
				inputState->IntervalTapped.Buttons = JVS_NONE;
				// todo: slider
			}
		}
		else
		{
			// only enter pause if in game
			if (*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN)
			{
				if (isPauseKeyTapped())
				{
					setPaused(true);
				}
			}
		}
	}

	void Pause::UpdateDrawSprites()
	{
		if (isPaused && showUI)
		{
			// setup draw objects
			FontInfo fontInfo(0x11);
			DrawParams dtParams(&fontInfo);
			dtParams.layer = 0x19; // same as startup screen


			// bg rect
			RectangleBounds rect;
			rect = { 0, 0, 1280, 720 };
			dtParams.colour = 0x80000000;
			dtParams.fillColour = 0x80000000;
			fillRectangle(&dtParams, &rect);


			// pause icon
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
			fillRectangle(&dtParams, &rect);
			rect = { pauseX2, pauseY1, pausePartWidth, pauseHeight };
			fillRectangle(&dtParams, &rect);


			// selection cursor
			int selectBoxOrigin = menuY - menuItemHeight * (menuItems.size() / 2.0) - (menuItemHeight - menuTextSize) / 2;
			int selectBoxPos = selectBoxOrigin + menuItemHeight * menuPos;
			const float selectBoxWidth = 200;
			const float selectBoxHeight = menuItemHeight;
			const float selectBoxThickness = 2;

			const float selectBoxX1 = menuX - selectBoxWidth / 2;
			const float selectBoxX2 = selectBoxX1 + selectBoxWidth - selectBoxThickness;
			const float selectBoxY1 = selectBoxPos;
			const float selectBoxY2 = selectBoxY1 + selectBoxThickness;
			const float selectBoxY3 = selectBoxY1 + selectBoxHeight - selectBoxThickness;
			const float selectBoxSideHeight = selectBoxHeight - 2 * selectBoxThickness;

			// dirty hack using fillRectangle to get thickness (idk how to set it or it isn't possible to with drawRectangle)
			dtParams.colour = 0xc0ffff00;
			dtParams.fillColour = 0xc0ffff00;
			rect = { selectBoxX1, selectBoxY1, selectBoxWidth, selectBoxThickness }; // top
			fillRectangle(&dtParams, &rect);
			rect = { selectBoxX1, selectBoxY3, selectBoxWidth, selectBoxThickness }; // bottom
			fillRectangle(&dtParams, &rect);
			rect = { selectBoxX1, selectBoxY2, selectBoxThickness, selectBoxSideHeight };
			fillRectangle(&dtParams, &rect);
			rect = { selectBoxX2, selectBoxY2, selectBoxThickness, selectBoxSideHeight };
			fillRectangle(&dtParams, &rect);


			// menu
			fontInfo.setSize(menuTextSize, menuTextSize);

			dtParams.xBegin = menuX;
			dtParams.yBegin = menuY - menuItemHeight * (menuItems.size() / 2.0);

			for (int i = 0; i < menuItems.size(); i++)
			{
				std::pair<std::string, void(*)()> &item = menuItems[i];

				if (i == menuPos)
					dtParams.colour = 0xffffff00;
				else
					dtParams.colour = 0xffffffff;

				dtParams.xCurrent = dtParams.xBegin;
				dtParams.yCurrent = dtParams.yBegin;
				drawText(&dtParams, (drawTextFlags)(DRAWTEXT_ALIGN_CENTRE), item.first);
				dtParams.yBegin += menuItemHeight;
			}


			// key legend
			fontInfo.setSize(18, 18);

			dtParams.xBegin = menuX;
			dtParams.yBegin = 600;
			dtParams.xCurrent = dtParams.xBegin;
			dtParams.yCurrent = dtParams.yBegin;
			dtParams.colour = 0xffffffff;
			drawTextW(&dtParams, (drawTextFlags)(DRAWTEXT_ALIGN_CENTRE), L"○:Select　×:Close　L/R:Move　□:Hide Menu");
		}
	}

	bool Pause::isPauseKeyTapped()
	{
		return ((InputState*)(*(uint64_t*)INPUT_STATE_PTR_ADDRESS))->Tapped.Buttons & JVS_START;
	}

	bool Pause::isUnpauseKeyTapped()
	{
		// actually use released when unpausing from a face button so it doesn't trigger input
		return ((InputState*)(*(uint64_t*)INPUT_STATE_PTR_ADDRESS))->Tapped.Buttons & JVS_START ||
			((InputState*)(*(uint64_t*)INPUT_STATE_PTR_ADDRESS))->Released.Buttons & JVS_CROSS;
	}

	void Pause::setPaused(bool pause)
	{
		uint64_t audioMixerAddr = *(uint64_t*)(AUDIO_MAIN_CLASS_ADDRESS + 0x70);
		uint64_t audioStreamsAddress = *(uint64_t*)(audioMixerAddr + 0x18);;
		int nAudioStreams = *(uint64_t*)(audioMixerAddr + 0x20);

		if (pause)
		{
			((void(*)())DSC_PAUSE_FUNC_ADDRESS)();

			InjectCode(aetMovPatchAddress, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode(framespeedPatchAddress, { 0x0f, 0x57, 0xc0, 0xc3 }); // XORPS XMM0,XMM0; RET

			for (int i = 0; i < nAudioStreams; i++)
			{
				uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
				if (i < streamPlayStates.size())
					streamPlayStates[i] = *playstate;
				else
					streamPlayStates.push_back(*playstate);

				*playstate = 0;
			}

			menuPos = 0;
			showUI = true;
		}
		else
		{
			((void(*)())DSC_UNPAUSE_FUNC_ADDRESS)();

			InjectCode(aetMovPatchAddress, origAetMovOp);
			InjectCode(framespeedPatchAddress, origFramespeedOp);

			for (int i = 0; i < nAudioStreams; i++)
			{
				uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
				if (i < streamPlayStates.size())
					*playstate = streamPlayStates[i];
			}
		}

		isPaused = pause;
	}

	bool Pause::hookedGiveUpFunc(void* cls)
	{
		if (giveUp)
		{
			giveUp = false;
			setPaused(false);
			return true;
		}
		else
		{
			if (divaGiveUpFunc(cls))
			{
				setPaused(false);
				return true;
			}
		}
		return false;
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
