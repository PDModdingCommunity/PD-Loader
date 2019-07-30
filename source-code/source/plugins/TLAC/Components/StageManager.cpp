#include "StageManager.h"
#include "../Constants.h"
#include <windows.h>

namespace TLAC::Components
{
	StageManager::StageManager()
	{
	}

	StageManager::~StageManager()
	{
	}

	const char* StageManager::GetDisplayName()
	{
		return "stage_manager";
	}

	void StageManager::Initialize(ComponentsManager*)
	{
		// add the offset between moving 2 into ebx and the start of the function
		int32_t* playCount = (int32_t*)(((uint8_t*)PLAYS_PER_SESSION_GETTER_ADDRESS) + 0x7);

		DWORD oldProtect;
		VirtualProtect(playCount, sizeof(int32_t), PAGE_EXECUTE_READWRITE, &oldProtect);

		// set í èÌÉÇÅ[Éh per session play count
		*playCount = GetPlayCount();
	}

	void StageManager::Update()
	{
		return;
	}

	int32_t StageManager::GetPlayCount()
	{
		return INT32_MAX;
	}
}