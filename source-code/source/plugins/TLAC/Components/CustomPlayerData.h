#pragma once
#include <string>

namespace TLAC::Components
{
	struct CustomPlayerData
	{
		std::string *PlayerName;
		std::string* LevelName;
		int LevelPlateId;
		int LevelPlateEffect;
		int SkinEquip;
		int BtnSeEquip;
		int SlideSeEquip;
		int ChainslideSeEquip;
		int SlidertouchSeEquip;
		bool ShowGreatClearBorder;
		bool ShowExcellentClearBorder;
		bool ShowRivalClearBorder;
		bool UseCard;
		bool GameModifierOptions;
		bool ActionSE;
	};
}
