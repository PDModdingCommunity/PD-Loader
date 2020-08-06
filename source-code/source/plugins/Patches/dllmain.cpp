#include "windows.h"
#include "vector"
#include <tchar.h>
#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include "PluginConfigApi.h"

#include <detours.h>
#include "framework.h"
#pragma comment(lib, "detours.lib")

unsigned short game_version = 710;

void InjectCode(void* address, const std::vector<uint8_t> data);
void ApplyPatches();
void ApplyCustomPatches(std::wstring CPATCH_FILE_STRING);

const LPCWSTR CONFIG_FILE = L".\\config.ini";

HMODULE *hModulePtr;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hModulePtr = &hModule;
		if (*(char*)0x140A570F0 == '6') game_version = 600;
		ApplyPatches();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void patchMovieExt(std::string moviefile, void* address)
{
	bool isinmdata = 0;
	try {
		for (std::filesystem::path p : std::filesystem::directory_iterator("../mdata"))
		{
			if (std::filesystem::path(p).filename().string().at(0) != 'M' || std::filesystem::path(p).filename().string().length() > 4) continue;
			if (std::filesystem::exists(p.string() + "/rom/movie/" + moviefile + ".mp4"))
			{
				isinmdata = 1;
				//std::cout << "[Patches] Movie " << moviefile << ".mp4 found in M" << ss.str() << std::endl;
				break;
			}
		}
	}
	catch (const std::filesystem::filesystem_error & e) {
		std::cout << "[Patches] File system error " << e.what() << " " << e.path1() << " " << e.path2() << " " << e.code() << std::endl;
	}

	if (isinmdata || std::filesystem::exists("../rom/movie/" + moviefile + ".mp4"))
	{
		InjectCode(address, { 0x6D, 0x70, 0x34 });
		std::cout << "[Patches] Movie " << moviefile << " patched to mp4\n";
		return;
	}
	//std::cout << "[Patches] Movie " << moviefile << " NOT patched to mp4\n";
	return;
}

void ApplyPatches() {
	const struct { void* Address; std::vector<uint8_t> Data; } patches_600[] =
	{
		// Always return true for the SelCredit enter SelPv check
		{ (void*)0x000000014037A560, { 0xB0, 0x01, 0xC3, 0x90, 0x90, 0x90 } },
		// Just completely ignore all SYSTEM_STARTUP errors
		{ (void*)0x00000001403DC590, { 0xC3 } },
		// Always exit TASK_MODE_APP_ERROR on the first frame
		{ (void*)0x00000001403DE8B7, { 0x90, 0x90 } },
		{ (void*)0x00000001403DE8D3, { 0x89, 0xD1, 0x90 } },
		// Ignore the EngineClear variable to clear the framebuffer at all resolutions
		{ (void*)0x00000001404E7470, { 0x90, 0x90 } },
		{ (void*)0x00000001404E7505, { 0x90, 0x90 } },
		// Write ram files to the current directory instead of Y : / SBZV / ram
		{ (void*)0x0000000140648AE9, { 0xE9, 0xD8, 0x00 } },
		// Change mdata path from "C:/Mount/Option" to "mdata/"
		{ (void*)0x0000000140A51058, { 0x6D, 0x64, 0x61, 0x74, 0x61, 0x2F, 0x00 } },
		{ (void*)0x0000000140648A8E, { 0x06 } },
		// Skip parts of the network check state
		{ (void*)0x000000014064D391, { 0xE9, 0x22, 0x03, 0x00 } },
		// Set the initial DHCP WAIT timer value to 0
		{ (void*)0x000000014064E0C7, { 0x00, 0x00 } },
		// Ignore SYSTEM_STARTUP Location Server checks
		{ (void*)0x000000014064EE82, { 0x90, 0x90 } },
		// Toon Shader Fix by lybxlpsv
		{ (void*)0x00000001404E813F, { 0x90 } },
		{ (void*)0x00000001404E8140, { 0x90 } },
		// Toon Shader Outline Fix by lybxlpsv
		{ (void*)0x000000014061C6B2, { 0x01 } },
		// Skip unnecessary checks
		{ (void*)0x0000000140207FB0, { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 } },
		{ (void*)0x000000014064A400, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 } },
		// Disables call to glutFitWindowSizeToDesktop, prevents window automatic resize
		{ (void*)0x000000014018CE06, { 0x90, 0x90, 0x90, 0x90, 0x90 } },
		// Allow modifier mode selection (by Team Shimapan)
		{ (void*)0x00000001405A7C03, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		{ (void*)0x00000001405A6B45, { 0x90, 0x90 } },
		// allow modifier modes to work without use_card
		{ (void*)0x00000001405A7B9A,{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		{ (void*)0x000000014012F2CA,{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		// enable module selector without use_card
		// { (void*)0x00000001405C513B,{ 0x01 } }, // no
		// Force Hide IDs
		{ (void*)0x000000014096D840, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ (void*)0x000000014096D850, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		// fix TouchReaction
			// get some more space by optimizing this code for size
			/*{ (void*)0x000000014067D1F2,{ 0x7E } },                                            // MOVQ  XMM0,qword ptr [0x168 + RSP] (change to MOVQ)
			{ (void*)(0x000000014067D1F2 + 0x7),{ 0x66, 0x0F, 0xD6, 0x44, 0x24, 0x6C } },              // MOVQ  qword ptr [RSP + 0x6c],XMM0
			{ (void*)(0x000000014067D1F2 + 0xD),{ 0xC7, 0x44, 0x24, 0x74, 0x00, 0x00, 0x00, 0x00 } },  // MOV  dword ptr [RSP + 0x74],0x0
			{ (void*)(0x000000014067D1F2 + 0x15),{ 0xEB, 0x0E } },                                      // JMP  0x1406a2007 (to rest of function as usual) // unsure
																							   // add new code
			{ (void*)(0x000000014067D1F2 + 0x17),{ 0x66, 0x48, 0x0F, 0x6E, 0xC2 } },                    // MOVQ  XMM0,RDX (load touch pos)
			{ (void*)(0x000000014067D1F2 + 0x1C),{ 0xEB, 0x5D } },                                      // JMP  0x1406a205d // unsure
			{ (void*)(0x000000014067D1F2 + 0x7B),{ 0x0F, 0x2A, 0x0D, 0xB8, 0x6A, 0x31, 0x00 } },        // CVTPI2PS  XMM1,qword ptr [0x1409b8b1c] (load 1280x720) // unsuew
			{ (void*)(0x000000014067D1F2 + 0x82),{ 0x0F, 0x12, 0x51, 0x1C } },                          // MOVLPS  XMM2,qword ptr [RCX + 0x1c] (load actual res)
			{ (void*)(0x000000014067D1F2 + 0x86),{ 0xE9, 0x14, 0xFF, 0xFF, 0xFF } },                    // JMP  0x1406a1f81 // unsure
			{ (void*)(0x000000014067D1F2 - 0x61),{ 0x0F, 0x59, 0xC1 } },                                // MULPS  XMM0,XMM1
			{ (void*)(0x000000014067D1F2 - 0x61),{ 0x0F, 0x5E, 0xC2 } },                                // DIVPS  XMM0,XMM2
			{ (void*)(0x000000014067D1F2 - 0x5B),{ 0x66, 0x0F, 0xD6, 0x44, 0x24, 0x10 } },              // MOVQ  qword ptr [RSP+0x10],XMM0
			{ (void*)(0x000000014067D1F2 - 0x55),{ 0xEB, 0x06 } },                                      // JMP  0x1406a1f95 (back to original function) // unsure
																							   // jmp to new code
			{ (void*)(0x000000014067D1F2 - 0x52),{ 0xEB, 0x67 } },*/                                      // JMP  0x1406a1ff9 // unsure
	};
	const struct { void* Address; std::vector<uint8_t> Data; } patches_710[] =
	{
		// Just completely ignore all SYSTEM_STARTUP errors
		{ (void*)0x00000001403F5080, { 0xC3 } },
		// Always exit TASK_MODE_APP_ERROR on the first frame
		{ (void*)0x00000001403F73A7, { 0x90, 0x90 } },
		{ (void*)0x00000001403F73C3, { 0x89, 0xD1, 0x90 } },
		// Ignore the EngineClear variable to clear the framebuffer at all resolutions
		{ (void*)0x0000000140501480, { 0x90, 0x90 } },
		{ (void*)0x0000000140501515, { 0x90, 0x90 } },
		// Write ram files to the current directory instead of Y : / SBZV / ram
		{ (void*)0x000000014066CF09, { 0xE9, 0xD8, 0x00 } },
		// Change mdata path from "C:/Mount/Option" to "mdata/"
		{ (void*)0x0000000140A8CA18, { 0x6D, 0x64, 0x61, 0x74, 0x61, 0x2F, 0x00 } },
		{ (void*)0x000000014066CEAE, { 0x06 } },
		// Skip parts of the network check state
		{ (void*)0x00000001406717B1, { 0xE9, 0x22, 0x03, 0x00 } },
		// Set the initial DHCP WAIT timer value to 0
		{ (void*)0x00000001406724E7, { 0x00, 0x00 } },
		// Ignore SYSTEM_STARTUP Location Server checks
		{ (void*)0x00000001406732A2, { 0x90, 0x90 } },
		// Toon Shader Fix by lybxlpsv
		{ (void*)0x000000014050214F, { 0x90 } },
		{ (void*)0x0000000140502150, { 0x90 } },
		// Toon Shader Outline Fix by lybxlpsv
		{ (void*)0x0000000140641102, { 0x01 } },
		// Skip unnecessary checks
		{ (void*)0x0000000140210820, { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 } },
		{ (void*)0x000000014066E820, { 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3 } },
		// Disables call to glutFitWindowSizeToDesktop, prevents window automatic resize
		{ (void*)0x0000000140194E06, { 0x90, 0x90, 0x90, 0x90, 0x90 } },
		// Allow modifier mode selection (by Team Shimapan)
		{ (void*)0x00000001405CB1B3, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		{ (void*)0x00000001405CA0F5, { 0x90, 0x90 } },
		// allow modifier modes to work without use_card
		{ (void*)0x00000001405CB14A,{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		{ (void*)0x0000000140136CFA,{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } },
		// enable module selector without use_card
		{ (void*)0x00000001405C513B,{ 0x01 } },
		// fix back button
		{ (void*)0x0000000140578FB8, { 0xE9, 0x73, 0xFF, 0xFF, 0xFF } },
		// Force Hide IDs
		{ (void*)0x00000001409A5918, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ (void*)0x00000001409A5928, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		// fix TouchReaction
			// get some more space by optimizing this code for size
			{ (void*)0x00000001406A1FE2,{ 0x7E } },                                            // MOVQ  XMM0,qword ptr [0x168 + RSP] (change to MOVQ)
			{ (void*)0x00000001406A1FE9,{ 0x66, 0x0F, 0xD6, 0x44, 0x24, 0x6C } },              // MOVQ  qword ptr [RSP + 0x6c],XMM0
			{ (void*)0x00000001406A1FEF,{ 0xC7, 0x44, 0x24, 0x74, 0x00, 0x00, 0x00, 0x00 } },  // MOV  dword ptr [RSP + 0x74],0x0
			{ (void*)0x00000001406A1FF7,{ 0xEB, 0x0E } },                                      // JMP  0x1406a2007 (to rest of function as usual)
																							   // add new code
			{ (void*)0x00000001406A1FF9,{ 0x66, 0x48, 0x0F, 0x6E, 0xC2 } },                    // MOVQ  XMM0,RDX (load touch pos)
			{ (void*)0x00000001406A1FFE,{ 0xEB, 0x5D } },                                      // JMP  0x1406a205d
			{ (void*)0x00000001406A205D,{ 0x0F, 0x2A, 0x0D, 0xB8, 0x6A, 0x31, 0x00 } },        // CVTPI2PS  XMM1,qword ptr [0x1409b8b1c] (load 1280x720)
			{ (void*)0x00000001406A2064,{ 0x0F, 0x12, 0x51, 0x1C } },                          // MOVLPS  XMM2,qword ptr [RCX + 0x1c] (load actual res)
			{ (void*)0x00000001406A2068,{ 0xE9, 0x14, 0xFF, 0xFF, 0xFF } },                    // JMP  0x1406a1f81
			{ (void*)0x00000001406A1F81,{ 0x0F, 0x59, 0xC1 } },                                // MULPS  XMM0,XMM1
			{ (void*)0x00000001406A1F84,{ 0x0F, 0x5E, 0xC2 } },                                // DIVPS  XMM0,XMM2
			{ (void*)0x00000001406A1F87,{ 0x66, 0x0F, 0xD6, 0x44, 0x24, 0x10 } },              // MOVQ  qword ptr [RSP+0x10],XMM0
			{ (void*)0x00000001406A1F8D,{ 0xEB, 0x06 } },                                      // JMP  0x1406a1f95 (back to original function)
																							   // jmp to new code
			{ (void*)0x00000001406A1F90,{ 0xEB, 0x67 } },                                      // JMP  0x1406a1ff9
	};

	auto nCursor = GetPrivateProfileIntW(L"patches", L"cursor", TRUE, CONFIG_FILE);
	auto nHideFreeplay = GetPrivateProfileIntW(L"patches", L"hide_freeplay", FALSE, CONFIG_FILE);
	auto nFreeplay = GetPrivateProfileIntW(L"patches", L"freeplay", TRUE, CONFIG_FILE);
	auto nPDLoaderText = GetPrivateProfileIntW(L"patches", L"pdloadertext", TRUE, CONFIG_FILE);
	auto nStatusIcons = GetPrivateProfileIntW(L"patches", L"status_icons", 0, CONFIG_FILE);
	auto nHidePVWatermark = GetPrivateProfileIntW(L"patches", L"hide_pv_watermark", FALSE, CONFIG_FILE);
	auto nNoPVUi = GetPrivateProfileIntW(L"patches", L"no_pv_ui", FALSE, CONFIG_FILE);
	auto nHideVolCtrl = GetPrivateProfileIntW(L"patches", L"hide_volume", FALSE, CONFIG_FILE);
	auto nNoLyrics = GetPrivateProfileIntW(L"patches", L"no_lyrics", FALSE, CONFIG_FILE);
	auto nNoMovies = GetPrivateProfileIntW(L"patches", L"no_movies", FALSE, CONFIG_FILE);
	auto nMP4Movies = GetPrivateProfileIntW(L"patches", L"mp4_movies", FALSE, CONFIG_FILE);
	auto nNoError = GetPrivateProfileIntW(L"patches", L"no_error", FALSE, CONFIG_FILE);
	auto nNoTimer = GetPrivateProfileIntW(L"patches", L"no_timer", TRUE, CONFIG_FILE);
	auto nNoTimerSprite = GetPrivateProfileIntW(L"patches", L"no_timer_sprite", TRUE, CONFIG_FILE);
	auto nEStageManager = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager", 0, CONFIG_FILE);
	auto nEStageManagerEncore = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager_encore", TRUE, CONFIG_FILE);
	auto nUnlockPseudo = GetPrivateProfileIntW(L"patches", L"unlock_pseudo", FALSE, CONFIG_FILE);
	auto nCard = GetPrivateProfileIntW(L"patches", L"card", TRUE, CONFIG_FILE);
	auto nHardwareSlider = GetPrivateProfileIntW(L"patches", L"hardware_slider", FALSE, CONFIG_FILE);
	auto nOGLPatchA = GetPrivateProfileIntW(L"patches", L"opengl_patch_a", FALSE, CONFIG_FILE);
	auto nOGLPatchB = GetPrivateProfileIntW(L"patches", L"opengl_patch_b", FALSE, CONFIG_FILE);
	auto nTAA = GetPrivateProfileIntW(L"graphics", L"taa", TRUE, CONFIG_FILE);
	auto nMLAA = GetPrivateProfileIntW(L"graphics", L"mlaa", TRUE, CONFIG_FILE);
	auto nStereo = GetPrivateProfileIntW(L"patches", L"stereo", TRUE, CONFIG_FILE);
	auto nCustomPatches = GetPrivateProfileIntW(L"patches", L"custom_patches", TRUE, CONFIG_FILE);
	auto nQuickStart = GetPrivateProfileIntW(L"patches", L"quick_start", 1, CONFIG_FILE);
	auto nNoScrollingSfx = GetPrivateProfileIntW(L"patches", L"no_scrolling_sfx", FALSE, CONFIG_FILE);
	auto nNoHandScaling = GetPrivateProfileIntW(L"patches", L"no_hand_scaling", FALSE, CONFIG_FILE);
	auto nDefaultHandSize = GetPrivateProfileIntW(L"patches", L"default_hand_size", -1, CONFIG_FILE);
	auto nForceMouth = GetPrivateProfileIntW(L"patches", L"force_mouth", 0, CONFIG_FILE);
	auto nForceExpressions = GetPrivateProfileIntW(L"patches", L"force_expressions", 0, CONFIG_FILE);
	auto nForceLook = GetPrivateProfileIntW(L"patches", L"force_look", 0, CONFIG_FILE);
	auto nNpr1 = GetPrivateProfileIntW(L"graphics", L"npr1", 0, CONFIG_FILE);

	std::string version_string = std::to_string(game_version);
	version_string.insert(version_string.begin()+1, '.');
	std::cout << "[Patches] Game version " + version_string << std::endl;

	switch (game_version)
	{
	case 600:
		for (size_t i = 0; i < _countof(patches_600); i++)
			InjectCode(patches_600[i].Address, patches_600[i].Data);
		
		// The old stereo patch...
		// Use 2 channels instead of 4
		if (nStereo)
		{
			InjectCode((void*)0x0000000140A4A588, { 0x02 });
			printf("[Patches] Stereo patch enabled\n");
		}

		// Disable AA
		if (!nTAA)
		{
			// set TAA var (shouldn't be needed but whatever)
			//*(byte*)0x00000001411AB67C = 0; // no

			// make constructor/init not set TAA
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x000000014049258D, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x000000014049258D + 0) = 0x90;
			*((byte*)0x000000014049258D + 1) = 0x90;
			*((byte*)0x000000014049258D + 2) = 0x90;
			VirtualProtect((BYTE*)0x000000014049258D, 3, oldProtect, &bck); }

			// not sure, but it's somewhere in TaskPvGame init
			// just make it set TAA to 0 instead of 1 to avoid possible issues
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001400FF48D, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001400FF48D + 0) = 0x00;
			VirtualProtect((BYTE*)0x00000001400FF48D, 1, oldProtect, &bck); }

			// prevent re-enabling after taking photos
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x0000000140476EC8, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x0000000140476EC8 + 0) = 0x00;
			VirtualProtect((BYTE*)0x0000000140476EC8, 1, oldProtect, &bck); }

			printf("[Patches] TAA disabled\n");
		}
		if (!nMLAA)
		{
			// set MLAA var (shouldn't be needed but whatever)
			//*(byte*)0x00000001411AB680 = 0; // no

			// make constructor/init not set MLAA
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x000000014049258A, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x000000014049258A + 0) = 0x90;
			*((byte*)0x000000014049258A + 1) = 0x90;
			*((byte*)0x000000014049258A + 2) = 0x90;
			VirtualProtect((BYTE*)0x000000014049258A, 3, oldProtect, &bck); }

			printf("[Patches] MLAA disabled\n");
		}

		// Replace the hardcoded videos with MP4s, if they exist
		if (nMP4Movies)
		{
			patchMovieExt("adv_cfm_cm", (void*)0x0000000140989ED5);
			patchMovieExt("adv_sega_cm", (void*)0x0000000140989EF6);
			patchMovieExt("diva_adv02", (void*)0x00000001409C537D);
			patchMovieExt("diva_adv", (void*)0x00000001409C53AB);
		}
		// Hide "FREE PLAY"
		if (nHideFreeplay)
		{
			InjectCode((void*)0x0000000140388A5B, { 0x06, 0xB6 });
			printf("[Patches] Hide FREE PLAY/CREDIT(S) enabled\n");
		}
		// Enable "FREE PLAY" mode
		if (nFreeplay || nHideFreeplay)
		{
			InjectCode((void*)0x00000001403A1B1A, { 0x75 });
			printf("[Patches] Show FREE PLAY instead of CREDIT(S)\n");

			if (nPDLoaderText && !nHideFreeplay)
			{
				InjectCode((void*)0x00000001409BC188, { 0x50, 0x44, 0x20, 0x4C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x20, 0x00 });
				printf("[Patches] Show PD Loader text\n");
			}
		}
		// Use GLUT_CURSOR_RIGHT_ARROW instead of GLUT_CURSOR_NONE
		if (nCursor)
		{
			/*InjectCode((void*)0x000000014018B44A, { 0x00 });
			InjectCode((void*)0x00000001402E7FE5, { 0xeb }); // Disable debug cursor // unsure
			printf("[Patches] Cursor enabled\n");*/
		}
		// Override status icon states to be invalid (hides them)
		if (nStatusIcons > 0)
		{
			std::vector<uint8_t> cardIcon = { 0xFD, 0x0A };
			std::vector<uint8_t> networkIcon = { 0x9E, 0x1E };

			if (nStatusIcons == 2) // 2 for error icons
			{
				cardIcon = { 0xFA, 0x0A };
				networkIcon = { 0x9F, 0x1E };
				printf("[Patches] Status icons set to error state\n");
			}
			else if (nStatusIcons == 3) // 3 for OK icons
			{
				cardIcon = { 0xFC, 0x0A };
				networkIcon = { 0xA0, 0x1E };
				printf("[Patches] Status icons set to OK state\n");
			}
			else if (nStatusIcons == 4) // 4 for partial OK icons
			{
				cardIcon = { 0xFB, 0x0A };
				networkIcon = { 0xA1, 0x1E };
				printf("[Patches] Status icons set to partial OK state\n");
			}
			else // 1 or invalid for hidden
			{
				cardIcon = { 0xFD, 0x0A };
				networkIcon = { 0x9E, 0x1E };
				printf("[Patches] Status icons hidden\n");
			}

			// card icon
			InjectCode((void*)0x00000001403A0C9E, cardIcon); // error state
			InjectCode((void*)0x00000001403A0CA3, cardIcon); // OK state

			// network icon
			InjectCode((void*)0x00000001403A107B, networkIcon); // error state
			InjectCode((void*)0x00000001403A1085, networkIcon); // OK state
			InjectCode((void*)0x00000001403A109B, networkIcon); // partial state

			InjectCode((void*)0x00000001403A10D5, { 0x48, 0xE9 }); // never show the error code for partial connection

			// I was going to use this with a string, but the assignment wasn't behaving well and making separate prints was easier than figuring it out
			// printf("[Patches] Status icons %s\n", iconType);
		}
		// Removes PV watermark
		if (nHidePVWatermark)
		{
			InjectCode((void*)0x00000001409D9798, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
			printf("[Patches] PV watermark hidden\n");
		}
		// Disable the PV screen photo UI
		if (nNoPVUi)
		{
			InjectCode((void*)0x0000000140476DB1, { 0xEB, 0x6F }); // skip button panel image (JMP	0x14048FB02) // unsure

			// patch minimum PV UI state to 1 instead of 0
			// hook check for lyrics enabled (UI state < 2) to change UI state 0 into 1
			// dump new code in the skipped button panel condition
			InjectCode((void*)0x0000000140476DB3, { 0xC7, 0x83, 0x58, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 }); // MOV	dword ptr [0x158 + RBX],0x1
			InjectCode((void*)0x0000000140476DBD, { 0xC6, 0x80, 0x3A, 0xD1, 0x02, 0x00, 0x01 }); // MOV	byte ptr [0x2d13a + RAX],0x1
			InjectCode((void*)0x0000000140476DC4, { 0xE9, 0x8B, 0xFB, 0xFF, 0xFF }); // JMP	0x14048F634 // unsure

			InjectCode((void*)0x000000014047694D, { 0xE9, 0x61, 0x04, 0x00, 0x00 }); // JMP	0x14048FA93 // unsure

			printf("[Patches] PV UI disabled\n");
		}
		// Don't show volume control
		if (nHideVolCtrl)
		{
			// skip SE button
			InjectCode((void*)0x000000014096CD28, { 0xC0, 0xD3 });

			// skip volume sliders button
			InjectCode((void*)0x0000000140A4A3D8, { 0xE0, 0x50 });

			printf("[Patches] Volume control hidden\n");
		}
		// Skip loading (and therefore displaying) song lyrics
		if (nNoLyrics)
		{
			InjectCode((void*)0x00000001404CE362, { 0x00, 0x00 });
			InjectCode((void*)0x00000001404CE29E, { 0x48, 0xE9 }); // ensure first iteration doesn't run // unsure
			printf("[Patches] Lyrics disabled\n");
		}
		// Skip loading (and therefore displaying) song movies
		if (nNoMovies)
		{
			InjectCode((void*)0x00000001404D1C6B, { 0x48, 0xE9 }); // unsure
			InjectCode((void*)0x00000001404D1B61, { 0x48, 0xE9 }); // unsure
			printf("[Patches] Movies disabled\n");
		}
		// Disable error banner
		if (nNoError)
		{
			// Disable Errors Banner
			InjectCode((void*)0x00000001403A0DCB, { 0x90, 0x90 });
			printf("[Patches] Errors Banner disabled\n");
		}
		// Disable timer
		if (nNoTimer)
		{
			// Freeze mode selection timer
			InjectCode((void*)0x00000001405456DE, { 0x90, 0x90, 0x90 });

			// Freeze PV selection timer
			InjectCode((void*)0x000000014059B21F, { 0x90, 0x90, 0x90, 0x90 });
		}
		// Disable timer sprite
		if (nNoTimerSprite)
		{
			InjectCode((void*)0x00000001409883A8, { 0x00 }); // time_loop
			InjectCode((void*)0x0000000140A01E20, { 0x00 }); // time_in
			InjectCode((void*)0x0000000140A01E28, { 0x00 }); // time_out
		}
		// Enhanced Stage Manager
		if (nEStageManager > 0)
		{
			// Replace the function that provides the number of stages and compact some of it
			InjectCode((void*)0x0000000140371E50, { 0x48, 0x8B, 0x88, 0x40, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x20, 0x48, 0x8B, 0xD0, 0x48, 0x8B, 0x88, 0x48, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x28, 0x8B, 0x88, 0x50, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x30, 0x8B, 0x88, 0x54, 0x01, 0x00, 0x00, 0x8B, 0x80, 0x58, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x38, 0x8B, 0x82, 0x5C, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x34, 0x89, 0x44, 0x24, 0x3C, 0x48, 0x8B, 0x82, 0x60, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x40, 0x48, 0x8B, 0x82, 0x68, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x48, 0x48, 0x8B, 0x82, 0x70, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x50, 0x8B, 0x82, 0x78, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x58, 0x84, 0xC0, 0x74, 0x2A, 0x48, 0x8B, 0x44, 0x24, 0x38, 0x48, 0xC1, 0xE8, 0x20, 0x85, 0xC0, 0x75, 0x1D, 0xE8, 0xD9, 0xD9, 0xE5, 0xFF, 0x48, 0x85, 0xC0, 0x74, 0x13, 0x48, 0x8D, 0x48, 0x10, 0xE8, 0xFB, 0xD3, 0xE5, 0xFF, 0xB9, 0x03, 0x00, 0x00, 0x00, 0x85, 0xC0, 0x0F, 0x45, 0xD9, 0x8B, 0x1D, 0x1B, 0x0C, 0x00, 0x00, 0x83, 0x3D, 0x1C, 0x0C, 0x00, 0x00, 0x00, 0x74, 0x06, 0x8B, 0x1D, 0x10, 0x0C, 0x00, 0x00, 0x8B, 0xC3, 0x48, 0x83, 0xC4, 0x60, 0x5B, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }); // unsure

			// Jump to another section by addding some code to replace the values (Jump 1)
			InjectCode((void*)(0x0000000140371E50+0x104), { 0xE9, 0x87, 0x0B, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }); // unsure

			// Jump to another section by addding some code to replace the values (Replace 1/2) while some reserving space for values
			InjectCode((void*)(0x0000000140371E50+0xC50), { 0xFF, 0x05, 0x76, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x70, 0x00, 0x00, 0x00, 0xBA, 0x02, 0x00, 0x00, 0x00, 0x83, 0x3D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x74, 0x02, 0xFF, 0xC2, 0x39, 0xD1, 0x0F, 0x4D, 0xCA, 0x89, 0x48, 0x08, 0xB9, 0x0E, 0x00, 0x00, 0x00, 0xE8, 0x92, 0x98, 0xE0, 0xFF, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x28, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x44, 0x89, 0x61, 0x08, 0x44, 0x88, 0x61, 0x0C, 0x4C, 0x89, 0x61, 0x10, 0x44, 0x89, 0x25, 0x29, 0x00, 0x00, 0x00, 0xE9, 0x68, 0xF4, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }); // unsure

			// Use the value from our own address instead of the original one
			InjectCode((void*)0x00000001403DDABF, { 0x48, 0x8D, 0x05, 0x4A, 0x18, 0xDA, 0x00, 0x8B, 0x1D, 0x00, 0x56 }); // unsure

			// Jump to another section by addding some code to replace the values (Jump 2)
			InjectCode((void*)(0x00000001403F65AF+0x89), { 0xE9, 0x03, 0x55, 0xF9, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }); // unsure

			DWORD StageCountProtect;
			VirtualProtect((int32_t*)0x140372B10, 0x10, PAGE_EXECUTE_READWRITE, &StageCountProtect); // unsure

			int* ESM = (int*)0x0000000140372B10; // unsure

			if (nEStageManagerEncore)
				ESM[1] = nEStageManager;
			else
				ESM[0] = nEStageManager;

			ESM[2] = nEStageManagerEncore;

			printf("[Patches] Enhanced Stage Manager enabled\n");
		}
		// OpenGL Patches
		if (nOGLPatchA)
		{
			// (call cs:glGetError) -> (xor eax, eax); (nop); (nop); (nop); (nop);
			InjectCode((void*)0x000000014067842D, { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 });
		}
		if (nOGLPatchB)
		{
			// (js loc_14069BC32) -> (nop)...
			InjectCode((void*)0x0000000140676D04, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
		}
		// Unlock PSEUDO modules (which will all default to Miku, unless we also patch them to match the first performer)
		if (nUnlockPseudo)
		{
			InjectCode((void*)0x00000001409E71C0, { 0x00 });
			InjectCode((void*)0x00000001409E71D0, { 0x00 });
			InjectCode((void*)0x00000001409E71E0, { 0x00 });
			InjectCode((void*)0x00000001409E71F0, { 0x00 });
			InjectCode((void*)0x00000001409E7200, { 0x00 });
		}
		// Enable card button by somewhatlurker (pretty much just eye candy for now)
		if (nCard)
		{
			InjectCode((void*)0x00000001405449EB, { 0x90, 0x90 });
		}
		// The original slider update needs to run for hardware sliders to work -- only patch it when using emulation
		if (!nHardwareSlider)
		{
			// Don't update the touch slider state so we can write our own
			InjectCode((void*)0x00000001405F0C5B, { 0x90, 0x90, 0x90, 0x8B, 0x42, 0xE0, 0x90, 0x90, 0x90 });
		}
		break;
	default:
		for (size_t i = 0; i < _countof(patches_710); i++)
			InjectCode(patches_710[i].Address, patches_710[i].Data);
		
		// The old stereo patch...
		// Use 2 channels instead of 4
		if (nStereo)
		{
			InjectCode((void*)0x0000000140A860C0, { 0x02 });
			printf("[Patches] Stereo patch enabled\n");
		}

		// Disable AA
		if (!nTAA)
		{
			// set TAA var (shouldn't be needed but whatever)
			*(byte*)0x00000001411AB67C = 0;

			// make constructor/init not set TAA
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001404AB11D, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001404AB11D + 0) = 0x90;
			*((byte*)0x00000001404AB11D + 1) = 0x90;
			*((byte*)0x00000001404AB11D + 2) = 0x90;
			VirtualProtect((BYTE*)0x00000001404AB11D, 3, oldProtect, &bck); }

			// not sure, but it's somewhere in TaskPvGame init
			// just make it set TAA to 0 instead of 1 to avoid possible issues
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001401063CE, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001401063CE + 0) = 0x00;
			VirtualProtect((BYTE*)0x00000001401063CE, 1, oldProtect, &bck); }

			// prevent re-enabling after taking photos
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x000000014048FBA9, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x000000014048FBA9 + 0) = 0x00;
			VirtualProtect((BYTE*)0x000000014048FBA9, 1, oldProtect, &bck); }

			printf("[Patches] TAA disabled\n");
		}
		if (!nMLAA)
		{
			// set MLAA var (shouldn't be needed but whatever)
			*(byte*)0x00000001411AB680 = 0;

			// make constructor/init not set MLAA
			{DWORD oldProtect, bck;
			VirtualProtect((BYTE*)0x00000001404AB11A, 3, PAGE_EXECUTE_READWRITE, &oldProtect);
			*((byte*)0x00000001404AB11A + 0) = 0x90;
			*((byte*)0x00000001404AB11A + 1) = 0x90;
			*((byte*)0x00000001404AB11A + 2) = 0x90;
			VirtualProtect((BYTE*)0x00000001404AB11A, 3, oldProtect, &bck); }

			printf("[Patches] MLAA disabled\n");
		}

		// Replace the hardcoded videos with MP4s, if they exist
		if (nMP4Movies)
		{
			patchMovieExt("adv_cm_01", (void*)0x00000001409A53CC);
			patchMovieExt("adv_cm_02", (void*)0x00000001409A53E4);
			patchMovieExt("adv_cm_03", (void*)0x00000001409A53FC);
			patchMovieExt("adv_cfm_cm", (void*)0x00000001409C22CD);
			patchMovieExt("adv_sega_cm", (void*)0x00000001409C22EE);
			patchMovieExt("diva_adv02", (void*)0x00000001409FF455);
			patchMovieExt("diva_adv", (void*)0x00000001409FF483);
		}
		// Hide "FREE PLAY"
		if (nHideFreeplay)
		{
			InjectCode((void*)0x00000001403BABEF, { 0x06, 0xB6 });
			printf("[Patches] Hide FREE PLAY/CREDIT(S) enabled\n");
		}
		// Enable "FREE PLAY" mode
		if (nFreeplay || nHideFreeplay)
		{
			// Always return true for the SelCredit enter SelPv check
			InjectCode((void*)0x0000000140393610, { 0xB0, 0x01, 0xC3, 0x90, 0x90, 0x90 });

			InjectCode((void*)0x00000001403BABEA, { 0x75 });
			printf("[Patches] Show FREE PLAY instead of CREDIT(S)\n");

			if (nPDLoaderText && !nHideFreeplay)
			{
				InjectCode((void*)0x00000001409F61F0, { 0x50, 0x44, 0x20, 0x4C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x20, 0x00 });
				printf("[Patches] Show PD Loader text\n");
			}
		}
		// Use GLUT_CURSOR_RIGHT_ARROW instead of GLUT_CURSOR_NONE
		if (nCursor)
		{
			InjectCode((void*)0x000000014019341B, { 0x00 });
			InjectCode((void*)0x00000001403012b5, { 0xeb }); // Disable debug cursor
			printf("[Patches] Cursor enabled\n");
		}
		// Override status icon states to be invalid (hides them)
		if (nStatusIcons > 0)
		{
			std::vector<uint8_t> cardIcon = { 0xFD, 0x0A };
			std::vector<uint8_t> networkIcon = { 0x9E, 0x1E };

			if (nStatusIcons == 2) // 2 for error icons
			{
				cardIcon = { 0xFA, 0x0A };
				networkIcon = { 0x9F, 0x1E };
				printf("[Patches] Status icons set to error state\n");
			}
			else if (nStatusIcons == 3) // 3 for OK icons
			{
				cardIcon = { 0xFC, 0x0A };
				networkIcon = { 0xA0, 0x1E };
				printf("[Patches] Status icons set to OK state\n");
			}
			else if (nStatusIcons == 4) // 4 for partial OK icons
			{
				cardIcon = { 0xFB, 0x0A };
				networkIcon = { 0xA1, 0x1E };
				printf("[Patches] Status icons set to partial OK state\n");
			}
			else // 1 or invalid for hidden
			{
				cardIcon = { 0xFD, 0x0A };
				networkIcon = { 0x9E, 0x1E };
				printf("[Patches] Status icons hidden\n");
			}

			// card icon
			InjectCode((void*)0x00000001403B9D6E, cardIcon); // error state
			InjectCode((void*)0x00000001403B9D73, cardIcon); // OK state

			// network icon
			InjectCode((void*)0x00000001403BA14B, networkIcon); // error state
			InjectCode((void*)0x00000001403BA155, networkIcon); // OK state
			InjectCode((void*)0x00000001403BA16B, networkIcon); // partial state

			InjectCode((void*)0x00000001403BA1A5, { 0x48, 0xE9 }); // never show the error code for partial connection

			// I was going to use this with a string, but the assignment wasn't behaving well and making separate prints was easier than figuring it out
			// printf("[Patches] Status icons %s\n", iconType);
		}
		// Removes PV watermark
		if (nHidePVWatermark)
		{
			InjectCode((void*)0x0000000140A13A88, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
			printf("[Patches] PV watermark hidden\n");
		}
		// Disable the PV screen photo UI
		if (nNoPVUi)
		{
			InjectCode((void*)0x000000014048FA91, { 0xEB, 0x6F }); // skip button panel image (JMP	0x14048FB02)

			// patch minimum PV UI state to 1 instead of 0
			// hook check for lyrics enabled (UI state < 2) to change UI state 0 into 1
			// dump new code in the skipped button panel condition
			InjectCode((void*)0x000000014048FA93, { 0xC7, 0x83, 0x58, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 }); // MOV	dword ptr [0x158 + RBX],0x1
			InjectCode((void*)0x000000014048FA9D, { 0xC6, 0x80, 0x3A, 0xD1, 0x02, 0x00, 0x01 }); // MOV	byte ptr [0x2d13a + RAX],0x1
			InjectCode((void*)0x000000014048FAA4, { 0xE9, 0x8B, 0xFB, 0xFF, 0xFF }); // JMP	0x14048F634

			InjectCode((void*)0x000000014048F62D, { 0xE9, 0x61, 0x04, 0x00, 0x00 }); // JMP	0x14048FA93

			printf("[Patches] PV UI disabled\n");
		}
		// Don't show volume control
		if (nHideVolCtrl)
		{
			// skip SE button
			InjectCode((void*)0x00000001409A4D60, { 0xC0, 0xD3 });

			// skip volume sliders button
			InjectCode((void*)0x0000000140A85F10, { 0xE0, 0x50 });

			printf("[Patches] Volume control hidden\n");
		}
		// Skip loading (and therefore displaying) song lyrics
		if (nNoLyrics)
		{
			InjectCode((void*)0x00000001404E7A25, { 0x00, 0x00 });
			InjectCode((void*)0x00000001404E7950, { 0x48, 0xE9 }); // ensure first iteration doesn't run
			printf("[Patches] Lyrics disabled\n");
		}
		// Skip loading (and therefore displaying) song movies
		if (nNoMovies)
		{
			InjectCode((void*)0x00000001404EB584, { 0x48, 0xE9 });
			InjectCode((void*)0x00000001404EB471, { 0x48, 0xE9 });
			printf("[Patches] Movies disabled\n");
		}
		// Disable error banner
		if (nNoError)
		{
			// Disable Errors Banner
			InjectCode((void*)0x00000001403B9E9B, { 0x90, 0x90 });
			printf("[Patches] Errors Banner disabled\n");
		}
		// Disable timer
		if (nNoTimer)
		{
			// Freeze mode selection timer
			InjectCode((void*)0x0000000140566B9E, { 0x90, 0x90, 0x90 });

			// Freeze PV selection timer
			InjectCode((void*)0x00000001405BDFBF, { 0x90, 0x90, 0x90, 0x90 });
		}
		// Disable timer sprite
		if (nNoTimerSprite)
		{
			InjectCode((void*)0x00000001409C0758, { 0x00 }); // time_loop
			InjectCode((void*)0x0000000140A3D3F0, { 0x00 }); // time_in
			InjectCode((void*)0x0000000140A3D3F8, { 0x00 }); // time_out
		}
		// Enhanced Stage Manager
		if (nEStageManager > 0)
		{
			// Replace the function that provides the number of stages and compact some of it
			InjectCode((void*)0x000000014038AEF0, { 0x48, 0x8B, 0x88, 0x40, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x20, 0x48, 0x8B, 0xD0, 0x48, 0x8B, 0x88, 0x48, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x28, 0x8B, 0x88, 0x50, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x30, 0x8B, 0x88, 0x54, 0x01, 0x00, 0x00, 0x8B, 0x80, 0x58, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x38, 0x8B, 0x82, 0x5C, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x34, 0x89, 0x44, 0x24, 0x3C, 0x48, 0x8B, 0x82, 0x60, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x40, 0x48, 0x8B, 0x82, 0x68, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x48, 0x48, 0x8B, 0x82, 0x70, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x50, 0x8B, 0x82, 0x78, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x58, 0x84, 0xC0, 0x74, 0x2A, 0x48, 0x8B, 0x44, 0x24, 0x38, 0x48, 0xC1, 0xE8, 0x20, 0x85, 0xC0, 0x75, 0x1D, 0xE8, 0xD9, 0xD9, 0xE5, 0xFF, 0x48, 0x85, 0xC0, 0x74, 0x13, 0x48, 0x8D, 0x48, 0x10, 0xE8, 0xFB, 0xD3, 0xE5, 0xFF, 0xB9, 0x03, 0x00, 0x00, 0x00, 0x85, 0xC0, 0x0F, 0x45, 0xD9, 0x8B, 0x1D, 0x1B, 0x0C, 0x00, 0x00, 0x83, 0x3D, 0x1C, 0x0C, 0x00, 0x00, 0x00, 0x74, 0x06, 0x8B, 0x1D, 0x10, 0x0C, 0x00, 0x00, 0x8B, 0xC3, 0x48, 0x83, 0xC4, 0x60, 0x5B, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC });

			// Jump to another section by addding some code to replace the values (Jump 1)
			InjectCode((void*)0x000000014038AFF4, { 0xE9, 0x87, 0x0B, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC });

			// Jump to another section by addding some code to replace the values (Replace 1/2) while some reserving space for values
			InjectCode((void*)0x000000014038BB40, { 0xFF, 0x05, 0x76, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x70, 0x00, 0x00, 0x00, 0xBA, 0x02, 0x00, 0x00, 0x00, 0x83, 0x3D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x74, 0x02, 0xFF, 0xC2, 0x39, 0xD1, 0x0F, 0x4D, 0xCA, 0x89, 0x48, 0x08, 0xB9, 0x0E, 0x00, 0x00, 0x00, 0xE8, 0x92, 0x98, 0xE0, 0xFF, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x28, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x44, 0x89, 0x61, 0x08, 0x44, 0x88, 0x61, 0x0C, 0x4C, 0x89, 0x61, 0x10, 0x44, 0x89, 0x25, 0x29, 0x00, 0x00, 0x00, 0xE9, 0x68, 0xF4, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });

			// Use the value from our own address instead of the original one
			InjectCode((void*)0x00000001403F65AF, { 0x48, 0x8D, 0x05, 0x4A, 0x18, 0xDA, 0x00, 0x8B, 0x1D, 0x00, 0x56 });

			// Jump to another section by addding some code to replace the values (Jump 2)
			InjectCode((void*)0x00000001403F6638, { 0xE9, 0x03, 0x55, 0xF9, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC });

			DWORD StageCountProtect;
			VirtualProtect((int32_t*)0x14038BBB0, 0x10, PAGE_EXECUTE_READWRITE, &StageCountProtect);

			int* ESM = (int*)0x000000014038BBB0;

			if (nEStageManagerEncore)
				ESM[1] = nEStageManager;
			else
				ESM[0] = nEStageManager;

			ESM[2] = nEStageManagerEncore;

			printf("[Patches] Enhanced Stage Manager enabled\n");
		}
		// OpenGL Patches
		if (nOGLPatchA)
		{
			// (call cs:glGetError) -> (xor eax, eax); (nop); (nop); (nop); (nop);
			InjectCode((void*)0x000000014069D21D, { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 });
		}
		if (nOGLPatchB)
		{
			// (js loc_14069BC32) -> (nop)...
			InjectCode((void*)0x000000014069BAF4, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
		}
		// Unlock PSEUDO modules (which will all default to Miku, unless we also patch them to match the first performer)
		if (nUnlockPseudo)
		{
			InjectCode((void*)0x0000000140A21770, { 0x00 });
			InjectCode((void*)0x0000000140A21780, { 0x00 });
			InjectCode((void*)0x0000000140A21790, { 0x00 });
			InjectCode((void*)0x0000000140A217A0, { 0x00 });
			InjectCode((void*)0x0000000140A217B0, { 0x00 });
		}
		// Enable card button by somewhatlurker (pretty much just eye candy for now)
		if (nCard)
		{
			InjectCode((void*)0x0000000140565E6B, { 0x90, 0x90 });
		}
		// The original slider update needs to run for hardware sliders to work -- only patch it when using emulation
		if (!nHardwareSlider)
		{
			// Don't update the touch slider state so we can write our own
			InjectCode((void*)0x000000014061579B, { 0x90, 0x90, 0x90, 0x8B, 0x42, 0xE0, 0x90, 0x90, 0x90 });
		}
		// Quick start
		{
			if (nQuickStart == 1) // skip the card/guest screen
			{
				InjectCode((void*)0x0000000140578EA9, { 0xE9, 0x8E, 0x00, 0x00, 0x00 });
			}
			else if (nQuickStart == 2) // skip everything; normal mode
			{
				InjectCode((void*)0x0000000140578EA9, { 0xE9, 0xF1, 0x00, 0x00, 0x00 });
				InjectCode((void*)0x0000000140578E9D, { 0xC7, 0x47, 0x68, 0x28, 0x00, 0x00, 0x00 }); // skip back button error
			}
		}
		// Disable scrolling sound effect
		if (nNoScrollingSfx)
		{
			InjectCode((void*)0x00000001405C84B3, { 0x90, 0x90, 0x90, 0x90, 0x90 });
		}
		// Disable hand scaling
		if (nNoHandScaling)
		{
			InjectCode((void*)0x0000000140120709, { 0xE9, 0x82, 0x0A, 0x00 });
		}
		// Default hand size
		if (nDefaultHandSize != -1)
		{
			printf("[Patches] Changing default hand size...\n");
			const float num = (float)nDefaultHandSize / 10000.0;
			DWORD oldProtect;
			float* addr1 = (float*)(0x140506B59);
			float* addr2 = (float*)(0x140506B60);
			/*float* addr3 = (float*)(0x140506B67);
			float* addr4 = (float*)(0x140506B71);*/
			VirtualProtect(addr1, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
			VirtualProtect(addr2, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
			/*VirtualProtect(addr3, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
			VirtualProtect(addr4, 4, PAGE_EXECUTE_READWRITE, &oldProtect);*/
			*addr1 = *addr2 /*= *addr3 = *addr4*/ = num;
			VirtualProtect(addr1, 4, oldProtect, nullptr);
			VirtualProtect(addr2, 4, oldProtect, nullptr);
			/*VirtualProtect(addr3, 4, oldProtect, nullptr);
			VirtualProtect(addr4, 4, oldProtect, nullptr);*/
			printf("[Patches] New default hand size: %f\n", num);
		}
		// Force mouth animations
		{
			if (nForceMouth == 1) // PDA
			{
				printf("[Patches] Forcing PDA mouth...\n");
				int* mouth_table = (int*)(0x1409A1DC0);
				DWORD oldProtect;
				VirtualProtect(mouth_table, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 11; i++)
				{
					mouth_table[i]++;
				}
				VirtualProtect(mouth_table, 44, oldProtect, nullptr);
				printf("[Patches] PDA mouth forced\n");
			}
			else if (nForceMouth == 2) // FT
			{
				printf("[Patches] Forcing FT mouth...\n");
				int* mouth_table_oldid = (int*)(0x1409A1E1C);
				DWORD oldProtect;
				VirtualProtect(mouth_table_oldid, 44, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 11; i++)
				{
					mouth_table_oldid[i]--;
				}
				VirtualProtect(mouth_table_oldid, 44, oldProtect, nullptr);
				printf("[Patches] FT mouth forced\n");
			}
		}
		// Force expressions
		{
			if (nForceExpressions == 1) // PDA
			{
				printf("[Patches] Forcing PDA expressions...\n");
				int* exp_table = (int*)(0x140A21900);
				DWORD oldProtect;
				VirtualProtect(exp_table, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 26; i++)
				{
					exp_table[i] += 2;
				}
				VirtualProtect(exp_table, 104, oldProtect, nullptr);
				printf("[Patches] PDA expressions forced\n");
			}
			else if (nForceExpressions == 2) // FT
			{
				printf("[Patches] Forcing FT expressions...\n");
				int* exp_table_oldid = (int*)(0x140A219D0);
				DWORD oldProtect;
				VirtualProtect(exp_table_oldid, 104, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 26; i++)
				{
					exp_table_oldid[i] -= 2;
				}
				VirtualProtect(exp_table_oldid, 104, oldProtect, nullptr);
				printf("[Patches] FT expressions forced\n");
			}
		}
		// Force look animations
		{
			if (nForceLook == 1) // PDA
			{
				printf("[Patches] Forcing PDA look...\n");
				int* look_table = (int*)(0x1409A1D70);
				DWORD oldProtect;
				VirtualProtect(look_table, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 9; i++)
				{
					look_table[i]++;
				}
				VirtualProtect(look_table, 36, oldProtect, nullptr);
				printf("[Patches] PDA look forced\n");
			}
			else if (nForceLook == 2) // FT
			{
				printf("[Patches] Forcing FT look...\n");
				int* look_table_oldid = (int*)(0x1409A1D9C);
				DWORD oldProtect;
				VirtualProtect(look_table_oldid, 36, PAGE_EXECUTE_READWRITE, &oldProtect);
				for (int i = 0; i < 11; i++)
				{
					look_table_oldid[i]--;
				}
				VirtualProtect(look_table_oldid, 36, oldProtect, nullptr);
				printf("[Patches] FT look forced\n");
			}
		}
		// NPR1
		{
			if (nNpr1 == 1) // force on
			{
				InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
				InjectCode((void*)0x0000000140502FC6, { 0x01, 0x00, 0x00, 0x00, 0xC3 });

				printf("[Patches] NPR1 forced\n");
			}
			if (nNpr1 == 2) // force off
			{
				InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
				InjectCode((void*)0x0000000140502FC6, { 0x00, 0x00, 0x00, 0x00, 0xC3 });

				printf("[Patches] NPR1 disabled\n");
			}
		}
	}



	printf("[Patches] Patches loaded\n");

	if (nCustomPatches)
	{
		std::cout << "[Patches] Reading custom patches...\n";
		try {
			for (std::filesystem::path p : std::filesystem::directory_iterator("../patches"))
			{
				std::string extension = std::filesystem::path(p).extension().string();
				if ((extension == ".p" || extension == ".P" || extension == ".p2" || extension == ".P2") &&
					GetPrivateProfileIntW(L"plugins", std::filesystem::path(p).filename().c_str(), TRUE, CONFIG_FILE))
				{
					std::cout << "[Patches] Reading custom patch file: " << std::filesystem::path(p).filename().string() << std::endl;
					ApplyCustomPatches(std::filesystem::path(p).wstring());
				}
			}
		}
		catch (const std::filesystem::filesystem_error &e) {
			std::cout << "[Patches] File system error " << e.what() << " " << e.path1() << " " << e.path2() << " " << e.code() << std::endl;
		}

		std::cout << "[Patches] All custom patches applied\n";
	}
}

void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}

void ApplyCustomPatches(std::wstring CPATCH_FILE_STRING)
{
	LPCWSTR CPATCH_FILE = CPATCH_FILE_STRING.c_str();
	std::ifstream fileStream(CPATCH_FILE_STRING);

	if (!fileStream.is_open())
		return;

	std::string line;

	// check for BOM
	std::getline(fileStream, line);
	if (line.size() >= 3 && line.rfind("\xEF\xBB\xBF", 0) == 0)
		fileStream.seekg(3);
	else
		fileStream.seekg(0);

	while (std::getline(fileStream, line))
	{
		if (line[0] == '#')
		{
			std::cout << "[Patches]" << line.substr(1, line.size()-1) << std::endl;
			continue;
		}
		if (line == "IGNORE") break;
		if (line.find(':') == std::string::npos || (line[0] == '/' && line[1] == '/')) continue;

		std::vector<std::string> commentHSplit = SplitString(line, "#");
		std::vector<std::string> commentDSSplit = SplitString(commentHSplit[0], "//");
		std::vector<std::string> colonSplit = SplitString(commentDSSplit[0], ":");
		if (colonSplit.size() != 2) continue;
		bool echo = true;
		if (colonSplit[0].at(0) == '@')
		{
			echo = false;
			colonSplit[0].at(0) = ' ';
		}
		long long int address;
		std::istringstream iss(colonSplit[0]);
		iss >> std::setbase(16) >> address;
		if (address == 0) std::cout << "[Patches] Custom patch address wrong: " << std::hex << address << std::endl;

		if (colonSplit[1].at(0) == '!')
		{
			if (echo) std::cout << "[Patches] Patching: " << std::hex << address << ":!";
			std::vector<std::string> fullColonSplit = SplitString(line, ":");
			for (int i = 1; i < fullColonSplit[1].size(); i++)
			{
				unsigned char byte_u = fullColonSplit[1].at(i);
				if(byte_u=='\\' && i<fullColonSplit[1].size())
				{
					switch (fullColonSplit[1].at(i + 1))
					{
					case '0':
						byte_u = '\0';
						i++;
						break;
					case 'n':
						byte_u = '\n';
						i++;
						break;
					case 't':
						byte_u = '\t';
						i++;
						break;
					case 'r':
						byte_u = '\r';
						i++;
						break;
					case 'b':
						byte_u = '\b';
						i++;
						break;
					case 'a':
						byte_u = '\a';
						i++;
						break;
					case 'f':
						byte_u = '\f';
						i++;
						break;
					case 'v':
						byte_u = '\v';
						i++;
						break;
					case '\\':
						byte_u = '\\';
						i++;
						break;
					}
				}
				if (echo) std::cout << byte_u;
				std::vector<uint8_t> patch = { byte_u };
				InjectCode((void*)address, patch);
				address++;
			}
			if (echo) std::cout << std::endl;
		}
		else
		{
			std::vector<std::string> bytes = SplitString(colonSplit[1], " ");
			if (bytes.size() < 1) continue;

			std::string comment_string = "";
			int comment_counter = 0;
			if (commentHSplit.size() > 1)
			{
				bool ignore = 1;
				for (std::string comment : commentHSplit)
				{
					if (ignore)
					{
						ignore = 0;
						continue;
					}
					comment_string = comment_string + comment;
				}
			}

			if (echo) std::cout << "[Patches] Patching: " << std::hex << address << ":";
			for (std::string bytestring : bytes)
			{
				int byte;
				std::istringstream issb(bytestring);
				issb >> std::setbase(16) >> byte;
				unsigned char byte_u = byte;
				if (echo)
				{
					std::cout << std::hex << byte << " ";
					if (comment_counter < comment_string.length())
					{
						std::cout << "(" << comment_string.at(comment_counter) << ") ";
						comment_counter++;
					}
				}
				std::vector<uint8_t> patch = { byte_u };
				InjectCode((void*)address, patch);
				address++;
			}
			if (echo) std::cout << std::endl;
			else if (comment_string.length() > 0)
			{
				std::cout << "[Patches]";;
				if (comment_string.at(0) != ' ') std::cout << ' ';
				std::cout << comment_string << std::endl;
			}
		}
	}

	fileStream.close();
}


using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Patches";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Applies patches/tweaks to the game before it starts.\nThis plugin is required.";
}