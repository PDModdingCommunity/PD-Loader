#pragma once
#include "EmulatorComponent.h"
#include <vector>

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
		virtual void UpdatePostDraw() override;

		static bool isPaused;
	private:
		// this is a mess of static so that menuItems can work
		static bool isPauseKeyTapped();
		static bool isUnpauseKeyTapped();
		static void setPaused(bool pause);
		static std::vector<bool> streamPlayStates;
		static void InjectCode(void* address, const std::vector<uint8_t> data);

		static std::vector<uint8_t> origAetMovOp;
		static constexpr uint8_t* aetMovPatchAddress = (uint8_t*)0x1401703b3;

		static const int menuX = 640;
		static const int menuY = 360;
		static const int menuItemHeight = 36;
		static const int menuTextSize = 24;
		
		bool showUI = true;
		unsigned int menuPos = 0;
		static void unpause() { setPaused(false); };
		std::vector<std::pair<std::string, void(*)()>> menuItems = {
			std::pair<std::string, void(*)()>(std::string("RESUME"), &unpause),
			//std::pair<std::string, void(*)()>(std::string("GIVE UP"), &unpause),
		};
	};
}
