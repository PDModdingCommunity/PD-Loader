#include "CameraController.h"
#include <algorithm>
#include "Input/InputState.h"
#include "ComponentsManager.h"
#include "../framework.h"
#include "../Input/Mouse/Mouse.h"
#include "../Input/Keyboard/Keyboard.h"
#include "../Input/Bindings/KeyboardBinding.h"

#define  GLUT_CURSOR_RIGHT_ARROW	0x0000
#define  GLUT_CURSOR_NONE			0x0065

using namespace TLAC::Input;
using namespace TLAC::Utilities;

namespace TLAC::Components
{
	CameraController::CameraController()
	{
	}

	CameraController::~CameraController()
	{
		delete ToggleBinding;

		delete ForwardBinding;
		delete BackwardBinding;
		delete LeftBinding;
		delete RightBinding;

		delete UpBinding;
		delete DownBinding;
		delete ClockwiseBinding;
		delete CounterClockwiseBinding;
		delete ZoomInBinding;
		delete ZoomOutBinding;

		delete FastBinding;
		delete SlowBinding;
	}

	const char* CameraController::GetDisplayName()
	{
		return "camera_controller";
	}

	void CameraController::Initialize(ComponentsManager* manager)
	{
		componentsManager = manager;

		printf("[TLAC] CameraController::Initialize(): Initialized\n");

		for (int i = 0; i < sizeof(cameraSetterAddresses) / sizeof(void*); i++)
		{
			DWORD oldProtect;
			VirtualProtect((void*)cameraSetterAddresses[i], sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtect);

			originalSetterBytes[i] = *(uint8_t*)cameraSetterAddresses[i];
		}

		ToggleBinding = new Binding();
		ToggleBinding->AddBinding(new KeyboardBinding(VK_F3));

		ForwardBinding = new Binding();
		ForwardBinding->AddBinding(new KeyboardBinding('W'));
		BackwardBinding = new Binding();
		BackwardBinding->AddBinding(new KeyboardBinding('S'));
		LeftBinding = new Binding();
		LeftBinding->AddBinding(new KeyboardBinding('A'));
		RightBinding = new Binding();
		RightBinding->AddBinding(new KeyboardBinding('D'));

		UpBinding = new Binding();
		UpBinding->AddBinding(new KeyboardBinding(VK_SPACE));
		DownBinding = new Binding();
		DownBinding->AddBinding(new KeyboardBinding(VK_CONTROL));

		ClockwiseBinding = new Binding();
		ClockwiseBinding->AddBinding(new KeyboardBinding('E'));
		CounterClockwiseBinding = new Binding();
		CounterClockwiseBinding->AddBinding(new KeyboardBinding('Q'));

		ZoomInBinding = new Binding();
		ZoomInBinding->AddBinding(new KeyboardBinding('R'));
		ZoomOutBinding = new Binding();
		ZoomOutBinding->AddBinding(new KeyboardBinding('F'));

		FastBinding = new Binding();
		FastBinding->AddBinding(new KeyboardBinding(VK_SHIFT));
		SlowBinding = new Binding();
		SlowBinding->AddBinding(new KeyboardBinding(VK_MENU));

		camera = (Camera*)CAMERA_ADDRESS;
	}

	void CameraController::Update()
	{
		return;
	}

	void CameraController::UpdateInput()
	{
		if (ToggleBinding->AnyTapped())
		{
			SetControls(!GetIsEnabled());
			return;
		}

		if (!GetIsEnabled())
			return;

		auto keyboard = Keyboard::GetInstance();
		auto mouse = Mouse::GetInstance();

		bool forward = ForwardBinding->AnyDown();
		bool backward = BackwardBinding->AnyDown();
		bool left = LeftBinding->AnyDown();
		bool right = RightBinding->AnyDown();

		bool up = UpBinding->AnyDown();
		bool down = DownBinding->AnyDown();

		bool fast = FastBinding->AnyDown();
		bool slow = SlowBinding->AnyDown();

		bool clockwise = ClockwiseBinding->AnyDown();
		bool counterclockwise = CounterClockwiseBinding->AnyDown();

		bool zoomin = ZoomInBinding->AnyDown();
		bool zoomout = ZoomOutBinding->AnyDown();

		float speed = GetElapsedTime() * (fast ? fastSpeed : slow ? slowSpeed : normalSpeed);

		if (forward ^ backward)
			camera->Position += PointFromAngle(verticalRotation + (forward ? +0.0f : -180.0f), speed);

		if (left ^ right)
			camera->Position += PointFromAngle(verticalRotation + (right ? +90.0f : -90.0f), speed);

		if (up ^ down)
			camera->Position.Y += speed * (up ? +0.25f : -0.25f);

		if (clockwise ^ counterclockwise)
			camera->Rotation += speed * (clockwise ? -1.0f : +1.0f);

		if (zoomin ^ zoomout)
		{
			camera->HorizontalFov += speed * (zoomin ? -1.0f : +1.0f);
			camera->HorizontalFov = std::clamp(camera->HorizontalFov, +1.0f, +170.0f);
		}

		if (mouse->HasMoved())
		{
			SetMouseWindowCenter();

			auto delta = mouse->GetDeltaPosition();

			verticalRotation += delta.x * sensitivity;
			horizontalRotation -= delta.y * (sensitivity / 5.0f);

			horizontalRotation = std::clamp(horizontalRotation, -75.0f, +75.0f);
		}

		((InputState*)*(uint64_t*)INPUT_STATE_PTR_ADDRESS)->HideCursor();

		Vec2 focus = PointFromAngle(verticalRotation, 1.0f);
		camera->Focus.X = camera->Position.X + focus.X;
		camera->Focus.Z = camera->Position.Z + focus.Y;

		camera->Focus.Y = camera->Position.Y + PointFromAngle(horizontalRotation, 5.0f).X;
	}

	void CameraController::SetControls(bool value)
	{
		if (GetIsEnabled() == value)
			return;

		SetIsEnabled(value);
		componentsManager->SetUpdateGameInput(!value);

		printf("[TLAC] CameraController::SetControls(): enabled = %s\n", GetIsEnabled() ? "true" : "false");

		typedef void __stdcall _glutSetCursor(int);
		auto glutSetCursor = (_glutSetCursor*)GLUT_SET_CURSOR_ADDRESS;

		// hide cursor
		glutSetCursor(value ? GLUT_CURSOR_NONE : GLUT_CURSOR_RIGHT_ARROW);

		if (value)
		{
			// disable camera setters
			for (int i = 0; i < sizeof(cameraSetterAddresses) / sizeof(void*); i++)
				*(uint8_t*)cameraSetterAddresses[i] = RET_OPCODE;

			// set initial camera angle
			Vec2 camXz = Vec2(camera->Position.X, camera->Position.Z);
			Vec2 focusXz = Vec2(camera->Focus.X, camera->Focus.Z);
			verticalRotation = AngleFromPoints(camXz, focusXz);

			horizontalRotation = 0;
			camera->Rotation = defaultRotation;
			camera->HorizontalFov = defaultFov;
		}
		else
		{
			// restore camera setters
			for (int i = 0; i < sizeof(cameraSetterAddresses) / sizeof(void*); i++)
				*(uint8_t*)cameraSetterAddresses[i] = originalSetterBytes[i];
		}
	}

	void CameraController::SetMouseWindowCenter()
	{
		RECT windowRect = framework::GetWindowBounds();

		int centerX = windowRect.left + (windowRect.right - windowRect.left) / 2;
		int centerY = windowRect.top + (windowRect.bottom - windowRect.top) / 2;

		Mouse::GetInstance()->SetPosition(centerX, centerY);
	}

	bool CameraController::GetIsEnabled()
	{
		return isEnabled;
	}

	void CameraController::SetIsEnabled(bool value)
	{
		isEnabled = value;
	}
}