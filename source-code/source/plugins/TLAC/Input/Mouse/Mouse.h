#pragma once
#include "../IInputDevice.h"
#include "../DirectInput/DirectInputMouse.h"
#include "MouseState.h"

namespace TLAC::Input
{
	class Mouse : public IInputDevice
	{
	public:
		~Mouse();
		
		static Mouse* GetInstance();

		bool PollInput() override;
		
		POINT GetPosition();
		POINT GetRelativePosition();
		POINT GetDeltaPosition();
		
		long GetMouseWheel();
		long GetDeltaMouseWheel();
		
		bool HasMoved();
		bool GetIsScrolledUp();
		bool GetIsScrolledDown();
		bool GetWasScrolledUp();
		bool GetWasScrolledDown();

		void SetPosition(int x, int y);

	private:
		Mouse();
		MouseState lastState;
		MouseState currentState;
		DirectInputMouse* directInputMouse = nullptr;

		int* gameWidth;
		int* gameHeight;
		int* fbWidth;
		int* fbHeight;

		float xoffset;
		float scale;

		static Mouse* instance;
	};
}

