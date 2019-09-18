#pragma once
#include <stdint.h>

constexpr uint8_t NOP_OPCODE = 0x90;
constexpr uint8_t RET_OPCODE = 0xC3;
constexpr uint8_t JMP_OPCODE = 0xE9;
constexpr uint8_t JNE_OPCODE = 0x85;

//constexpr uint64_t ENGINE_UPDATE_HOOK_TARGET_ADDRESS	= 0x000000014018CC40;
constexpr uint64_t ENGINE_UPDATE_INPUT_ADDRESS			= 0x000000014018CBB0;

constexpr uint64_t CURRENT_GAME_STATE_ADDRESS			= 0x0000000140EDA810;
constexpr uint64_t CURRENT_GAME_SUB_STATE_ADDRESS		= 0x0000000140EDA82C;
constexpr uint64_t RESOLUTION_WIDTH_ADDRESS				= 0x0000000140EDA8BC;
constexpr uint64_t RESOLUTION_HEIGHT_ADDRESS			= 0x0000000140EDA8C0;
		  
constexpr uint64_t SYSTEM_WARNING_ELAPSED_ADDRESS		= (0x00000001411A1430 + 0x68);
constexpr uint64_t DATA_INIT_STATE_ADDRESS				= 0x0000000140EDA7A8;
		  
constexpr uint64_t AET_FRAME_DURATION_ADDRESS			= 0x00000001409A0A58;
constexpr uint64_t PV_FRAME_RATE_ADDRESS				= 0x0000000140EDA7CC;
constexpr uint64_t FRAME_SPEED_ADDRESS					= 0x0000000140EDA798;
constexpr uint64_t FRAME_RATE_ADDRESS					= 0x0000000140EDA6D0;
constexpr uint64_t USE_AUTO_FRAMESPEED_ADDRESS			= 0x0000000140EDA79C;
constexpr uint64_t AUTO_FRAMESPEED_TARGET_FRAMERATE_ADDRESS = 0x0000000140EDA6CC;
		  
constexpr uint64_t DW_GUI_DISPLAY_INSTANCE_PTR_ADDRESS	= 0x0000000141190108;
constexpr uint64_t INPUT_STATE_PTR_ADDRESS				= 0x0000000140EDA330;
constexpr uint64_t SLIDER_CTRL_TASK_ADDRESS				= 0x000000014CC5DE40;
constexpr uint64_t TASK_TOUCH_ADDRESS					= 0x000000014CC9EC30;
constexpr uint64_t SEL_PV_TIME_ADDRESS					= 0x000000014CC12498;
constexpr uint64_t PLAYER_DATA_ADDRESS					= 0x00000001411A8850;
constexpr uint64_t SET_DEFAULT_PLAYER_DATA_ADDRESS		= 0x00000001404A7370;
constexpr uint64_t PLAYS_PER_SESSION_GETTER_ADDRESS		= 0x000000014038AEE0;
constexpr uint64_t PV_SEL_SLOTS_TO_SCROLL				= 0x000000014CC12470;
constexpr uint64_t PV_SEL_SLOTS_CONST					= 0x000000014CC119C8;
constexpr uint64_t MODULE_SEL_SLOTS_TO_SCROLL			= 0x00000001418047EC;
constexpr uint64_t MODULE_IS_RECOMMENDED				= 0x00000001418047E0;

constexpr uint64_t CAMERA_ADDRESS						= 0x0000000140FBC2C0;
constexpr uint64_t CAMERA_POS_SETTER_ADDRESS			= 0x00000001401F9460;
constexpr uint64_t CAMERA_INTR_SETTER_ADDRESS			= 0x00000001401F93F0;
constexpr uint64_t CAMERA_ROT_SETTER_ADDRESS			= 0x00000001401F9480;
constexpr uint64_t CAMERA_PERS_SETTER_ADDRESS			= 0x00000001401F9430;

constexpr uint64_t UPDATE_TASKS_ADDRESS					= 0x000000014019B980;
constexpr uint64_t GLUT_SET_CURSOR_ADDRESS				= 0x00000001408B68E6;
constexpr uint64_t CHANGE_MODE_ADDRESS					= 0x00000001401953D0;
constexpr uint64_t CHANGE_SUB_MODE_ADDRESS				= 0x0000000140195260;

constexpr uint64_t TGT_CLASS_ADDRESS = 0x0000000140D0B660;
constexpr uint64_t TGT_STATES_FIRST_POINTER_ADDRESS = 0x0000000140D0B680;
constexpr uint64_t TGT_STATES_BASE_ADDRESS = 0x0000000140D0B688;
constexpr uint64_t TGT_ON_SCREEN_ADDRESS = 0x0000000140D0B678;

constexpr uint64_t HOLD_STATE_ADDRESS = 0x0000000140D1E20C;
constexpr uint64_t MAX_HOLD_STATE_ADDRESS = 0x0000000140D1E234;

constexpr uint64_t CHECK_SOMETHING_SET_MODULE_ADDRESS = 0x0000000140581C78;
constexpr uint64_t MODSELECTOR_CHECK_FUNCTION_ERRRET_ADDRESS = 0x00000001405869AD;
constexpr uint64_t MODSELECTOR_CLOSE_AFTER_MODULE = 0x0000000140583B45;
constexpr uint64_t MODSELECTOR_CLOSE_AFTER_CUSTOMIZE = 0x0000000140583C8C;
constexpr uint64_t MODULE_TABLE_START = PLAYER_DATA_ADDRESS + 0x140;
constexpr uint64_t MODULE_TABLE_END = MODULE_TABLE_START + 128;
constexpr uint64_t ITEM_TABLE_START = PLAYER_DATA_ADDRESS + 0x2B8;
constexpr uint64_t ITEM_TABLE_END = ITEM_TABLE_START + 128;

constexpr uint64_t FB_WIDTH_ADDRESS = 0x00000001411ABCA8;
constexpr uint64_t FB_HEIGHT_ADDRESS = 0x00000001411ABCAC;
constexpr uint64_t FB1_WIDTH_ADDRESS = 0x00000001411AD5F8;
constexpr uint64_t FB1_HEIGHT_ADDRESS = 0x00000001411AD5FC;
constexpr uint64_t FB2_WIDTH_ADDRESS = 0x0000000140EDA8E4;
constexpr uint64_t FB2_HEIGHT_ADDRESS = 0x0000000140EDA8E8;

constexpr uint64_t FB_RESOLUTION_WIDTH_ADDRESS = 0x00000001411ABB50;
constexpr uint64_t FB_RESOLUTION_HEIGHT_ADDRESS = 0x00000001411ABB54;

constexpr uint64_t UI_WIDTH_ADDRESS = 0x000000014CC621E4;
constexpr uint64_t UI_HEIGHT_ADDRESS = 0x000000014CC621E8;

constexpr uint64_t FB_ASPECT_RATIO = 0x0000000140FBC2E8;
constexpr uint64_t UI_ASPECT_RATIO = 0x000000014CC621D0;

constexpr uint64_t GET_SLIDER_TAPPED_ADDRESS = 0x0000000140618C60;
constexpr uint64_t GET_SLIDER_RELEASED_ADDRESS = 0x0000000140618C40;
constexpr uint64_t GET_SLIDER_DOWN_ADDRESS = 0x0000000140618C20;

constexpr uint64_t SHOULD_EXIT_BOOL_ADDRESS = 0x0000000140EDA6B0;
constexpr uint64_t DOEXIT_ADDRESS = 0x000000014085B574;

constexpr uint64_t RESULTS_INIT_ADDRESS = 0x000000014065BBC0;
constexpr uint64_t RESULTS_BASE_ADDRESS = 0x000000014CC93830;
constexpr uint64_t GAME_INFO_ADDRESS = 0x0000000141197E00;

constexpr uint64_t CURRENT_SONG_NAME_ADDRESS = 0x0000000140D0A578;
constexpr uint64_t CURRENT_SONG_ID_ADDRESS = 0x0000000140CDD8E0;
constexpr uint64_t SELPV_CURRENT_SONG_ADDRESS = 0x000000014CC12438;

#define XINPUT_A 0x00
#define XINPUT_B 0x01
#define XINPUT_X 0x02
#define XINPUT_Y 0x03
#define XINPUT_UP 0x10
#define XINPUT_DOWN 0x11
#define XINPUT_LEFT 0x12
#define XINPUT_RIGHT 0x13
#define XINPUT_LS 0x20
#define XINPUT_RS 0x21
#define XINPUT_LT 0x22
#define XINPUT_RT 0x23
#define XINPUT_LSB 0x24
#define XINPUT_RSB 0x25
#define XINPUT_START 0x30
#define XINPUT_BACK 0x31
#define XINPUT_LUP 0x40
#define XINPUT_LDOWN 0x41
#define XINPUT_LLEFT 0x42
#define XINPUT_LRIGHT 0x43
#define XINPUT_RUP 0x50
#define XINPUT_RDOWN 0x51
#define XINPUT_RLEFT 0x52
#define XINPUT_RRIGHT 0x53  