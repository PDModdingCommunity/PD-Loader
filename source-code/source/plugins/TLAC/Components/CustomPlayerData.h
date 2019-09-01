#pragma once
#include <string>

namespace TLAC::Components
{
	struct CustomPlayerData
	{
		std::string *PlayerName;
		std::string* LevelName;
		int LevelPlateId;
		int SkinEquip;
		int BtnSeEquip;
		int SlideSeEquip;
		int ChainslideSeEquip;
		bool ShowGreatClearBorder;
		bool ShowExcellentClearBorder;
		bool UseCard;
		bool GameModifierOptions;
		bool ActionSE;
	};
}
