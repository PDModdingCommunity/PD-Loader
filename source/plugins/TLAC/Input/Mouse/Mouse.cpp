#include "Mouse.h"
#include "../../framework.h"
#include "../../Constants.h"

namespace TLAC::Input
{
	Mouse* Mouse::instance;

	Mouse::Mouse()
	{
		directInputMouse = new DirectInputMouse();
	}

	Mouse::~Mouse()
	{
		if (directInputMouse != nullptr)
			delete directInputMouse;
	}

	Mouse* Mouse::GetInstance()
	{
		if (instance == nullptr)
			instance = new Mouse();

		return instance;
	}

	POINT Mouse::GetPosition()
	{
		return currentState.Position;
	}

	POINT Mouse::GetRelativePosition()
	{
		return currentState.RelativePosition;
	}

	POINT Mouse::GetDeltaPosition()
	{
		return 
		{ 
			currentState.Position.x - lastState.Position.x, 
			currentState.Position.y - lastState.Position.y 
		};
	}

	long Mouse::GetMouseWheel()
	{
		return currentState.MouseWheel;
	}

	long Mouse::GetDeltaMouseWheel()
	{
		return currentState.MouseWheel - lastState.MouseWheel;
	}

	bool Mouse::HasMoved()
	{
		POINT delta = GetDeltaPosition();
		return delta.x != 0 || delta.y != 0;
	}

	bool Mouse::GetIsScrolledUp()
	{
		return currentState.ScrolledUp;
	}

	bool Mouse::GetIsScrolledDown()
	{
		return currentState.ScrolledDown;
	}

	bool Mouse::GetWasScrolledUp()
	{
		return lastState.ScrolledUp;
	}

	bool Mouse::GetWasScrolledDown()
	{
		return lastState.ScrolledDown;
	}

	void Mouse::SetPosition(int x, int y)
	{
		lastState.Position.x = x;
		lastState.Position.y = y;
		SetCursorPos(x, y);
	}

	bool Mouse::PollInput()
	{
		lastState = currentState;

		GetCursorPos(&currentState.Position);
		currentState.RelativePosition = currentState.Position;

		if (framework::DivaWindowHandle != NULL)
			ScreenToClient(framework::DivaWindowHandle, &currentState.RelativePosition);

			RECT hWindow;
		GetClientRect(TLAC::framework::DivaWindowHandle, &hWindow);

		gameHeight = (int*)RESOLUTION_HEIGHT_ADDRESS;
		gameWidth = (int*)RESOLUTION_WIDTH_ADDRESS;
		fbWidth = (int*)FB_WIDTH_ADDRESS;
		fbHeight = (int*)FB_HEIGHT_ADDRESS;

		if (directInputMouse != nullptr)
		{
			if (directInputMouse->Poll())
				currentState.MouseWheel += directInputMouse->GetMouseWheel();
		
			currentState.ScrolledUp = (GetDeltaMouseWheel() > 0);
			currentState.ScrolledDown = (GetDeltaMouseWheel() < 0);
		}

		if ((fbWidth != gameWidth) && (fbHeight != gameHeight)) {
			xoffset = ((float)16 / (float)9) * (hWindow.bottom - hWindow.top);
			if (xoffset != (hWindow.right - hWindow.left))
			{
				scale = xoffset / (hWindow.right - hWindow.left);
				xoffset = ((hWindow.right - hWindow.left) / 2) - (xoffset / 2);
			}
			else {
				xoffset = 0;
				scale = 1;
			}
			
			currentState.RelativePosition.x = ((currentState.RelativePosition.x - round(xoffset)) * *gameWidth / (hWindow.right - hWindow.left)) / scale;
			currentState.RelativePosition.y = currentState.RelativePosition.y * *gameHeight / (hWindow.bottom - hWindow.top);
		}

		return true;
	}
}
