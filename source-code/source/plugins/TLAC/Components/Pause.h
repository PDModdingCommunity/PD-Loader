#pragma once
#include "../Constants.h"
#include "EmulatorComponent.h"
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

		static bool isPaused;
		static bool giveUp;
		static void setPaused(bool pause);
	private:
		// this is a mess of static so that menuItems can work
		static bool isPauseKeyTapped();
		static std::vector<bool> streamPlayStates;
		static void InjectCode(void* address, const std::vector<uint8_t> data);

		static std::vector<uint8_t> origAetMovOp;
		static constexpr uint8_t* aetMovPatchAddress = (uint8_t*)0x1401703b3;

		static std::vector<uint8_t> origFramespeedOp;
		static constexpr uint8_t* framespeedPatchAddress = (uint8_t*)0x140192D50;

		static bool hookedGiveUpFunc(void* cls);

		static const int menuX = 640;
		static const int menuY = 360;
		static const int menuItemHeight = 36;
		static const int menuTextSize = 24;
		
		static bool showUI;
		static unsigned int menuPos;

		static std::chrono::time_point<std::chrono::high_resolution_clock> menuItemSelectTime;

		static void unpause() { setPaused(false); };
		static void giveup() { giveUp = true; };

		std::vector<std::pair<std::string, void(*)()>> menuItems = {
			std::pair<std::string, void(*)()>(std::string("RESUME"), &unpause),
			std::pair<std::string, void(*)()>(std::string("GIVE UP"), &giveup),
		};
	};
}
