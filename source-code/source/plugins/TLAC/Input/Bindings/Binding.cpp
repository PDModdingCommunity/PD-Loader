#include "Binding.h"

namespace TLAC::Input
{
	Binding::Binding()
	{
	}

	Binding::~Binding()
	{
		for (auto& binding : InputBindings)
			delete binding;
	}

	void Binding::AddBinding(IInputBinding* inputBinding)
	{
		InputBindings.push_back(inputBinding);
	}

	bool Binding::AnyDown()
	{
		int i = 0;
		for (const auto& binding : InputBindings)
		{
			// If an old input is still held down, don't register it, only a new input is allowed.
			if (binding->IsDown() && (this->lastDownId == i || binding->BypassTransferCheck()))
				return true;

			i++;
		}

		return false;
	}

	bool Binding::AnyTapped()
	{
		int i = 0;
		for (const auto& binding : InputBindings)
		{
			if (binding->IsTapped()) {
				// This saves the last button id to a variable, so that it's known as the last input registered (see AnyDown()).
				this->lastDownId = i;
				return true;
			}

			i++;
		}

		return false;
	}

	bool Binding::AnyReleased()
	{
		for (const auto& binding : InputBindings)
		{
			if (binding->IsReleased())
				return true;
		}

		return false;
	}

	int Binding::GetDownCount()
	{
		int count = 0;

		for (const auto& binding : InputBindings)
		{
			if (binding->IsDown())
				count++;
		}

		return count;
	}

	int Binding::GetTappedCount()
	{
		int count = 0;

		for (const auto& binding : InputBindings)
		{
			if (binding->IsTapped())
				count++;
		}

		return count;
	}

	int Binding::GetReleasedCount()
	{
		int count = 0;

		for (const auto& binding : InputBindings)
		{
			if (binding->IsReleased())
				count++;
		}

		return count;
	}
}
