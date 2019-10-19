#include "Pause.h"
#include "../Constants.h"
#include "GameState.h"
#include "DrawText.h"
#include "Input/InputState.h"
#include "GL/glut.h"
#include <windows.h>
#include <vector>

namespace TLAC::Components
{
	bool Pause::isPaused = false;
	std::vector<uint8_t> Pause::origAetMovOp;
	std::vector<bool> Pause::streamPlayStates;

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
			FontInfo fontInfo(0x11);
			fontInfo.setSize(menuTextSize, menuTextSize);
			DrawTextParams dtParams(&fontInfo);

			dtParams.strokeColour = 0xff000000;

			dtParams.xBegin = menuX;
			dtParams.yBegin = menuY - menuItemHeight * (menuItems.size() / 2.0);

			for (int i = 0; i < menuItems.size(); i++)
			{
				std::pair<std::string, void(*)()> &item = menuItems[i];

				if (i == menuPos)
					dtParams.textColour = 0xffffff00;
				else
					dtParams.textColour = 0xffffffff;

				dtParams.xCurrent = dtParams.xBegin;
				dtParams.yCurrent = dtParams.yBegin;
				drawText(&dtParams, 8, item.first);
				dtParams.yBegin += menuItemHeight;
			}

			fontInfo.setSize(18, 18);

			dtParams.xBegin = menuX;
			dtParams.yBegin = 600;
			dtParams.xCurrent = dtParams.xBegin;
			dtParams.yCurrent = dtParams.yBegin;

			dtParams.textColour = 0xffffffff;

			drawTextW(&dtParams, 8, L"○:Select　×:Close　L/R:Move　□:Hide Menu");
		}
	}

	void Pause::UpdatePostDraw()
	{
		if (isPaused && showUI)
		{
			// this isn't imgui code, but I probably wouldn't have gotten this working properly if I didn't reference it, so...  umm...  yeah lol
			glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);
			glDisable(GL_COLOR_MATERIAL);

			int width = glutGet(GLUT_WINDOW_WIDTH);
			int height = glutGet(GLUT_WINDOW_HEIGHT);

			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, 1280, 720, 0, -1.0f, +1.0f);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glBegin(GL_QUADS);
			glBindTexture(GL_TEXTURE_2D, 0);

			glColor4ub(0, 0, 0, 64);
			glVertex2i(0, 0);
			glVertex2i(1280, 0);
			glVertex2i(1280, 720);
			glVertex2i(0, 720);

			const int pauseWidth = 80;
			const int pauseHeight = 110;
			const int pauseGap = 20;
			const int pausePosX = 32;
			const int pausePosY = 32;

			const int pauseX1 = pausePosX;
			const int pauseX2 = pauseX1 + (pauseWidth - pauseGap) / 2;
			const int pauseX3 = pauseX2 + pauseGap;
			const int pauseX4 = pauseX1 + pauseWidth;

			const int pauseY1 = pausePosY;
			const int pauseY2 = pauseY1 + pauseHeight;

			glColor4ub(255, 255, 255, 127);
			glVertex2i(pauseX1, pauseY1);
			glVertex2i(pauseX2, pauseY1);
			glVertex2i(pauseX2, pauseY2);
			glVertex2i(pauseX1, pauseY2);
			glVertex2i(pauseX3, pauseY1);
			glVertex2i(pauseX4, pauseY1);
			glVertex2i(pauseX4, pauseY2);
			glVertex2i(pauseX3, pauseY2);

			glEnd();
			glBegin(GL_QUAD_STRIP);

			int selectBoxOrigin = menuY - menuItemHeight * (menuItems.size() / 2.0) - (menuItemHeight - menuTextSize) / 2;
			int selectBoxPos = selectBoxOrigin + menuItemHeight * menuPos;
			const int selectBoxWidth = 200;
			const int selectBoxHeight = menuItemHeight;
			const int selectBoxThickness = 2;

			const int selectBoxX1 = menuX - selectBoxWidth / 2;
			const int selectBoxX4 = selectBoxX1 + selectBoxWidth;
			const int selectBoxX2 = selectBoxX1 + selectBoxThickness;
			const int selectBoxX3 = selectBoxX4 - selectBoxThickness;

			const int selectBoxY1 = selectBoxPos;
			const int selectBoxY4 = selectBoxY1 + selectBoxHeight;
			const int selectBoxY2 = selectBoxY1 + selectBoxThickness;
			const int selectBoxY3 = selectBoxY4 - selectBoxThickness;

			glColor4ub(0, 255, 255, 127);
			glVertex2i(selectBoxX2, selectBoxY2);
			glVertex2i(selectBoxX1, selectBoxY1);
			glVertex2i(selectBoxX3, selectBoxY2);
			glVertex2i(selectBoxX4, selectBoxY1);
			glVertex2i(selectBoxX3, selectBoxY3);
			glVertex2i(selectBoxX4, selectBoxY4);
			glVertex2i(selectBoxX2, selectBoxY3);
			glVertex2i(selectBoxX1, selectBoxY4);
			glVertex2i(selectBoxX2, selectBoxY2);
			glVertex2i(selectBoxX1, selectBoxY1);

			glEnd();
			
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glPopAttrib();
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
			*(float*)FRAME_SPEED_ADDRESS = 0.0f; // todo: hook getFrameSpeed so this can't be screwed up
			((void(*)())DSC_PAUSE_FUNC_ADDRESS)();

			InjectCode(aetMovPatchAddress, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

			for (int i = 0; i < nAudioStreams; i++)
			{
				uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
				if (i < streamPlayStates.size())
					streamPlayStates[i] = *playstate;
				else
					streamPlayStates.push_back(*playstate);

				*playstate = 0;
			}
		}
		else
		{
			*(float*)FRAME_SPEED_ADDRESS = 1.0f;
			((void(*)())DSC_UNPAUSE_FUNC_ADDRESS)();

			InjectCode(aetMovPatchAddress, origAetMovOp);

			for (int i = 0; i < nAudioStreams; i++)
			{
				uint32_t* playstate = (uint32_t*)(audioStreamsAddress + i * 0x50 + 0x18);
				if (i < streamPlayStates.size())
					*playstate = streamPlayStates[i];
			}
		}

		isPaused = pause;
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
