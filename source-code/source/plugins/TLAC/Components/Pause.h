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

	private:
		bool isPaused;
		bool isPauseKeyTapped();
		void setPaused(bool pause);
		std::vector<bool> streamPlayStates;
		void InjectCode(void* address, const std::vector<uint8_t> data);

		std::vector<uint8_t> origAetMovOp;
		uint8_t* aetMovPatchAddress = (uint8_t*)0x1401703b3;
	};
}
