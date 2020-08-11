#include "TouchSliderEmulator.h"
#include "../ComponentsManager.h"
#include "../../Constants.h"
#include "../../framework.h"
#include "../../Input/Mouse/Mouse.h"
#include "../../Input/Keyboard/Keyboard.h"
#include "../../Input/Bindings/KeyboardBinding.h"
#include "../../Input/KeyConfig/Config.h"
#include "../../Input/DirectInput/Ds4/DualShock4.h"
#include "../../FileSystem/ConfigFile.h"
#include "../../Utilities/Math.h"
#include <algorithm>
#include <stdio.h>
#include "../GameState.h"

using namespace TLAC::Input;
using namespace TLAC::Input::KeyConfig;
using namespace TLAC::Utilities;

namespace TLAC::Components
{
	const std::string KEY_CONFIG_FILE_NAME = "keyconfig.ini";

	TouchSliderEmulator* TouchSliderEmulator::LatestInstance;

	TouchSliderEmulator::TouchSliderEmulator()
	{
		LatestInstance = this;
	}

	TouchSliderEmulator::~TouchSliderEmulator()
	{
		delete LeftSideSlideLeft;
		delete LeftSideSlideRight;

		delete RightSideSlideLeft;
		delete RightSideSlideRight;
	}

	const char* TouchSliderEmulator::GetDisplayName()
	{
		return "touch_slider_emulator";
	}

	void TouchSliderEmulator::Initialize(ComponentsManager* manager)
	{
		componentsManager = manager;
		sliderState = (TouchSliderState*)SLIDER_CTRL_TASK_ADDRESS;

		LeftSideSlideLeft = new Binding();
		LeftSideSlideRight = new Binding();

		RightSideSlideLeft = new Binding();
		RightSideSlideRight = new Binding();

		FileSystem::ConfigFile configFile(framework::GetModuleDirectory(), KEY_CONFIG_FILE_NAME);
		configFile.OpenRead();

		usePs4OfficialSlider = configFile.GetBooleanValue("ps4_official_slider");
		enableInMenus = configFile.GetBooleanValue("slider_in_menus");

		if (!usePs4OfficialSlider)
		{
			Config::BindConfigKeys(configFile.ConfigMap, "LEFT_SIDE_SLIDE_LEFT", *LeftSideSlideLeft, { "Q" });
			Config::BindConfigKeys(configFile.ConfigMap, "LEFT_SIDE_SLIDE_RIGHT", *LeftSideSlideRight, { "E" });

			Config::BindConfigKeys(configFile.ConfigMap, "RIGHT_SIDE_SLIDE_LEFT", *RightSideSlideLeft, { "U" });
			Config::BindConfigKeys(configFile.ConfigMap, "RIGHT_SIDE_SLIDE_RIGHT", *RightSideSlideRight, { "O" });

			float touchSliderEmulationSpeed = configFile.GetFloatValue("touch_slider_emulation_speed");

			if (touchSliderEmulationSpeed != 0.0f)
				sliderSpeed = touchSliderEmulationSpeed;
		}
	}

	void TouchSliderEmulator::Update()
	{
		sliderState->State = SLIDER_OK;
	}

	void TouchSliderEmulator::UpdateInput()
	{
		if (!componentsManager->GetUpdateGameInput() || componentsManager->IsDwGuiActive() || (!enableInMenus && !(*(GameState*)CURRENT_GAME_STATE_ADDRESS == GS_GAME && *(SubGameState*)CURRENT_GAME_SUB_STATE_ADDRESS == SUB_GAME_MAIN)))
			return;

		if (usePs4OfficialSlider)
		{
			DualShock4* ds4 = DualShock4::GetInstance();
			if (ds4 == nullptr)
				return;

			Joystick ls = ds4->GetLeftStick();
			Joystick rs = ds4->GetRightStick();

			// this looks bad because it is -- experimentally derived based on observed values
			/*
				0: -1
				63: -0.507805
				65: -0.492210
				126: -0.015640
				127: -0.007828
				128: -0.000015
				129: 0.000015
				130: 0.007950
				131: 0.015885
				192: 0.500023
				255: 1
			*/
			uint32_t state = 0;
			uint8_t read;
			//printf("left stick: %8.6f, %8.6f, ", ls.XAxis, ls.YAxis);
			read = ls.XAxis < 0 ? ((ls.XAxis + 1.001) * 128) : 255 - (uint8_t)((-ls.XAxis + 1.001) * 126);
			//printf("%d, ", (int)read);
			state |= read ^ 0b10000000;
			read = ls.YAxis < 0 ? ((ls.YAxis + 1.001) * 128) : 255 - (uint8_t)((-ls.YAxis + 1.001) * 126);
			//printf("%d\n", (int)read);
			state |= (read ^ 0b10000000) << 8;
			read = rs.XAxis < 0 ? ((rs.XAxis + 1.001) * 128) : 255 - (uint8_t)((-rs.XAxis + 1.001) * 126);
			state |= (read ^ 0b10000000) << 16;
			read = rs.YAxis < 0 ? ((rs.YAxis + 1.001) * 128) : 255 - (uint8_t)((-rs.YAxis + 1.001) * 126);
			state |= (read ^ 0b10000000) << 24;

			ApplyBitfieldState(state);
		}
		else
		{
			sliderIncrement = GetElapsedTime() / sliderSpeed;

			constexpr float sensorStep = (1.0f / SLIDER_SENSORS);

			EmulateSliderInput(LeftSideSlideLeft, LeftSideSlideRight, ContactPoints[0], 0.0f, 0.5f);
			EmulateSliderInput(RightSideSlideLeft, RightSideSlideRight, ContactPoints[1], 0.5f + sensorStep, 1.0f + sensorStep);

			sliderState->ResetSensors();

			for (int i = 0; i < CONTACT_POINTS; i++)
				ApplyContactPoint(ContactPoints[i]);
		}
	}

	void TouchSliderEmulator::OnFocusLost()
	{
		sliderState->ResetSensors();
	}

	void TouchSliderEmulator::EmulateSliderInput(Binding *leftBinding, Binding *rightBinding, ContactPoint &contactPoint, float start, float end)
	{
		bool leftDown = leftBinding->AnyDown();
		bool rightDown = rightBinding->AnyDown();

		if (leftDown)
			contactPoint.Position -= sliderIncrement;
		else if (rightDown)
			contactPoint.Position += sliderIncrement;

		if (contactPoint.Position < start)
			contactPoint.Position = end;

		if (contactPoint.Position > end)
			contactPoint.Position = start;

		bool leftTapped = leftBinding->AnyTapped();
		bool rightTapped = rightBinding->AnyTapped();

		if (leftTapped || rightTapped)
			contactPoint.Position = (start + end) / 2.0f;

		contactPoint.InContact = leftDown || rightDown;
	}

	void TouchSliderEmulator::ApplyContactPoint(ContactPoint& contactPoint)
	{
		if (contactPoint.InContact)
		{
			float position = std::clamp(contactPoint.Position, 0.0f, 1.0f);
			int sensor = (int)(position * (SLIDER_SENSORS - 1));

			sliderState->SetSensor(sensor, FULL_PRESSURE);
		}
	}

	void TouchSliderEmulator::ApplyBitfieldState(uint32_t state)
	{
		for (int i = 0; i < 32; i++)
		{
			if (state & (1 << (31 - i)))
				sliderState->SetSensor(i, FULL_PRESSURE);
			else
				sliderState->SetSensor(i, NO_PRESSURE);
		}
	}
}