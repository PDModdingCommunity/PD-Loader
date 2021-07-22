﻿#pragma once

void writeTemplate(char* cfgtemplate, const wchar_t* filename);

char config_template[] =
{"#########################################\n"
"# /!\ WARNING: DO NOT EDIT WITH NOTEPAD!#\n"
"#########################################\n"
"\n"
"[Global]\n"
"# Load plugins including below options\n"
"Enable=1\n"
"# Enable built-in patches\n"
"Builtin_Patches=1\n"
"# Enable custom patches\n"
"Custom_Patches=1\n"
"# Enable built-in render\n"
"Builtin_Render=1\n"
"# PC\n"
"pc=\n"
"\n"
"[GPU]\n"
"# Force GPU model -- -1=auto, 0=Kepler, 1=Maxwell, 2=Turing, 3=Ampere\n"
"Model=-1\n"
"\n"
"[Launcher]\n"
"# Forces the launcher to be skipped, you can use the --launch parameter instead of this.\n"
"Skip=0\n"
"# Disables unsupported GPU warning dialogs.\n"
"No_GPU_Dialog=0\n"
"# Sets the dark colour scheme in the launcher.\n"
"Dark_Launcher=0\n"
"# Ignores some OpenGL-related errors. Don't use both patches at the same time unless you're know what you're doing\n"
"OpenGL_Patch_A=0\n"
"# Ignores some OpenGL-related errors. Don't use both patches at the same time unless you're know what you're doing\n"
"OpenGL_Patch_B=0\n"
"# Command line arguments\n"
"Command_Line=\n"
"# Launches divahook.bat intead of diva.exe.\n"
"Use_divahook_bat=0\n"
"\n"
"[Patches]\n"
"# Disable movies (enable this if the game hangs when loading certain PVs)\n"
"No_Movies=0\n"
"# Enable or disable mouse cursor\n"
"Cursor=1\n"
"# Use 2 channels instead of 4 (when not using DivaSound)\n"
"Stereo=1\n"
"# Skip one or more menus -- 0=disabled, 1=guest, 2=guest+normal\n"
"Quick_Start=1\n"
"# Disable the scrolling sound effect\n"
"No_Scrolling_SFX=0\n"
"# Enhanced Stage Manager (sets a custom number of stages; \"stage_manager\" should be disabled in \"components.ini\")\n"
"# Number of stages (0 = disabled)\n"
"Enhanced_Stage_Manager=0\n"
"# Use encore stages or not\n"
"Enhanced_Stage_Manager_Encore=1\n"
"# Change the mouth animations -- 0=default, 1=force PDA, 2=force FT\n"
"Force_Mouth=0\n"
"# Change the expressions -- 0=default, 1=force PDA, 2=force FT\n"
"Force_Expressions=0\n"
"# Change the look animations -- 0=default, 1=force PDA, 2=force FT\n"
"Force_Look=0\n"
"# Disable hand scaling\n"
"No_Hand_Scaling=0\n"
"# Default hand size -- -1=default, 12200=PDA\n"
"Default_Hand_Size=-1\n"
"# Show \"FREE PLAY\" instead of \"CREDIT(S)\" and don't require credits\n"
"Freeplay=1\n"
"# Sing missed notes\n"
"Sing_Missed=0\n"
"# Hide the volume and SE control buttons\n"
"Hide_Volume=0\n"
"# Remove the photo controls during PV playback\n"
"No_PV_UI=0\n"
"# Hide the watermark that's usually shown in PV viewing mode\n"
"Hide_PV_Watermark=0\n"
"# Set the card reader and network status icons -- 0=default, 1=hidden, 2=error, 3=OK, 4=partial OK\n"
"Status_Icons=0\n"
"# Disable showing lyrics\n"
"No_Lyrics=0\n"
"# Disable the error banner\n"
"No_Error=1\n"
"# Hide \"CREDIT(S)\"/\"FREE PLAY\"\n"
"Hide_Freeplay=0\n"
"# Show \"PD Loader\" instead of \"FREE PLAY\"\n"
"PDLoaderText=1\n"
"# Freeze the timer\n"
"No_Timer=1\n"
"# Disable the timer sprite\n"
"No_Timer_Sprite=1\n"
"# Disable Message Bar\n"
"No_Message_Bar=0\n"
"# Disable Stage Text\n"
"No_Stage_Text=0\n"
"# Unlock PSEUDO modules (incomplete, will default to Miku)\n"
"Unlock_Pseudo=0\n"
"# Enable the card menu (incomplete, it doesn't bypass the card prompt)\n"
"Card=0\n"
"# Disable Extended Data (may break physics in some edits) AKA Skyth's no-opd patch\n"
"No_Opd=0\n"
"# Scale Debug Windows above HD internal resolution AKA somewhatlurker's dwgui patch\n"
"Dwgui_Scaling=0\n"
"# Allow using a real arcade slider attached to COM11 (or PS4 official FT controller)\n"
"Hardware_Slider=0\n"
"# Prevent data deletion\n"
"Prevent_Data_Deletion=0\n"
"\n"
"[Graphics]\n"
"# Lag Compensation\n"
"Lag_Compensation = 0\n"
"# Set to -1 to unlock the frame rate.\n"
"FPS.Limit=60\n"
"# Reduces FPS Limiter CPU usage\n"
"# (may have less consistent performance)\n"
"FPS.Limit.LightMode=1\n"
"# Enable debug warnings for lightweight limiter\n"
"FPS.Limit.Verbose=0\n"
"# Temporal Anti-Aliasing\n"
"TAA=1\n"
"# Morphological Anti-Aliasing\n"
"MLAA=1\n"
"# MAG Filter -- 0=default (bilinear), 1=nearest-neighbour, 2=sharpen, 3=cone (smooth)\n"
"MAG=0\n"
"# Depth of Field\n"
"DOF=1\n"
"# Reflections\n"
"Reflections=1\n"
"# Shadows\n"
"Shadows=1\n"
"# Transparent Meshes\n"
"Punchthrough=1\n"
"# Glare\n"
"Glare=1\n"
"# Shader\n"
"Shader=1\n"
"# Toon shader (press F9 to toggle) -- 0=default, 1=force on, 2=force off\n"
"NPR1=0\n"
"# Disable all 3D passes. WARNING: The extended data will be deleted unless Prevent Data Deletion is enabled. Don't enable on Intel GPUs, enable no_opd instead.\n"
"2D=0\n"
"# FrameRateManager motion rate (fps)\n"
"FRM.Motion.Rate=400\n"
"\n"
"[Resolution]\n"
"# 0 = Windowed, 1 = Fast, 2 = Exclusive, 3 = Safe\n"
"# For same as screen res: -1x-1\n"
"Display=1\n"
"Width=-1\n"
"Height=-1\n"
"# Fullscreen only options\n"
"BitDepth=32\n"
"RefreshRate=60\n"
"\n"
"# Set a custom internal render resolution\n"
"# Same as window: -1x-1\n"
"# HD    : 1280x720\n"
"# FHD   : 1920x1080\n"
"# WQHD  : 2560x1440\n"
"# 4K UHD: 3840x2160\n"
"# 5K UHD: 5120x2880\n"
"# 8K UHD: 7680x4320\n"
"r.Enable=0\n"
"r.Width=-1\n"
"r.Height=-1\n"
"\n"
};

char components_template[] =
{
"#########################################\n"
"# /!\ WARNING: DO NOT EDIT WITH NOTEPAD!#\n"
"#########################################\n"
"\n"
"# components.ini\n"
"# defines which components will be enabled\n"
"[components]\n"
"\n"
"# emulates game input through mouse and keyboard.\n"
"# controls can be set inside the keyconfig.ini file.\n"
"input_emulator = true\n"
"touch_slider_emulator = true\n"
"touch_panel_emulator = true\n"
"\n"
"# freezes the PV select timer at 39 seconds.\n"
"sys_timer = true\n"
"\n"
"# loads user defined const values from the playerdata.ini config\n"
"# and writes them to the game's PlayerData struct.\n"
"# required for modules selection\n"
"player_data_manager = true\n"
"\n"
"# adjusts 2D and 3D animations to retain their original speed at different frame rates.\n"
"frame_rate_manager = false\n"
"\n"
"# skip unnecessary DATA_INITIALIZE loading time, speed up SYSTEM_STARTUP and skip the startup WARNING message.\n"
"# further improvements can be achieved by shortening the duration of the pv_999 script file as it's test loaded during the SYSTEM_STARTUP.\n"
"fast_loader = true\n"
"\n"
"# allows for a user controllable camera which can be toggled using F3,\n"
"# can be controlled using W/A/S/D to move, SPACE/CTRL to ascend/descend, Q/E to rotate, R/F to zoom in/out\n"
"# and holding SHIFT/ALT to speed up or slow down these controls.\n"
"camera_controller = false\n"
"\n"
"# scales the framebuffer so that it will fit the current screen or window\n"
"# required to set resolutions in config.ini\n"
"scale_component = true\n"
"\n"
"# lets the user change the current game state using the F4 - F8 keys, enable the dw_gui / DATA_TESTs\n"
"# and speed up 2D animations / menu navigation by holding SHIFT + TAB.\n"
"# enabling this component is not recommended for most users.\n"
"debug_component = false\n"
"\n"
"# allows the usage of hold transfer\n"
"target_inspector = true\n"
"\n"
"# saves high scores to plugins/scores.ini\n"
"score_saver = true\n"
"\n"
"# adds a pause menu\n"
"pause = true\n"
};

char playerdata_template[] =
{
"[playerdata]\n"
"#########################################\n"
"# /!\ WARNING: DO NOT EDIT WITH NOTEPAD!#\n"
"#########################################\n"
"\n"
"# playerdata.ini\n"
"# defines constant values to be loaded into the PlayerData struct\n"
"# this file has to be encoded using utf-8\n"
"player_name =ＮＯ－ＮＡＭＥ\n"
"level_name =忘れないでね私の声を\n"
"level =1\n"
"vocaloid_point=0\n"
"\n"
"# IDs defined in rom/gm_plate_tbl.farc/gm_plate_id.bin\n"
"level_plate_id =0\n"
"level_plate_effect=1\n"
"# IDs same as gam_skin%3d\n"
"skin_equip =0\n"
"# Allows for song specific skin settings which can be set in the file skins.ini\n"
"use_pv_skin_equip =true\n"
"# IDs defined in rom/gm_btn_se_tbl.farc/gm_btn_se_id.bin\n"
"btn_se_equip =-1\n"
"# Allows for song specific sound effect settings which can be set in the file sfx.ini\n"
"use_pv_sfx_equip =true\n"
"# IDs defined in rom/gm_slide_se_tbl.farc/gm_slide_se_id.bin\n"
"slide_se_equip =-1\n"
"# IDs defined in rom/gm_chainslide_se_tbl.farc/gm_chainslide_se_id.bin\n"
"chainslide_se_equip =-1\n"
"slidertouch_se_equip=-1\n"
"# \"Shared Set\" or \"Original\" setting, works similarly to PSP series module selection (Updates when changed in-game)\n"
"module_equip_cmn0=0\n"
"module_equip_cmn1=0\n"
"module_equip_cmn2=0\n"
"module_equip_cmn3=0\n"
"module_equip_cmn4=0\n"
"module_equip_cmn5=0\n"
"# Allows Song Specific module selections (values update when changed in-game in the file modules.ini) (May lag on song select and menu)\n"
"use_pv_module_equip=true\n"
"# Enable button/slider sounds & volume settings\n"
"act_toggle =true\n"
"act_vol =100\n"
"act_slide_vol=100\n"
"# Show clear borders on Progress Bar for:\n"
"# Great\n"
"border_great =true\n"
"# Excellent\n"
"border_excellent =true\n"
"# Rival\n"
"border_rival=true\n"
"\n"
"# Allows module selection (WARNING: It may cause lag in the main menu and other places) (May be necessary for use_pv_equip settings!)\n"
"use_card =true\n"
"\n"
"# Allows module selection without card and attempts to disable module refresh when sliding in PV select to reduce CPU and IO load.\n"
"module_card_workaround =false\n"
"\n"
"# Allows use of game mode modifiers (hi-speed, hidden, and sudden)\n"
"gamemode_options =true\n"
"\n"
"\n"
"\n"
"mylist_A=\n"
"mylist_B=\n"
"mylist_C=\n"
};

char keyconfig_template[] =
{
"#########################################\n"
"# /!\ WARNING: DO NOT EDIT WITH NOTEPAD!#\n"
"#########################################\n"
"[keyconfig]\n"
"# keyconfig.ini\n"
"# defines the input_emulator key bindings\n"
"# multiple keys are to be specified comma separated\n"
"\n"
"# Options:\n"
"\n"
"# Allow the user to mouse wheel scroll through the PV selection list\n"
"mouse_scroll_pv_selection = true\n"
"# The interval it takes for the emulated slider touch points to move from one contact point to another.\n"
"# In most cases, this does not need editing.\n"
"touch_slider_emulation_speed = 750.0\n"
"\n"
"# Preferred Xinput controller (0-3)\n"
"xinput_preferred =0\n"
"\n"
"# DirectInput Vendor ID and Product ID\n"
"# Check them in Device Manager or dxdiag.exe\n"
"VID = 046D\n"
"PID = C216\n"
"\n"
"# Rumble (Xinput)\n"
"rumble =true\n"
"\n"
"# Pause when focus is lost\n"
"autopause =true\n"
"\n"
"# Allow using slider in menus\n"
"# Recommended to enable this and remove MENU_L/R bindings if you have a real slider.\n"
"# Also needed to see slider in input test\n"
"slider_in_menus =false\n"
"\n"
"# Use the hardware slider from HORI FT official controller (PS4 version)\n"
"# Requires enabling hardware slider support.\n"
"# Warning: alpha feature, may not work properly\n"
"# Recommended to remove Ds4_L_Stick_* and Ds4_R_Stick_* from button bindings (JVS_LEFT, JVS_RIGHT, MENU_L, MENU_R).\n"
"ps4_official_slider =false\n"
"\n"
"# JVS Buttons:\n"
"JVS_TEST = F1\n"
"JVS_SERVICE = F2\n"
"JVS_SW1 = F11\n"
"JVS_SW2 = F12\n"
"\n"
"JVS_START = Enter, Ds4_Options, XINPUT_START, MouseMiddle\n"
"JVS_TRIANGLE = W, I, Ds4_Triangle, Ds4_DPad_Up, Ds4_L_Trigger, Ds4_R_Trigger, XINPUT_Y, XINPUT_UP, XINPUT_LT, XINPUT_RT\n"
"JVS_SQUARE = A, J, Ds4_Square, Ds4_DPad_Left, Ds4_L_Trigger, Ds4_R_Trigger, XINPUT_X, XINPUT_LEFT, XINPUT_LT, XINPUT_RT, MouseX2\n"
"JVS_CROSS = S, K, Ds4_Cross, Ds4_DPad_Down, Ds4_L_Trigger, Ds4_R_Trigger, XINPUT_A, XINPUT_DOWN, XINPUT_LT, XINPUT_RT, MouseX1\n"
"JVS_CIRCLE = D, L, Ds4_Circle, Ds4_DPad_Right, Ds4_L_Trigger, Ds4_R_Trigger, XINPUT_B, XINPUT_RIGHT, XINPUT_LT, XINPUT_RT\n"
"\n"
"JVS_LEFT = Left, Up, Ds4_L1, Ds4_L_Stick_Up, Ds4_R_Stick_Up, XINPUT_LS\n"
"JVS_RIGHT = Right, Down, Ds4_R1, Ds4_L_Stick_Down, Ds4_R_Stick_Down, XINPUT_RS\n"
"\n"
"MENU_L = Left, Up, Ds4_L1, XINPUT_LS, Ds4_DPad_Left, Ds4_DPad_Up, XINPUT_LEFT, XINPUT_UP, Q, U, Ds4_L_Stick_Left, XINPUT_LLEFT, Ds4_L_Stick_Up, XINPUT_LUP, Ds4_R_Stick_Left, XINPUT_RLEFT, Ds4_R_Stick_Up, XINPUT_RUP\n"
"MENU_R = Right, Down, Ds4_R1, XINPUT_RS, Ds4_DPad_Right, Ds4_DPad_Down, XINPUT_RIGHT, XINPUT_DOWN, E, O, Ds4_L_Stick_Right, XINPUT_LRIGHT, Ds4_L_Stick_Down, XINPUT_LDOWN, Ds4_R_Stick_Right, XINPUT_RRIGHT, Ds4_R_Stick_Down, XINPUT_RDOWN\n"
"MENU_CIRCLE = Spacebar\n"
"\n"
"COIN = F10\n"
"\n"
"WIREFRAME = F12\n"
"\n"
"# Touch Slider:\n"
"LEFT_SIDE_SLIDE_LEFT = Q, Ds4_L_Stick_Left, XINPUT_LLEFT\n"
"LEFT_SIDE_SLIDE_RIGHT = E, Ds4_L_Stick_Right, XINPUT_LRIGHT\n"
"\n"
"RIGHT_SIDE_SLIDE_LEFT = U, Ds4_R_Stick_Left, XINPUT_RLEFT\n"
"RIGHT_SIDE_SLIDE_RIGHT = O, Ds4_R_Stick_Right, XINPUT_RRIGHT\n"
"\n"
"# --- KEYBOARD KEY NAMES: ---\n"
"\n"
"# - Standard Keys\n"
"# uppercase \"A\" to \"Z\", \"0\" to \"9\"\n"
"\n"
"# - NumPad Keys\n"
"# \"NumPad0\" to \"NumPad9\"\n"
"# \"Plus\", \"Minus\", \"Divide\", \"Multiply\"\n"
"\n"
"# - Function Keys\n"
"# \"F1\" to \"F24\"\n"
"\n"
"# - Shift Keys\n"
"# \"LeftShift\", \"RightShift\"\n"
"\n"
"# - Control Keys\n"
"# \"LeftControl\", \"RightControl\"\n"
"\n"
"# - Arrow Keys\n"
"# \"Up\", \"Down\", \"Left\", \"Right\"\n"
"\n"
"# - Special Keys (may vary depending on the layout)\n"
"# \"Enter\", \"Tab\", \"Backspace\", \"Spacebar\", \"Insert\", \"Delete\", \"Home\", \"End\", \"PageUp\", \"PageDown\", \"Escape\"\n"
"# \"Comma\", \"Period\", \"Colon\", \"Slash\", \"Tilde\", \"LeftBracket\", \"Backslash\", \"RightBracket\", \"SingleDoubleQuote\", \"OEM_8\"\n"
"\n"
"# --- MOUSE BUTTON NAMES: ---\n"
"# \"MouseLeft\", \"MouseMiddle\", \"MouseRight\", \"MouseX1\", \"MouseX2\"\n"
"\n"
"# --- DUALSHOCK 4 BUTTON NAMES: ---\n"
"\n"
"# - Face Buttons\n"
"# \"Ds4_Square\", \"Ds4_Cross\", \"Ds4_Circle\", \"Ds4_Triangle\"\n"
"\n"
"# - Standard Buttons\n"
"# \"Ds4_Share\", \"Ds4_Options\", \"Ds4_PS\", \"Ds4_Touch\", \"Ds4_L1\", \"Ds4_R1\"\n"
"\n"
"# - D-Pad Directions\n"
"# \"Ds4_DPad_Up\", \"Ds4_DPad_Right\", \"Ds4_DPad_Down\", \"Ds4_DPad_Left\"\n"
"\n"
"# - Trigger Buttons\n"
"# \"Ds4_L_Trigger\", \"Ds4_R_Trigger\"\n"
"\n"
"# - Left Joystick\n"
"# \"Ds4_L_Stick_Up\", \"Ds4_L_Stick_Right\", \"Ds4_L_Stick_Down\", \"Ds4_L_Stick_Left\", \"Ds4_L3\"\n"
"\n"
"# - Right Joystick\n"
"# \"Ds4_R_Stick_Up\", \"Ds4_R_Stick_Right\", \"Ds4_R_Stick_Down\", \"Ds4_R_Stick_Left\", \"Ds4_R3\"\n"
"\n"
"# --- XINPUT BUTTON NAMES: ---\n"
"\n"
"# - Face Buttons\n"
"# \"XINPUT_A\", \"XINPUT_B\", \"XINPUT_X\", \"XINPUT_Y\"\n"
"\n"
"# - Standard Buttons\n"
"# \"XINPUT_START\", \"XINPUT_BACK\", \"XINPUT_LS\"/\"XINPUT_LB\", \"XINPUT_RS\"/\"XINPUT_RB\"\n"
"\n"
"# - D-Pad Directions\n"
"# \"XINPUT_UP\", \"XINPUT_DOWN\", \"XINPUT_LEFT\", \"XINPUT_RIGHT\"\n"
"\n"
"# - Trigger Buttons\n"
"# \"XINPUT_LT\", \"XINPUT_RT\"\n"
"\n"
"# - Left Joystick\n"
"# \"XINPUT_LLEFT\", \"XINPUT_LRIGHT\", \"XINPUT_LUP\", \"XINPUT_LDOWN\", \"XINPUT_LSB\"\n"
"\n"
"# - Right Joystick\n"
"# \"XINPUT_RLEFT\", \"XINPUT_RRIGHT\", \"XINPUT_RUP\", \"XINPUT_RDOWN\", \"XINPUT_RSB\"\n"
"\n"
"# --- Generic USB BUTTON NAMES: ---\n"
"\n"
"# - Standard Buttons (Button number in \"Game controller settings\", max allowed 13 buttons at the moment)\n"
"# \"GU_BUTTON1\", \"GU_BUTTON2\", \"GU_BUTTON3\", \"GU_BUTTON4\", \"GU_BUTTON5\", \"GU_BUTTON6\", \"GU_BUTTON7\", \"GU_BUTTON8\", \"GU_BUTTON9\", \"GU_BUTTON10\", \"GU_BUTTON11\", \"GU_BUTTON12\", \"GU_BUTTON13\"\n"
"\n"
"# - D-Pad Directions\n"
"# \"GU_DPAD_UP\", \"GU_DPAD_RIGHT\", \"GU_DPAD_DOWN\", \"GU_DPAD_LEFT\"\n"
"\n"
"# - Left Joystick\n"
"# \"GU_L_STICK_UP\", \"GU_L_STICK_RIGHT\", \"GU_L_STICK_DOWN\", \"GU_L_STICK_LEFT\"\n"
"\n"
"# - Right Joystick\n"
"# \"GU_R_STICK_UP\", \"GU_R_STICK_RIGHT\", \"GU_R_STICK_DOWN\", \"GU_R_STICK_LEFT\"\n"
};

char divasound_template[] =
{
"#########################################\n"
"# /!\ WARNING: DO NOT EDIT WITH NOTEPAD!#\n"
"#########################################\n"
"\n"
"[General]\n"
"# You can choose 2 channel or 4 channel mode.\n"
"# 4 channels uses front channels for speakers and rear channels as headphones.\n"
"# 2 channels only has headphones output (with in-game volume bypassed).\n"
"Channels=2\n"
"\n"
"# 16 or 24 bit integer and 32 bit float output are supported.\n"
"# If you don't know what this means, you should leave it at 16.\n"
"# On some versions of Windows floating point samples may increase latency.\n"
"Bit_Depth=16\n"
"\n"
"# This can be WASAPI, DirectSound, WASAPI_Exclusive, or ASIO.\n"
"# Only use DirectSound if WASAPI doesn't work for some reason.\n"
"# WASAPI_Exclusive may not work with some hardware, but can have lower latency.\n"
"# ASIO requires special driver support.\n"
"# ASIO doesn't use the buffer settings. Use ASIO config instead.\n"
"# For full descriptions of the backends see https://github.com/somewhatlurker/DivaSound/wiki/Backends.\n"
"Backend=WASAPI\n"
"\n"
"# If the game crashes when loading, try setting this to 0.\n"
"Alternate_Init=1\n"
"\n"
"[Buffer]\n"
"# Sets the requested buffer size in milliseconds.\n"
"# Try increasing this if audio stutters or desyncs.\n"
"# WASAPI will usually provide a larger buffer than specified.\n"
"# You can check the actual buffer size by enabling debug_component.\n"
"Buffer_Size=10\n"
"\n"
"# Sets how many periods to use.\n"
"# Fewer periods = lower latency, but it may cause issues.\n"
"# You probably shouldn't set this below 2, and increasing it probably isn't necessary.\n"
"Periods=2\n"
"\n"
"[ASIO]\n"
"# Sets the device ID.\n"
"# 0=first device(...), -1=first available device\n"
"Device=-1\n"
"\n"
"# When set to 1 the ASIO driver config will open alongside the game.\n"
"# Use this to adjust output settings, then disable it.\n"
"Show_Config=0\n"
};

char shaderpatchconfig_template[] =
{
"[Config]\n"
"# Patches to remove glitching with modern GPUs.\n"
"# Appropriate patches will automatically be selected based on your GPU.\n"
"Compat=1\n"
"\n"
"# Lyb's toon shader improvements (24 sept 2019)\n"
"# Toon_Improve: Adjusts toon shader lighting\n"
"Toon_Improve=0\n"
"# Specular Multiplier\n"
"Toon_Improve_Val1=0.9\n"
"# Specular Offset\n"
"Toon_Improve_Val2=-0.5\n"
"\n"
"# Lyb's toon shader improvements (24 sept 2019)\n"
"# Toon_Eyes_Improve: Adjusts toon shader eyes\n"
"Toon_Eyes_Improve=0\n"
"\n"
"# Lyb's toon shader improvements (24 sept 2019)\n"
"# Toon_Hair_Improve: Adjusts toon shader hair\n"
"Toon_Hair_Improve=0\n"
"# Diffuse Brightness\n"
"Toon_Hair_Improve_Val1=1.25\n"
"\n"
"# Lyb's toon shader improvements (24 sept 2019)\n"
"# Toon_Lines_Improve: Adjusts toon shader outlines\n"
"Toon_Lines_Improve=0\n"
"# ???\n"
"Toon_Lines_Improve_Val1=0.75\n"
"# Thickness Multiplier(?)\n"
"Toon_Lines_Improve_Val2=0.75\n"
"# Thickness Offset(?)\n"
"Toon_Lines_Improve_Val3=0.75\n"
"\n"
"# Debug patches:\n"
"# Nametags\n"
"# Adds comments with the shader file name to the shader text. Useful for debugging issues.\n"
"# You only need to enable one of these.\n"
"# `nametags2` is a bit faster, but the tags are placed at the end of the file and you may not like that.\n"
"Nametags1=0\n"
"Nametags2=0\n"
};
