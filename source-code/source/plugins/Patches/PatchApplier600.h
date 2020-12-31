#pragma once
#include <vector>
#include "PatchApplier.h"
#include "framework.h"

class PatchApplier600 : public PatchApplier {
	virtual void ApplyPatches() {
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
			InjectCode((void*)(0x0000000140371E50 + 0x104), { 0xE9, 0x87, 0x0B, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }); // unsure

			// Jump to another section by addding some code to replace the values (Replace 1/2) while some reserving space for values
			InjectCode((void*)(0x0000000140371E50 + 0xC50), { 0xFF, 0x05, 0x76, 0x00, 0x00, 0x00, 0x8B, 0x0D, 0x70, 0x00, 0x00, 0x00, 0xBA, 0x02, 0x00, 0x00, 0x00, 0x83, 0x3D, 0x60, 0x00, 0x00, 0x00, 0x00, 0x74, 0x02, 0xFF, 0xC2, 0x39, 0xD1, 0x0F, 0x4D, 0xCA, 0x89, 0x48, 0x08, 0xB9, 0x0E, 0x00, 0x00, 0x00, 0xE8, 0x92, 0x98, 0xE0, 0xFF, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x28, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x44, 0x89, 0x61, 0x08, 0x44, 0x88, 0x61, 0x0C, 0x4C, 0x89, 0x61, 0x10, 0x44, 0x89, 0x25, 0x29, 0x00, 0x00, 0x00, 0xE9, 0x68, 0xF4, 0xFF, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }); // unsure

			// Use the value from our own address instead of the original one
			InjectCode((void*)0x00000001403DDABF, { 0x48, 0x8D, 0x05, 0x4A, 0x18, 0xDA, 0x00, 0x8B, 0x1D, 0x00, 0x56 }); // unsure

			// Jump to another section by addding some code to replace the values (Jump 2)
			InjectCode((void*)(0x00000001403F65AF + 0x89), { 0xE9, 0x03, 0x55, 0xF9, 0xFF, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }); // unsure

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
	}
};