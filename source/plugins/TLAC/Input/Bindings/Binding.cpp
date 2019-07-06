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
		for (const auto& binding : InputBindings)
		{
			if (binding->IsDown())
				return true;
		}

		return false;
	}

	bool Binding::AnyTapped()
	{
		for (const auto& binding : InputBindings)
		{
			if (binding->IsTapped())
				return true;
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
