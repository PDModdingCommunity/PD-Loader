#pragma once

#pragma once
#include <vector>
#include "PatchApplier.h"
#include "framework.h"

class PatchApplier710 : public PatchApplier {
	virtual void ApplyPatches() {
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

		// gamma
		if (nGamma != 100 && nGamma != 0)
		{
			float gamma_float = (float)nGamma / 100.0f;
			InjectCode((void*)(0x1404a863b), { *((uint8_t*)(&gamma_float)), *((uint8_t*)(&gamma_float) + 1), *((uint8_t*)(&gamma_float) + 2), *((uint8_t*)(&gamma_float) + 3) });

			printf("[Patches] Gamma: %f\n", gamma_float);
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
				InjectCode((void*)0x00000001409F61F0, { 0x50, 0x44, 0x20, 0x4C, 0x6F, 0x61, 0x64, 0x65, 0x72,
					0x20, 0x32, 0x2E, 0x36, 0x2E, 0x35,
					0x00});
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
		if (!nUseDivahookBat)
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
		if (nEnablePvCustomization)
		{
			// Disable hand scaling
			if (nNoHandScaling)
			{
				InjectCode((void*)0x0000000140120709, { 0xE9, 0x82, 0x0A, 0x00 });
			}
			// Default hand size
			if (nDefaultHandSize >= 10000 && nDefaultHandSize <= 1000000)
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
					for (int i = 0; i < 9; i++)
					{
						look_table_oldid[i]--;
					}
					VirtualProtect(look_table_oldid, 36, oldProtect, nullptr);
					printf("[Patches] FT look forced\n");
				}
			}
		}
		// Sing missed notes
		if (nSingMissed)
		{
			InjectCode((void*)0x140109044, { 0xEB });
			// Breaks border:
			// InjectCode((void*)0x140109096, { 0xEB });
		}
		// NPR1
		{
			if (nNpr1 == 1) // force on
			{
				InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
				InjectCode((void*)0x0000000140502FC6, { 0x01, 0x00, 0x00, 0x00, 0xC3 });

				printf("[Patches] NPR1 forced\n");
			}
			else if (nNpr1 == 2) // force off
			{
				InjectCode((void*)0x0000000140502FC0, { 0xC7, 0x05, 0x6E });
				InjectCode((void*)0x0000000140502FC6, { 0x00, 0x00, 0x00, 0x00, 0xC3 });

				printf("[Patches] NPR1 disabled\n");
			}
		}
		// Depth of Field
		if (!nDoF)
		{
			InjectCode((void*)0x00000001409476AB, { 0x90, 0x90, 0x90, 0x90, 0x90 });

			InjectCode((void*)0x00000001411AB650, { 0b00000001 });
		}
		// MAG filter
		if (nMAG > 0)
		{
			//InjectCode((void*)0x00000001404AB142, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

			unsigned char filter;
			switch (nMAG)
			{
			case 1: // nearest
				filter = 0;
				InjectCode((void*)0x00000001404ACE56, { 0x90, 0x90 });
				break;
			case 2: // sharpen
				filter = 3; // sharpen(4tap)
				break;
			case 3: // cone
				filter = 5; // cone(2tap)
				break;
			default:
				filter = 1;
			}

			//InjectCode((void*)0x00000001404A864F, { filter });
			//InjectCode((void*)0x00000001411AC518, { filter });

			InjectCode((void*)0x00000001404ACE8E, { 0xB9, filter, 0x00, 0x00 });
			InjectCode((void*)0x00000001404ACE93, { 0x90 });
		}
		// Reflections
		if (!nReflections)
		{
			InjectCode((void*)0x1406477C0, { 0xE9, 0xF3, 0x00, 0x00, 0x00, 0x90 });
			InjectCode((void*)0x1411ADAFC, { 0x00 });
		}
		// Shadows
		if (!nShadows)
		{
			//InjectCode((void*)0x140502B44, { 0x90, 0x90, 0x90 });
			//InjectCode((void*)0x140502BB2, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)0x140502766, { 0x00 });
			InjectCode((void*)0x1411AD438, { 0x00 });
			InjectCode((void*)0x1411AD320, { 0x00 });
		}
		// Punch-through transparency
		if (!nPunchthrough)
		{
			InjectCode((void*)0x1411AD43D, { 0x00 });
		}
		// Glare
		if (!nGlare)
		{
			InjectCode((void*)0x1404B2168, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
		}
		// Shader
		if (!nShader)
		{
			InjectCode((void*)0x140C9C48E, { 0x00 });
		}
		// 2D
		if (n2D)
		{
			//InjectCode((void*)0x140502B44, { 0x90, 0x90, 0x90 });
			InjectCode((void*)0x140502BB2, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)0x140502A3C, { 0x90, 0x90, 0x90, 0x90, 0x90 });
			/*InjectCode((void*)0x1411AD320, { 0x00 }); // shadow
			InjectCode((void*)0x1411AD323, { 0x00 }); // reflect
			InjectCode((void*)0x1411AD321, { 0x00 }); // SS_SSS
			InjectCode((void*)0x1411AD325, { 0x00 }); // preprocess
			InjectCode((void*)0x1411AD328, { 0x00 }); // 3D*/
			InjectCode((void*)0x1411AD32A, { 0x01 }); // post process
			InjectCode((void*)0x1411AD32B, { 0x01 }); // sprite

			InjectCode((void*)0x140A07920, { 0x00 }); // ignore objset
		}
		// Prevent data deletion
		if (nNoDelete)
		{
			InjectCode((void*)(0x1406FEF80 + 0x1DB), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407113F0 + 0x13C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712270 + 0x6C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712580 + 0x4C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712580 + 0x7C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713470 + 0x104), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x102), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x135), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x232), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713810 + 0x3E8), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713D00 + 0x677), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x9C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x1D2), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x218), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x22A), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140720910 + 0x25C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140723EE0 + 0xEC), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140870DE0 + 0x4), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1406FEF80 + 0x1DB), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407113F0 + 0x13C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712270 + 0x6E), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712580 + 0x4C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140712580 + 0x7C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713470 + 0x104), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x102), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x135), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407135B0 + 0x232), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713810 + 0x3E8), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140713D00 + 0x677), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x9C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x1D2), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x218), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x22A), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140720910 + 0x25C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140723EE0 + 0xEC), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407168A0 + 0x9C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140870DE0 + 0x4), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x14081B6A4), { 0xC3 });

			InjectCode((void*)(0x1406E1090 + 0x43C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140718250 + 0x176), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140718750 + 0xC9), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407193E0 + 0xFC), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1406E1090 + 0x43C), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140718250 + 0x176), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x140718750 + 0xC9), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x1407193E0 + 0xFC), { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
			InjectCode((void*)(0x14081B67A), { 0xC3 });
		}

		// patch refract and reflect buffer sizes
		if (nRefractResWidth != 512 || nRefractResHeight != 256)
		{
			InjectCode((void*)(0x140a24424), std::vector<uint8_t>((uint8_t*)&nRefractResWidth, (uint8_t*)((int64_t)&nRefractResWidth + 4)));
			InjectCode((void*)(0x140a24428), std::vector<uint8_t>((uint8_t*)&nRefractResHeight, (uint8_t*)((int64_t)&nRefractResHeight + 4)));

			printf("[Patches] Refraction resolution patched.\n");
		}
		if (nReflectResWidth != 512 || nReflectResHeight != 256)
		{
			InjectCode((void*)(0x140a2443c), std::vector<uint8_t>((uint8_t*)&nReflectResWidth, (uint8_t*)((int64_t)&nReflectResWidth + 4)));
			InjectCode((void*)(0x140a24440), std::vector<uint8_t>((uint8_t*)&nReflectResHeight, (uint8_t*)((int64_t)&nReflectResHeight + 4)));
			// for large reflect resolutions, adjust blur effect amount to not lose the effect
			// scaling is based on a 1024 width instead of the original 512px
			// because we don't wanna make it too blurry when just being used to smooth jaggies
			if (nReflectResWidth >= 2048)
			{
				int blurAmount = nReflectResWidth / 1024;

				// multiply blur setting
				// ECX has blur amount, EDX has blur type
				InjectCode((void*)(0x140503000), { 0x89, 0x15, 0x3a, 0xa3, 0xca, 0x00 });  // MOV  dword ptr [0x1411ad340], EDX
				InjectCode((void*)(0x140503006), { 0xb8 });                                // MOV  EAX, blurAmount
				InjectCode((void*)(0x140503007), std::vector<uint8_t>((uint8_t*)&blurAmount, (uint8_t*)((int64_t)&blurAmount + 4)));
				InjectCode((void*)(0x14050300b), { 0xf7, 0xe1 });                          // MUL  (EDX:EAX,)ECX
				InjectCode((void*)(0x14050300d), { 0xeb, 0x09 });                          // JMP  0x140503018
				InjectCode((void*)(0x140503018), { 0x89, 0x05, 0x1e, 0xa3, 0xca, 0x00 });  // MOV  dword ptr [0x1411ad33c], EAX
				InjectCode((void*)(0x14050301e), { 0xc3 });                                // RET

				// divide dwgui blur amount getting
				// RDX gets type, RCX gets amount
				InjectCode((void*)(0x140502910), { 0x48, 0x85, 0xd2 });                    // TEST  RDX, RDX
				InjectCode((void*)(0x140502913), { 0x74, 0x08 });                          // JZ  0x14050291d
				InjectCode((void*)(0x140502915), { 0x8b, 0x05, 0x25, 0xaa, 0xca, 0x00 });  // MOV  EAX, dword ptr [0x1411ad340]
				InjectCode((void*)(0x14050291b), { 0x89, 0x02 });                          // MOV  dword ptr [RDX], EAX

				InjectCode((void*)(0x14050291d), { 0x48, 0x85, 0xc9 });                    // TEST  RCX, RCX
				InjectCode((void*)(0x140502920), { 0x74, 0x2d });                          // JZ  0x14050294f
				InjectCode((void*)(0x140502922), { 0x8b, 0x05, 0x14, 0xaa, 0xca, 0x00 });  // MOV  EAX, dword ptr [0x1411ad33c]

				InjectCode((void*)(0x140502928), { 0xeb, 0x0d });                          // JMP  0x140502937
				InjectCode((void*)(0x140502937), { 0x49, 0xc7, 0xc0 });                    // MOV  R8, blurAmount
				InjectCode((void*)(0x14050293a), std::vector<uint8_t>((uint8_t*)&blurAmount, (uint8_t*)((int64_t)&blurAmount + 4)));
				InjectCode((void*)(0x14050293e), { 0xeb, 0x08 });                          // JMP  0x140502948
				InjectCode((void*)(0x140502948), { 0x31, 0xd2 });                          // XOR  EDX,EDX
				InjectCode((void*)(0x14050294a), { 0x49, 0xf7, 0xf0 });                    // DIV  (EDX:EAX,)R8

				InjectCode((void*)(0x14050294d), { 0x89, 0x01 });                          // MOV  dword ptr [RCX], EAX
				InjectCode((void*)(0x14050294f), { 0xc3 });                                // RET
			}

			printf("[Patches] Reflection resolution patched.\n");
		}

		// lag compensation
		if (nLagCompensation > 0 && nLagCompensation <= 500)
		{
			InjectCode((void*)(0x14011e44e), { 0xf3, 0x0f, 0x10, 0x05, 0x8a, 0xcf, 0x9c, 0x00 });	// hijack xmm0
			InjectCode((void*)(0x14011e46b), { 0xf3, 0x0f, 0x11, 0x44, 0x24, 0x20 });	// get value from xmm0 instead of xmm1
			float startPos = (float)nLagCompensation / 1000.0f;
			InjectCode((void*)(0x140aeB3e0), { *((uint8_t*)(&startPos)), *((uint8_t*)(&startPos) + 1), *((uint8_t*)(&startPos) + 2), *((uint8_t*)(&startPos) + 3) });

			printf("[Patches] Lag Compensation: %f\n", startPos);
		}
	}
};