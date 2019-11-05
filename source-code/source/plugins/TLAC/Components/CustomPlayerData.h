#pragma once
#include <string>
#include <vector>

namespace TLAC::Components
{
	struct CustomPlayerData
	{
		std::string *PlayerName;
		std::string* LevelName;
		int VocaloidPoints;
		int LevelNum;
		int LevelPlateId;
		int LevelPlateEffect;
		int SkinEquip;
		int BtnSeEquip;
		int SlideSeEquip;
		int ChainslideSeEquip;
		int SlidertouchSeEquip;
		int ModuleEquip0;
		int ModuleEquip1;
		int ModuleEquip2;
		int ModuleEquip3;
		int ModuleEquip4;
		int ModuleEquip5;
		int ModuleEquipCmn0;
		int ModuleEquipCmn1;
		int ModuleEquipCmn2;
		int ModuleEquipCmn3;
		int ModuleEquipCmn4;
		int ModuleEquipCmn5;
		bool ShowGreatClearBorder;
		bool ShowExcellentClearBorder;
		bool ShowRivalClearBorder;
		bool UseCard;
		bool GameModifierOptions;
		bool ActionSE;
		std::vector<int> Mylist[3];
	};
}
