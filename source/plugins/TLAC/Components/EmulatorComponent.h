#pragma once

namespace TLAC::Components
{
	class ComponentsManager;

	class EmulatorComponent
	{
	public:
		EmulatorComponent();
		~EmulatorComponent();

		virtual const char* GetDisplayName() = 0;
		
		virtual void Initialize(ComponentsManager*) = 0;
		virtual void Update() = 0;

		virtual void UpdateInput() {};
		virtual void OnFocusGain() {};
		virtual void OnFocusLost() {};
		
		void SetElapsedTime(float value);
		float GetElapsedTime();
		float GetFrameRate();
		float GetGameFrameRate();

	private:
		float elapsedTime;
	};
}
