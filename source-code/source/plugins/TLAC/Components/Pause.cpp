#include "Pause.h"
#include "../Constants.h"
#include "GameState.h"
#include "Input/InputState.h"
#include "GL\glut.h"
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
			if (isPauseKeyTapped() || *(GameState*)CURRENT_GAME_STATE_ADDRESS != GS_GAME || *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS != SUB_GAME_MAIN)
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

			glColor4ub(0, 0, 0, 127);
			glVertex2i(0, 0);
			glVertex2i(1280, 0);
			glVertex2i(1280, 720);
			glVertex2i(0, 720);

			glColor4ub(255, 255, 255, 127);
			glVertex2i(560, 250);
			glVertex2i(620, 250);
			glVertex2i(620, 470);
			glVertex2i(560, 470);
			glVertex2i(660, 250);
			glVertex2i(720, 250);
			glVertex2i(720, 470);
			glVertex2i(660, 470);

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
