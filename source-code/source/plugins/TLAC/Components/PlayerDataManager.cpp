#include "PlayerDataManager.h"
#include <string>
#include "../framework.h"
#include "../Constants.h"
#include "../Input/Keyboard/Keyboard.h"
#include "../FileSystem/ConfigFile.h"
#include "../Constants.h"

const std::string PLAYER_DATA_FILE_NAME = "playerdata.ini";

namespace TLAC::Components
{
	PlayerDataManager::PlayerDataManager()
	{
	}

	PlayerDataManager::~PlayerDataManager()
	{
		if (customPlayerData != nullptr)
			delete customPlayerData;
	}

	const char* PlayerDataManager::GetDisplayName()
	{
		return "player_data_manager";
	}

	void PlayerDataManager::Initialize(ComponentsManager*)
	{
		playerData = (PlayerData*)PLAYER_DATA_ADDRESS;

		ApplyPatch();
		LoadConfig();
		ApplyCustomData();
	}

	void PlayerDataManager::Update()
	{
		ApplyCustomData();

		if (false && Input::Keyboard::GetInstance()->IsTapped(VK_F12))
		{
			printf("[TLAC] PlayerDataManager::Update(): Loading config...\n");
			LoadConfig();
		}
		if (moduleCardWorkaround) {
			int* pvId = (int*)0x00000001418054C4;
			int* modState = (int*)0x00000001411A9790;
			int* loadState = (int*)0x0000000141197E58;

			if (!customPlayerData->UseCard)
			{
				if (*(char*)0x000000014CC5E270 == 32)
				{
					DWORD oldProtect, bck;
					VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405BC8E6 + 0) = 0x00;
					VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);
					pvModuleLoaded = false;
				}
				else {
					if (!pvModuleLoaded)
					{
						pvModuleLoaded = true;
						DWORD oldProtect, bck;
						VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
						*((BYTE*)0x00000001405BC8E6 + 0) = 0x01;
						VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);
					}
				}

				if ((initPvId == false) || ((lastModState == 0) && (*modState == 1)))
				{
					if ((lastModState == 0) && (*modState == 1))
					{
						*(int*)0x00000001411A8A10 = *(int*)0x00000001411A8A28;
						*(int*)(0x00000001411A8A10 + 4) = *(int*)(0x00000001411A8A28 + 4);
						*(int*)(0x00000001411A8A10 + 8) = *(int*)(0x00000001411A8A28 + 8);
						*(int*)(0x00000001411A8A10 + 12) = *(int*)(0x00000001411A8A28 + 12);
						*(int*)(0x00000001411A8A10 + 16) = *(int*)(0x00000001411A8A28 + 16);
						*(int*)(0x00000001411A8A10 + 18) = *(int*)(0x00000001411A8A28 + 18);
					}

					initPvId = true;
					lastModState = *modState;
					DWORD oldProtect, bck;
					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405CBBA3 + 0) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 1) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 2) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 3) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 4) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 5) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 6) = 0x90;
					*((BYTE*)0x00000001405CBBA3 + 7) = 0x90;
					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, oldProtect, &bck);
				}

				if (((*modState == 0) && (*pvId != lastPvId)) || ((lastModState == 1) && (*modState == 0)))
				{
					if ((lastModState == 1) && (*modState == 0))
					{
						*(int*)0x00000001411A8A28 = *(int*)0x00000001411A8A10;
						*(int*)(0x00000001411A8A28 + 4) = *(int*)(0x00000001411A8A10 + 4);
						*(int*)(0x00000001411A8A28 + 8) = *(int*)(0x00000001411A8A10 + 8);
						*(int*)(0x00000001411A8A28 + 12) = *(int*)(0x00000001411A8A10 + 12);
						*(int*)(0x00000001411A8A28 + 16) = *(int*)(0x00000001411A8A10 + 16);
						*(int*)(0x00000001411A8A28 + 18) = *(int*)(0x00000001411A8A10 + 18);
					}

					initPvId = false;
					lastPvId = *pvId;
					lastModState = *modState;
					DWORD oldProtect, bck;
					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405CBBA3 + 0) = 0x42;
					*((BYTE*)0x00000001405CBBA3 + 1) = 0x89;
					*((BYTE*)0x00000001405CBBA3 + 2) = 0x84;
					*((BYTE*)0x00000001405CBBA3 + 3) = 0xb6;
					*((BYTE*)0x00000001405CBBA3 + 4) = 0xc0;
					*((BYTE*)0x00000001405CBBA3 + 5) = 0x01;
					*((BYTE*)0x00000001405CBBA3 + 6) = 0x00;
					*((BYTE*)0x00000001405CBBA3 + 7) = 0x00;
					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, oldProtect, &bck);
				}
			}
			else {
				if (*(char*)0x000000014CC5E270 == 32)
				{
					DWORD oldProtect, bck;
					VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405BC8E6 + 0) = 0x01;
					VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);

					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405CBBA3 + 0) = 0x42;
					*((BYTE*)0x00000001405CBBA3 + 1) = 0x89;
					*((BYTE*)0x00000001405CBBA3 + 2) = 0x84;
					*((BYTE*)0x00000001405CBBA3 + 3) = 0xb6;
					*((BYTE*)0x00000001405CBBA3 + 4) = 0xc0;
					*((BYTE*)0x00000001405CBBA3 + 5) = 0x01;
					*((BYTE*)0x00000001405CBBA3 + 6) = 0x00;
					*((BYTE*)0x00000001405CBBA3 + 7) = 0x00;
					VirtualProtect((BYTE*)0x00000001405CBBA3, 8, oldProtect, &bck);

					VirtualProtect((BYTE*)0x00000001405BCBE3, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
					*((BYTE*)0x00000001405BCBE3 + 0) = 0x00;
					VirtualProtect((BYTE*)0x00000001405BCBE3, 2, oldProtect, &bck);

					pvModuleLoaded = false;
				}
				else {
					if (!pvModuleLoaded)
					{
						pvModuleLoaded = true;
						DWORD oldProtect, bck;
						VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
						*((BYTE*)0x00000001405BC8E6 + 0) = 0x00;
						VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);

						VirtualProtect((BYTE*)0x00000001405CBBA3, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
						*((BYTE*)0x00000001405CBBA3 + 0) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 1) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 2) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 3) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 4) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 5) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 6) = 0x90;
						*((BYTE*)0x00000001405CBBA3 + 7) = 0x90;
						VirtualProtect((BYTE*)0x00000001405CBBA3, 8, oldProtect, &bck);

						VirtualProtect((BYTE*)0x00000001405BCBE3, 2, PAGE_EXECUTE_READWRITE, &oldProtect);
						*((BYTE*)0x00000001405BCBE3 + 0) = 0x01;
						VirtualProtect((BYTE*)0x00000001405BCBE3, 2, oldProtect, &bck);
					}
				}
			}
		}
	}

	void PlayerDataManager::ApplyPatch()
	{
		DWORD oldProtect;
		VirtualProtect((void*)SET_DEFAULT_PLAYER_DATA_ADDRESS, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtect);
		{
			// prevent the PlayerData from being reset
			*(uint8_t*)(SET_DEFAULT_PLAYER_DATA_ADDRESS) = RET_OPCODE;
		}
		VirtualProtect((void*)SET_DEFAULT_PLAYER_DATA_ADDRESS, sizeof(uint8_t), oldProtect, &oldProtect);

		// allow player to select the module and extra item
		VirtualProtect((void*)MODSELECTOR_CHECK_FUNCTION_ERRRET_ADDRESS, sizeof(byte) * 2, PAGE_EXECUTE_READWRITE, &oldProtect);
		{
			*(byte*)(MODSELECTOR_CHECK_FUNCTION_ERRRET_ADDRESS) = 0xB0; // xor al,al -> ld al,1
			*(byte*)(MODSELECTOR_CHECK_FUNCTION_ERRRET_ADDRESS + 0x1) = 0x01;
		}
		VirtualProtect((void*)MODSELECTOR_CHECK_FUNCTION_ERRRET_ADDRESS, sizeof(byte) * 2, oldProtect, &oldProtect);

		// fix annoying behavior of closing after changing module or item (don't yet know the reason, maybe NW/Card checks)
		{
			VirtualProtect((void*)MODSELECTOR_CLOSE_AFTER_MODULE, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtect);
			{
				*(uint8_t*)(MODSELECTOR_CLOSE_AFTER_MODULE) = JNE_OPCODE;
			}
			VirtualProtect((void*)MODSELECTOR_CLOSE_AFTER_MODULE, sizeof(uint8_t), oldProtect, &oldProtect);
			VirtualProtect((void*)MODSELECTOR_CLOSE_AFTER_CUSTOMIZE, sizeof(uint8_t), PAGE_EXECUTE_READWRITE, &oldProtect);
			{
				*(uint8_t*)(MODSELECTOR_CLOSE_AFTER_CUSTOMIZE) = JNE_OPCODE;
			}
			VirtualProtect((void*)MODSELECTOR_CLOSE_AFTER_CUSTOMIZE, sizeof(uint8_t), oldProtect, &oldProtect);
		}
	}

	void PlayerDataManager::LoadConfig()
	{
		if (playerData == nullptr)
			return;

		FileSystem::ConfigFile config(framework::GetModuleDirectory(), PLAYER_DATA_FILE_NAME);

		if (!config.OpenRead())
			return;

		if (customPlayerData != nullptr)
			delete customPlayerData;

		customPlayerData = new CustomPlayerData();
		config.TryGetValue("player_name", &customPlayerData->PlayerName);
		config.TryGetValue("level_name", &customPlayerData->LevelName);

		customPlayerData->LevelPlateId = config.GetIntegerValue("level_plate_id");
		customPlayerData->SkinEquip = config.GetIntegerValue("skin_equip");
		customPlayerData->BtnSeEquip = config.GetIntegerValue("btn_se_equip");
		customPlayerData->SlideSeEquip = config.GetIntegerValue("slide_se_equip");
		customPlayerData->ChainslideSeEquip = config.GetIntegerValue("chainslide_se_equip");
		customPlayerData->ShowExcellentClearBorder = config.GetBooleanValue("border_excellent");
		customPlayerData->ShowGreatClearBorder = config.GetBooleanValue("border_great");
		customPlayerData->UseCard = config.GetBooleanValue("use_card");
		customPlayerData->GameModifierOptions = config.GetBooleanValue("gamemode_options");
		customPlayerData->ActionSE = config.GetBooleanValue("act_toggle");
		moduleCardWorkaround = config.GetBooleanValue("module_card_workaround");

		if (moduleCardWorkaround) {
			if (!customPlayerData->UseCard)
			{
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x000000014010523F, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x000000014010523F + 0) = 0x30;
				*((BYTE*)0x000000014010523F + 1) = 0xC0;
				*((BYTE*)0x000000014010523F + 2) = 0x90;
				VirtualProtect((BYTE*)0x000000014010523F, 3, oldProtect, &bck);

				VirtualProtect((BYTE*)0x00000001405BCC48, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x00000001405BCC48 + 0) = 0x90;
				*((BYTE*)0x00000001405BCC48 + 1) = 0x90;
				*((BYTE*)0x00000001405BCC48 + 2) = 0x90;
				*((BYTE*)0x00000001405BCC48 + 3) = 0x90;
				*((BYTE*)0x00000001405BCC48 + 4) = 0x90;
				*((BYTE*)0x00000001405BCC48 + 5) = 0x90;
				VirtualProtect((BYTE*)0x00000001405BCC48, 6, oldProtect, &bck);

				VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x00000001405BC8E6 + 0) = 0x01;
				VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);

			}
			else {
				DWORD oldProtect, bck;
				VirtualProtect((BYTE*)0x00000001405BC8E6, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x00000001405BC8E6 + 0) = 0x00;
				VirtualProtect((BYTE*)0x00000001405BC8E6, 3, oldProtect, &bck);

				VirtualProtect((BYTE*)0x00000001405BCC48, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x00000001405BCC48 + 0) = 0xC7;
				*((BYTE*)0x00000001405BCC48 + 1) = 0x03;
				*((BYTE*)0x00000001405BCC48 + 2) = 0x00;
				*((BYTE*)0x00000001405BCC48 + 3) = 0x00;
				*((BYTE*)0x00000001405BCC48 + 4) = 0x00;
				*((BYTE*)0x00000001405BCC48 + 5) = 0x00;
				VirtualProtect((BYTE*)0x00000001405BCC48, 6, oldProtect, &bck);

				VirtualProtect((BYTE*)0x000000014010523F, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x000000014010523F + 0) = 0x0F;
				*((BYTE*)0x000000014010523F + 1) = 0x94;
				*((BYTE*)0x000000014010523F + 2) = 0xC1;
				VirtualProtect((BYTE*)0x000000014010523F, 3, oldProtect, &bck);

				//just incase the player is reloading
				VirtualProtect((BYTE*)0x00000001405CBBA3, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
				*((BYTE*)0x00000001405CBBA3 + 0) = 0x42;
				*((BYTE*)0x00000001405CBBA3 + 1) = 0x89;
				*((BYTE*)0x00000001405CBBA3 + 2) = 0x84;
				*((BYTE*)0x00000001405CBBA3 + 3) = 0xb6;
				*((BYTE*)0x00000001405CBBA3 + 4) = 0xc0;
				*((BYTE*)0x00000001405CBBA3 + 5) = 0x01;
				*((BYTE*)0x00000001405CBBA3 + 6) = 0x00;
				*((BYTE*)0x00000001405CBBA3 + 7) = 0x00;
				VirtualProtect((BYTE*)0x00000001405CBBA3, 8, oldProtect, &bck);
			}
		}
	}

	void PlayerDataManager::ApplyCustomData()
	{
		// don't want to overwrite the default values
		auto setIfNotEqual = [](int *target, int value, int comparison)
		{
			if (value != comparison)
				*target = value;
		};

		setIfNotEqual(&playerData->level_plate_id, customPlayerData->LevelPlateId, 0);
		setIfNotEqual(&playerData->skin_equip, customPlayerData->SkinEquip, 0);
		setIfNotEqual(&playerData->btn_se_equip, customPlayerData->BtnSeEquip, -1);
		setIfNotEqual(&playerData->slide_se_equip, customPlayerData->SlideSeEquip, -1);
		setIfNotEqual(&playerData->chainslide_se_equip, customPlayerData->ChainslideSeEquip, -1);
		setIfNotEqual(&playerData->act_toggle, customPlayerData->ActionSE, 1);

		// Display clear borders on the progress bar
		*(byte*)(PLAYER_DATA_ADDRESS + 0xD94) = (customPlayerData->ShowExcellentClearBorder << 1) | (customPlayerData->ShowGreatClearBorder);
		
		playerData->use_card = customPlayerData->UseCard; // required to allow for module selection

		playerData->game_opts = customPlayerData->GameModifierOptions; // hi-speed, etc..

		memset((void*)MODULE_TABLE_START, 0xFF, 128);
		memset((void*)ITEM_TABLE_START, 0xFF, 128);

		if (customPlayerData->PlayerName != nullptr)
		{
			playerData->field_DC = 0x10;
			playerData->player_name = (char*)customPlayerData->PlayerName->c_str();
		}


		if (customPlayerData->LevelName != nullptr) 
		{
			playerData->level_name = (char*)customPlayerData->LevelName->c_str();
			playerData->field_110 = 0xFF;
			playerData->field_118 = 0x1F;
		}
	}
}
