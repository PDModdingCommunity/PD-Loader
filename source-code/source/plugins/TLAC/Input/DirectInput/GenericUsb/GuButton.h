#pragma once

namespace TLAC::Input
{
	enum GuDirection
	{
		GU_DIR_UP, 
		GU_DIR_RIGHT, 
		GU_DIR_DOWN, 
		GU_DIR_LEFT
	};

	enum GuButton : int
	{
		GU_BUTTON1 = 0,
		GU_BUTTON2 = 1,
		GU_BUTTON3 = 2,
		GU_BUTTON4 = 3,
		
		GU_BUTTON5 = 4,
		GU_BUTTON6 = 5,

		GU_BUTTON7 = 6,
		GU_BUTTON8 = 7,

		GU_BUTTON9 = 8,
		GU_BUTTON10 = 9,

		GU_BUTTON11 = 10,
		GU_BUTTON12 = 11,

		GU_BUTTON13 = 12,
		
	
		GU_DPAD_UP = 14,
		GU_DPAD_RIGHT = 15,
		GU_DPAD_DOWN = 16,
		GU_DPAD_LEFT = 17,

		GU_L_STICK_UP = 18,
		GU_L_STICK_RIGHT = 19,
		GU_L_STICK_DOWN = 20,
		GU_L_STICK_LEFT = 21,

		GU_R_STICK_UP = 22,
		GU_R_STICK_RIGHT = 23,
		GU_R_STICK_DOWN = 24,
		GU_R_STICK_LEFT = 25,

		GU_BUTTON_MAX = 26,
	};
}