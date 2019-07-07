#include "windows.h"
#include "vector"
#include <tchar.h>
#include <GL/freeglut.h>

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
		// Don't update the touch slider state so we can write our own
		{ (void*)0x000000014061579B, { 0x90, 0x90, 0x90, 0x8B, 0x42, 0xE0, 0x90, 0x90, 0x90 } },
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
		// enable module selector without use_card (Modules are not getting saved upon selection, commented until a fix is found)
		//{ (void*)0x00000001405C513B, { 0x01 } },
		// Show Freeplay instead
		{ (void*)0x00000001403BABEA, { 0x75 } },
		// Force Hide IDs
		{ (void*)0x00000001409A5918, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
		{ (void*)0x00000001409A5928, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }
	};
	printf("[Patches] Patches loaded\n");

	for (size_t i = 0; i < _countof(patches); i++)
		InjectCode(patches[i].Address, patches[i].Data);

	auto nCursor = GetPrivateProfileIntW(L"patches", L"cursor", TRUE, CONFIG_FILE);
	auto nHideFreeplay = GetPrivateProfileIntW(L"patches", L"hide_freeplay", FALSE, CONFIG_FILE);
	auto nStatusIcons = GetPrivateProfileIntW(L"patches", L"status_icons", 0, CONFIG_FILE);
	auto nHidePVWatermark = GetPrivateProfileIntW(L"patches", L"hide_pv_watermark", FALSE, CONFIG_FILE);
	auto nNoPVUi = GetPrivateProfileIntW(L"patches", L"no_pv_ui", FALSE, CONFIG_FILE);
	auto nHideVolCtrl = GetPrivateProfileIntW(L"patches", L"hide_volume", FALSE, CONFIG_FILE);
	auto nNoLyrics = GetPrivateProfileIntW(L"patches", L"no_lyrics", FALSE, CONFIG_FILE);
	auto nNoMovies = GetPrivateProfileIntW(L"patches", L"no_movies", FALSE, CONFIG_FILE);
	auto nNoError = GetPrivateProfileIntW(L"patches", L"no_error", FALSE, CONFIG_FILE);
	// Hides the Freeplay text
	if (nHideFreeplay)
	{
		InjectCode((void*)0x00000001403BABEF, { 0x06, 0xB6 });
		printf("[Patches] Hide Freeplay enabled\n");
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
}

void InjectCode(void* address, const std::vector<uint8_t> data)
{
	const size_t byteCount = data.size() * sizeof(uint8_t);

	DWORD oldProtect;
	VirtualProtect(address, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(address, data.data(), byteCount);
	VirtualProtect(address, byteCount, oldProtect, nullptr);
}
