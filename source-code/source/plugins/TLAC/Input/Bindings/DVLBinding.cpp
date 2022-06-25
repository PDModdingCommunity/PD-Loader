#include "DVLBinding.h"

namespace TLAC::Input
{
	#define DivallerInstanceCheckDefault(checkFunc) (Divaller::InstanceInitialized() ? Divaller::GetInstance()->checkFunc : false)

	DivallerBinding::DivallerBinding(DivallerButton button) : Button(button)
	{
	}

	DivallerBinding::~DivallerBinding()
	{
	}

	bool DivallerBinding::IsDown()
	{
		return DivallerInstanceCheckDefault(IsDown(Button));
	}

	bool DivallerBinding::IsTapped()
	{
		return DivallerInstanceCheckDefault(IsTapped(Button));
	}

	bool DivallerBinding::IsReleased()
	{
		return DivallerInstanceCheckDefault(IsReleased(Button));
	}
}