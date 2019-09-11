#pragma once
#include "../EmulatorComponent.h"
#include "TouchSliderState.h"
#include "../../Input/Bindings/Binding.h"

namespace TLAC::Components
{
	constexpr int SLIDER_INPUTS = 4;
	constexpr int CONTACT_POINTS = 2;

	struct ContactPoint
	{
		float Position;
		bool InContact;
	};

	class TouchSliderEmulator : public EmulatorComponent
	{
	public:
		static TouchSliderEmulator* LatestInstance;

		Input::Binding* LeftSideSlideLeft;
		Input::Binding* LeftSideSlideRight;

		Input::Binding* RightSideSlideLeft;
		Input::Binding* RightSideSlideRight;

		TouchSliderEmulator();
		~TouchSliderEmulator();

		virtual const char* GetDisplayName() override;

		virtual void Initialize(ComponentsManager*) override;
		virtual void Update() override;
		virtual void UpdateInput() override;

		virtual void OnFocusLost() override;

		bool isSliderTouched()
		{
			for (int i = 0; i < 32; i++)
				if (sliderState->SensorTouched[i].IsTouched) return true;

			return false;
		}

	private:
		ComponentsManager* componentsManager;
		float sliderSpeed = 750.0f;
		float sliderIncrement;

		TouchSliderState *sliderState;
		ContactPoint ContactPoints[CONTACT_POINTS];

		void EmulateSliderInput(Input::Binding *leftBinding, Input::Binding *rightBinding, ContactPoint &contactPoint, float start, float end);
		void ApplyContactPoint(ContactPoint &contactPoint, int section);
	};
}

