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

		// Display clear borders on the progress bar
		*(byte*)(PLAYER_DATA_ADDRESS + 0xD94) = (customPlayerData->ShowExcellentClearBorder << 1) | (customPlayerData->ShowGreatClearBorder);
		
		if (customPlayerData->UseCard)
			playerData->use_card = 1; // required to allow for module selection

		if (customPlayerData->GameModifierOptions)
			playerData->game_opts = 1; // hi-speed, etc..

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
