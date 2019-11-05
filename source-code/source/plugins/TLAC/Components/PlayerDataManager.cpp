#include "PlayerDataManager.h"
#include <string>
#include "../framework.h"
#include "../Constants.h"
#include "../Input/Keyboard/Keyboard.h"
#include "../FileSystem/ConfigFile.h"
#include "../Utilities/Operations.h"
#include <windows.h>
#include "GameState.h"

const std::string PLAYER_DATA_FILE_NAME = "playerdata.ini";
static WCHAR configPath[256];
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

		/*if (false && Input::Keyboard::GetInstance()->IsTapped(VK_F12))
		{
			printf("[TLAC] PlayerDataManager::Update(): Loading config...\n");
			LoadConfig();
		}*/

		// nas, I moved the patches back to patches because please..  at least don't apply it every frame when it's not necessary lol
		// and reverting was the easiest way to undo that

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
					*((BYTE*)0x00000001405BC8E6 + 0) = 0x00;
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

		// ScoreSaver copies (can be improved?)
		std::string utf8path = TLAC::framework::GetModuleDirectory() + "/playerdata.ini";
		MultiByteToWideChar(CP_UTF8, 0, utf8path.c_str(), -1, configPath, 256);

		customPlayerData->LevelNum = config.GetIntegerValue("level");
		customPlayerData->VocaloidPoints = config.GetIntegerValue("vocaloid_point");
		customPlayerData->LevelPlateId = config.GetIntegerValue("level_plate_id");
		customPlayerData->LevelPlateEffect = config.GetIntegerValue("level_plate_effect");
		customPlayerData->SkinEquip = config.GetIntegerValue("skin_equip");
		customPlayerData->BtnSeEquip = config.GetIntegerValue("btn_se_equip");
		customPlayerData->SlideSeEquip = config.GetIntegerValue("slide_se_equip");
		customPlayerData->ChainslideSeEquip = config.GetIntegerValue("chainslide_se_equip");
		customPlayerData->SlidertouchSeEquip = config.GetIntegerValue("slidertouch_se_equip");
		//customPlayerData->ModuleEquip0 = config.GetIntegerValue("module_equip0"); See below for explanation
		//customPlayerData->ModuleEquip1 = config.GetIntegerValue("module_equip1");
		//customPlayerData->ModuleEquip2 = config.GetIntegerValue("module_equip2");
		//customPlayerData->ModuleEquip3 = config.GetIntegerValue("module_equip3");
		//customPlayerData->ModuleEquip4 = config.GetIntegerValue("module_equip4");
		//customPlayerData->ModuleEquip5 = config.GetIntegerValue("module_equip5");
		customPlayerData->ModuleEquipCmn0 = config.GetIntegerValue("module_equip_cmn0");
		customPlayerData->ModuleEquipCmn1 = config.GetIntegerValue("module_equip_cmn1");
		customPlayerData->ModuleEquipCmn2 = config.GetIntegerValue("module_equip_cmn2");
		customPlayerData->ModuleEquipCmn3 = config.GetIntegerValue("module_equip_cmn3");
		customPlayerData->ModuleEquipCmn4 = config.GetIntegerValue("module_equip_cmn4");
		customPlayerData->ModuleEquipCmn5 = config.GetIntegerValue("module_equip_cmn5");
		customPlayerData->ShowExcellentClearBorder = config.GetBooleanValue("border_excellent");
		customPlayerData->ShowGreatClearBorder = config.GetBooleanValue("border_great");
		customPlayerData->ShowRivalClearBorder = config.GetBooleanValue("border_rival");
		customPlayerData->UseCard = config.GetBooleanValue("use_card");
		customPlayerData->GameModifierOptions = config.GetBooleanValue("gamemode_options");
		customPlayerData->ActionSE = config.GetBooleanValue("act_toggle");
		moduleCardWorkaround = config.GetBooleanValue("module_card_workaround");

		// don't want to overwrite the default values
		auto setIfNotEqual = [](int* target, int value, int comparison)
		{
			if (value != comparison)
				* target = value;
		};

		// only need to run this line once on startup, otherwise cannot increase/decrease in-game
		setIfNotEqual(&playerData->vocaloid_point, customPlayerData->VocaloidPoints, 0);
		// run once to check if module setting is invalid
		setIfNotEqual(&playerData->module_equip_cmn0, customPlayerData->ModuleEquipCmn0, 0);
		setIfNotEqual(&playerData->module_equip_cmn1, customPlayerData->ModuleEquipCmn1, 0);
		setIfNotEqual(&playerData->module_equip_cmn2, customPlayerData->ModuleEquipCmn2, 0);
		setIfNotEqual(&playerData->module_equip_cmn3, customPlayerData->ModuleEquipCmn3, 0);
		setIfNotEqual(&playerData->module_equip_cmn4, customPlayerData->ModuleEquipCmn4, 0);
		setIfNotEqual(&playerData->module_equip_cmn5, customPlayerData->ModuleEquipCmn5, 0);

		std::string* mylistString;
		std::vector<std::string> mylistStringVec;

		customPlayerData->Mylist[0].clear();
		config.TryGetValue("mylist_A", &mylistString);
		if (mylistString != nullptr)
		{
			for (std::string &pvStr : TLAC::Utilities::Split(*mylistString, ","))
				customPlayerData->Mylist[0].push_back(atoi(pvStr.c_str()));
			delete mylistString;
		}

		customPlayerData->Mylist[1].clear();
		config.TryGetValue("mylist_B", &mylistString);
		if (mylistString != nullptr)
		{
			for (std::string &pvStr : TLAC::Utilities::Split(*mylistString, ","))
				customPlayerData->Mylist[1].push_back(atoi(pvStr.c_str()));
			delete mylistString;
		}

		customPlayerData->Mylist[2].clear();
		config.TryGetValue("mylist_C", &mylistString);
		if (mylistString != nullptr)
		{
			for (std::string &pvStr : TLAC::Utilities::Split(*mylistString, ","))
				customPlayerData->Mylist[2].push_back(atoi(pvStr.c_str()));
			delete mylistString;
		}

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
		// check if the playerData in-game value is different from the customPlayerData value and update and save the ini
		if (customPlayerData->VocaloidPoints != playerData->vocaloid_point)
		{
			customPlayerData->VocaloidPoints = playerData->vocaloid_point;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->vocaloid_point);
			WritePrivateProfileStringW(L"playerdata", L"vocaloid_point", val, configPath);
		}
		// save module choices to module_equip_cmn[0-5]
		if (customPlayerData->ModuleEquipCmn0 != playerData->module_equip_cmn0)
		{
			customPlayerData->ModuleEquipCmn0 = playerData->module_equip_cmn0;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn0);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn0", val, configPath);
		}
		if (customPlayerData->ModuleEquipCmn1 != playerData->module_equip_cmn1)
		{
			customPlayerData->ModuleEquipCmn1 = playerData->module_equip_cmn1;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn1);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn1", val, configPath);
		}
		if (customPlayerData->ModuleEquipCmn2 != playerData->module_equip_cmn2)
		{
			customPlayerData->ModuleEquipCmn2 = playerData->module_equip_cmn2;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn2);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn2", val, configPath);
		}
		if (customPlayerData->ModuleEquipCmn3 != playerData->module_equip_cmn3)
		{
			customPlayerData->ModuleEquipCmn3 = playerData->module_equip_cmn3;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn3);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn3", val, configPath);
		}
		if (customPlayerData->ModuleEquipCmn4 != playerData->module_equip_cmn4)
		{
			customPlayerData->ModuleEquipCmn4 = playerData->module_equip_cmn4;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn4);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn4", val, configPath);
		}
		if (customPlayerData->ModuleEquipCmn5 != playerData->module_equip_cmn5)
		{
			customPlayerData->ModuleEquipCmn5 = playerData->module_equip_cmn5;
			WCHAR val[32];
			swprintf(val, 32, L"%d", playerData->module_equip_cmn0);
			WritePrivateProfileStringW(L"playerdata", L"module_equip_cmn5", val, configPath);
		}

		setIfNotEqual(&playerData->level, customPlayerData->LevelNum, 1);
		setIfNotEqual(&playerData->level_plate_id, customPlayerData->LevelPlateId, 0);
		setIfNotEqual(&playerData->level_plate_effect, customPlayerData->LevelPlateEffect, 0);
		setIfNotEqual(&playerData->skin_equip, customPlayerData->SkinEquip, 0);
		setIfNotEqual(&playerData->btn_se_equip, customPlayerData->BtnSeEquip, -1);
		setIfNotEqual(&playerData->slide_se_equip, customPlayerData->SlideSeEquip, -1);
		setIfNotEqual(&playerData->chainslide_se_equip, customPlayerData->ChainslideSeEquip, -1);
		setIfNotEqual(&playerData->slidertouch_se_equip, customPlayerData->SlidertouchSeEquip, -1);
		setIfNotEqual(&playerData->act_toggle, customPlayerData->ActionSE, 1);
		//setIfNotEqual(&playerData->module_equip0, customPlayerData->ModuleEquip0, 0); Do NOT recommend using this
		//setIfNotEqual(&playerData->module_equip1, customPlayerData->ModuleEquip1, 0);
		//setIfNotEqual(&playerData->module_equip2, customPlayerData->ModuleEquip2, 0);
		//setIfNotEqual(&playerData->module_equip3, customPlayerData->ModuleEquip3, 0);
		//setIfNotEqual(&playerData->module_equip4, customPlayerData->ModuleEquip4, 0);
		//setIfNotEqual(&playerData->module_equip5, customPlayerData->ModuleEquip5, 0);

		// Display clear borders on the progress bar
		playerData->clear_border = (customPlayerData->ShowRivalClearBorder << 2) | (customPlayerData->ShowExcellentClearBorder << 1) | (customPlayerData->ShowGreatClearBorder);
		
		playerData->use_card = customPlayerData->UseCard; // required to allow for module selection

		playerData->game_opts = customPlayerData->GameModifierOptions; // hi-speed, etc..

		memset((void*)MODULE_TABLE_START, 0xFF, 128);
		memset((void*)ITEM_TABLE_START, 0xFF, 128);

		playerData->mylistA_begin = customPlayerData->Mylist[0].begin()._Ptr;
		playerData->mylistA_end = customPlayerData->Mylist[0].end()._Ptr;
		playerData->mylistB_begin = customPlayerData->Mylist[1].begin()._Ptr;
		playerData->mylistB_end = customPlayerData->Mylist[1].end()._Ptr;
		playerData->mylistC_begin = customPlayerData->Mylist[2].begin()._Ptr;
		playerData->mylistC_end = customPlayerData->Mylist[2].end()._Ptr;

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
