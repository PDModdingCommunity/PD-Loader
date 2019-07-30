#pragma once
#include "EmulatorComponent.h"
#include "../Utilities/Stopwatch.h"
#include <vector>
#include <string>

namespace TLAC::Components
{
	const std::string COMPONENTS_CONFIG_FILE_NAME = "components.ini";

	// Incomplete type
	struct DwGuiDisplay
	{
		void* vftable;
		void* active;
		void* cap;
		void* on;
		void* move;
		void* widget;
	};

	class ComponentsManager
	{
	public:
		ComponentsManager();
		~ComponentsManager();
		void Initialize();
		void Update();
		void UpdateInput();
		void OnFocusGain();
		void OnFocusLost();
		void Dispose();

		inline bool GetIsInputEmulatorUsed() { return isInputEmulatorUsed; };
		inline void SetIsInputEmulatorUsed(bool value) { isInputEmulatorUsed = value; };

		inline bool GetUpdateGameInput() { return updateGameInput; };
		inline void SetUpdateGameInput(bool value) { updateGameInput = value; }

		inline bool IsDwGuiActive() { return dwGuiDisplay->active != nullptr; };
		inline bool IsDwGuiHovered() { return dwGuiDisplay->on != nullptr; };

	private:
		DwGuiDisplay* dwGuiDisplay;

		bool isInputEmulatorUsed = false;
		bool updateGameInput = true;

		float elpasedTime;
		Utilities::Stopwatch updateStopwatch;
		std::vector<EmulatorComponent*> components;

		void ParseAddComponents();
	};
}