#include "windows.h"
#include "vector"
#include <tchar.h>
#include <GL/freeglut.h>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>

void InjectCode(void* address, const std::vector<uint8_t> data);
void ApplyPatches();

const LPCWSTR CONFIG_FILE = L".\\config.ini";

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
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
	for (int i = 0; i < 1000; i++)
	{
		std::stringstream ss;
		ss << std::setw(3) << std::setfill('0') << i;
		if (std::filesystem::exists("../mdata/M" + ss.str() + "/rom/movie/" + moviefile + ".mp4"))
		{
			isinmdata = 1;
			//std::cout << "Movie " << moviefile << ".mp4 found in M" << ss.str() << std::endl;
			break;
		}
	}
	if (isinmdata || std::filesystem::exists("../rom/movie/" + moviefile + ".mp4"))
	{
		InjectCode(address, { 0x6D, 0x70, 0x34 });
		std::cout << "Movie " << moviefile << " patched to mp4\n";
		return;
	}
	//std::cout << "Movie " << moviefile << " NOT patched to mp4\n";
	return;
}

void ApplyPatches() {

	const struct { void* Address; std::vector<uint8_t> Data; } patches[] =
	{
		// Always return true for the SelCredit enter SelPv check
		{ (void*)0x0000000140393610, { 0xB0, 0x01, 0xC3, 0x90, 0x90, 0x90 } },
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
		{ (void*)0x00000001405C513B, { 0x01 } },
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
			{ (void*)0x00000001406A1FF9,{ 0x66, 0x48, 0x0F, 0x6E, 0xC2 } },              // MOVQ  XMM0,RDX (load touch pos)
			{ (void*)0x00000001406A1FFE,{ 0xEB, 0x5D } },                                // JMP  0x1406a205d
			{ (void*)0x00000001406A205D,{ 0x0F, 0x2A, 0x0D, 0xB8, 0x6A, 0x31, 0x00 } },  // CVTPI2PS  XMM1,qword ptr [0x1409b8b1c] (load 1280x720)
			{ (void*)0x00000001406A2064,{ 0x0F, 0x12, 0x51, 0x1C } },                    // MOVLPS  XMM2,qword ptr [RCX + 0x1c] (load actual res)
			{ (void*)0x00000001406A2068,{ 0xE9, 0x14, 0xFF, 0xFF, 0xFF } },              // JMP  0x1406a1f81
			{ (void*)0x00000001406A1F81,{ 0x0F, 0x59, 0xC1 } },                          // MULPS  XMM0,XMM1
			{ (void*)0x00000001406A1F84,{ 0x0F, 0x5E, 0xC2 } },                          // DIVPS  XMM0,XMM2
			{ (void*)0x00000001406A1F87,{ 0x66, 0x0F, 0xD6, 0x44, 0x24, 0x10 } },        // MOVQ  qword ptr [RSP+0x10],XMM0
			{ (void*)0x00000001406A1F8D,{ 0xEB, 0x06 } },                                // JMP  0x1406a1f95 (back to original function)
																						 // jmp to new code
			{ (void*)0x00000001406A1F90,{ 0xEB, 0x67 } },  // JMP  0x1406a1ff9
	};
	printf("[Patches] Patches loaded\n");

	for (size_t i = 0; i < _countof(patches); i++)
		InjectCode(patches[i].Address, patches[i].Data);

	auto nCursor = GetPrivateProfileIntW(L"patches", L"cursor", TRUE, CONFIG_FILE);
	auto nHideFreeplay = GetPrivateProfileIntW(L"patches", L"hide_freeplay", FALSE, CONFIG_FILE);
	auto nFreeplay = GetPrivateProfileIntW(L"patches", L"freeplay", TRUE, CONFIG_FILE);
	auto nPDLoaderText = GetPrivateProfileIntW(L"patches", L"pdloader_text", TRUE, CONFIG_FILE);
	auto nStatusIcons = GetPrivateProfileIntW(L"patches", L"status_icons", 0, CONFIG_FILE);
	auto nHidePVWatermark = GetPrivateProfileIntW(L"patches", L"hide_pv_watermark", FALSE, CONFIG_FILE);
	auto nNoPVUi = GetPrivateProfileIntW(L"patches", L"no_pv_ui", FALSE, CONFIG_FILE);
	auto nHideVolCtrl = GetPrivateProfileIntW(L"patches", L"hide_volume", FALSE, CONFIG_FILE);
	auto nNoLyrics = GetPrivateProfileIntW(L"patches", L"no_lyrics", FALSE, CONFIG_FILE);
	auto nNoMovies = GetPrivateProfileIntW(L"patches", L"no_movies", FALSE, CONFIG_FILE);
	auto nMP4Movies = GetPrivateProfileIntW(L"patches", L"mp4_movies", FALSE, CONFIG_FILE);
	auto nNoError = GetPrivateProfileIntW(L"patches", L"no_error", FALSE, CONFIG_FILE);
	auto nEStageManager = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager", 0, CONFIG_FILE);
	auto nEStageManagerEncore = GetPrivateProfileIntW(L"patches", L"enhanced_stage_manager_encore", TRUE, CONFIG_FILE);
	auto nHardwareSlider = GetPrivateProfileIntW(L"patches", L"hardware_slider", FALSE, CONFIG_FILE);

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
		InjectCode((void*)0x00000001403BABEA, { 0x75 });
		printf("[Patches] Show FREE PLAY instead of CREDIT(S)\n");

		if (nPDLoaderText && !nHideFreeplay)
		{
			InjectCode((void*)0x00000001409F61F0, { 0x50, 0x44, 0x20, 0x4C, 0x6F, 0x61, 0x64, 0x65, 0x72, 0x20, 0x41, 0x4C, 0x50, 0x48, 0x41});
			printf("[Patches] Show PD Loader version\n");
		}
	}
	// Use GLUT_CURSOR_RIGHT_ARROW instead of GLUT_CURSOR_NONE
	if (nCursor)
	{
		InjectCode((void*)0x000000014019341B, { 0x00 });	
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
	// Enhanced Stage Manager
	if (nEStageManager > 0)
	{
		// Replace the function that provides the number of stages and compact some of it
		InjectCode((void*)0x000000014038AEF0, { 0x48, 0x8B, 0x88, 0x40, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x20, 0x48, 0x8B, 0xD0, 0x48, 0x8B, 0x88, 0x48, 0x01, 0x00, 0x00, 0x48, 0x89, 0x4C, 0x24, 0x28, 0x8B, 0x88, 0x50, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x30, 0x8B, 0x88, 0x54, 0x01, 0x00, 0x00, 0x8B, 0x80, 0x58, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x38, 0x8B, 0x82, 0x5C, 0x01, 0x00, 0x00, 0x89, 0x4C, 0x24, 0x34, 0x89, 0x44, 0x24, 0x3C, 0x48, 0x8B, 0x82, 0x60, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x40, 0x48, 0x8B, 0x82, 0x68, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x48, 0x48, 0x8B, 0x82, 0x70, 0x01, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x50, 0x8B, 0x82, 0x78, 0x01, 0x00, 0x00, 0x89, 0x44, 0x24, 0x58, 0x84, 0xC0, 0x74, 0x2A, 0x48, 0x8B, 0x44, 0x24, 0x38, 0x48, 0xC1, 0xE8, 0x20, 0x85, 0xC0, 0x75, 0x1D, 0xE8, 0xD9, 0xD9, 0xE5, 0xFF, 0x48, 0x85, 0xC0, 0x74, 0x13, 0x48, 0x8D, 0x48, 0x10, 0xE8, 0xFB, 0xD3, 0xE5, 0xFF, 0xB9, 0x03, 0x00, 0x00, 0x00, 0x85, 0xC0, 0x0F, 0x45, 0xD9, 0x8B, 0x1D, 0x1B, 0x0C, 0x00, 0x00, 0x83, 0x3D, 0x1C, 0x0C, 0x00, 0x00, 0x00, 0x74, 0x06, 0x8B, 0x1D, 0x10, 0x0C, 0x00, 0x00, 0x8B, 0xC3, 0x48, 0x83, 0xC4, 0x60, 0x5B, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC });
		
		// Jump to another section by addding some code to replace the values (Jump 1)
		InjectCode((void*)0x000000014038AFF4, { 0xE9, 0x87, 0x0B, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC });

		// Jump to another section by addding some code to replace the values (Replace 1/2) while some reserving space for values
		InjectCode((void*)0x000000014038BB40, { 0xFF, 0x05, 0x76, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x70, 0x00, 0x00, 0x00, 0xBA, 0x02, 0x00, 0x00, 0x00, 0x83, 0x3D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x74, 0x02, 0xFF, 0xC2, 0x39, 0xD1, 0x0F, 0x4D, 0xCA, 0x89, 0x48, 0x08, 0xB9, 0x0E, 0x00, 0x00, 0x00, 0xE8, 0x92, 0x98, 0xE0, 0xFF, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x28, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x44, 0x89, 0x61, 0x08, 0x44, 0x88, 0x61, 0x0C, 0x4C, 0x89, 0x61, 0x10, 0x44, 0x89, 0x25, 0x29, 0x00, 0x00, 0x00, 0xE9, 0x68, 0xF4, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

		// Use the value from our own address instead of the original one
		InjectCode((void*)0x00000001403F65AF, { 0x48, 0x8D, 0x05, 0x4A, 0x18, 0xDA, 0x00, 0x8B, 0x1D, 0x00, 0x56 });

		// Jump to another section by addding some code to replace the values (Jump 2)
		InjectCode((void*)0x00000001403F6638, { 0xE9, 0x03, 0x55, 0xF9, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC});

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
	// The original slider update needs to run for hardware sliders to work -- only patch it when using emulation
	if (!nHardwareSlider)
	{
		// Don't update the touch slider state so we can write our own
		InjectCode((void*)0x000000014061579B, { 0x90, 0x90, 0x90, 0x8B, 0x42, 0xE0, 0x90, 0x90, 0x90 });
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
