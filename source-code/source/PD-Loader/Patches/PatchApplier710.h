#pragma once
#include <vector>
#include "PatchApplier.h"
#include "patches-framework.h"

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



		/////////////////////
		////// Options //////
		/////////////////////
		
		//// Graphics
		
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

		// lag compensation
		if (nLagCompensation > 0 && nLagCompensation <= 500)
		{
			InjectCode((void*)(0x14011e44e), { 0xf3, 0x0f, 0x10, 0x05, 0x8a, 0xcf, 0x9c, 0x00 });	// hijack xmm0
			InjectCode((void*)(0x14011e46b), { 0xf3, 0x0f, 0x11, 0x44, 0x24, 0x20 });	// get value from xmm0 instead of xmm1
			float startPos = (float)nLagCompensation / 1000.0f;
			InjectCode((void*)(0x140aeB3e0), { *((uint8_t*)(&startPos)), *((uint8_t*)(&startPos) + 1), *((uint8_t*)(&startPos) + 2), *((uint8_t*)(&startPos) + 3) });

			printf("[Patches] Lag Compensation: %f\n", startPos);
		}


		//// Compatibility
		
		// Skip loading (and therefore displaying) song movies
		if (nNoMovies)
		{
			InjectCode((void*)0x00000001404EB584, { 0x48, 0xE9 });
			InjectCode((void*)0x00000001404EB471, { 0x48, 0xE9 });
			printf("[Patches] Movies disabled\n");
		}

		// The old stereo patch...
		// Use 2 channels instead of 4
		if (nStereo)
		{
			InjectCode((void*)0x0000000140A860C0, { 0x02 });
			printf("[Patches] Stereo patch enabled\n");
		}

		// OpenGL Patches
		/*if (nOGLPatchA)
		{
			// (call cs:glGetError) -> (xor eax, eax); (nop); (nop); (nop); (nop);
			InjectCode((void*)0x000000014069D21D, { 0x31, 0xC0, 0x90, 0x90, 0x90, 0x90 });
		}
		if (nOGLPatchB)
		{
			// (js loc_14069BC32) -> (nop)...
			InjectCode((void*)0x000000014069BAF4, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
		}*/


		//// PV Selector
		
		// Enable "FREE PLAY" mode
		if (nFreeplay || nHideFreeplay)
		{
			// Always return true for the SelCredit enter SelPv check
			InjectCode((void*)0x0000000140393610, { 0xB0, 0x01, 0xC3, 0x90, 0x90, 0x90 });

			InjectCode((void*)0x00000001403BABEA, { 0x75 });
			printf("[Patches] Show FREE PLAY instead of CREDIT(S)\n");
		}


		//// UI Elements

		// Use GLUT_CURSOR_RIGHT_ARROW instead of GLUT_CURSOR_NONE
		if (nCursor)
		{
			InjectCode((void*)0x000000014019341B, { 0x00 });
			InjectCode((void*)0x00000001403012b5, { 0xeb }); // Disable debug cursor
			printf("[Patches] Cursor enabled\n");
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

		// Removes PV watermark
		if (nHidePVWatermark)
		{
			InjectCode((void*)0x0000000140A13A88, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
			printf("[Patches] PV watermark hidden\n");
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

		// Hide "FREE PLAY"
		if (nHideFreeplay)
		{
			InjectCode((void*)0x00000001403BABEF, { 0x06, 0xB6 });
			printf("[Patches] Hide FREE PLAY/CREDIT(S) enabled\n");
		}

		// Skip loading (and therefore displaying) song lyrics
		if (nNoLyrics)
		{
			InjectCode((void*)0x00000001404E7A25, { 0x00, 0x00 });
			InjectCode((void*)0x00000001404E7950, { 0x48, 0xE9 }); // ensure first iteration doesn't run
			printf("[Patches] Lyrics disabled\n");
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

		// The original slider update needs to run for hardware sliders to work -- only patch it when using emulation
		if (!nHardwareSlider)
		{
			// Don't update the touch slider state so we can write our own
			InjectCode((void*)0x000000014061579B, { 0x90, 0x90, 0x90, 0x8B, 0x42, 0xE0, 0x90, 0x90, 0x90 });
		}

		// debug scaling by somewhatlurker
		if (nDwguiScaling)
		{
			InjectCode((void*)(0x1402ffccf), { 0x0d, 0x00, 0x00, 0x00 });  // 720p res index

			InjectCode((void*)(0x1403025f2), { 0x48, 0x8d, 0x05, 0x17, 0x65, 0x6b, 0x00 });  // LEA  RAX, [0x1409b8b10] (720p res info at +0xc, +-x10)
			InjectCode((void*)(0x1403025f9), { 0xeb, 0xb3 });  // JMP  0x1403025ae
			InjectCode((void*)(0x1403025a9), { 0xeb, 0x47 });  // JMP  0x1403025f2


			// hacky fix for mouse input scaling
			// someone please fix this properly if possible

			// directly load mouse coordinates to xmm0 to save code space
			InjectCode((void*)(0x140300a62), { 0x0f, 0x2a, 0x86, 0xa0, 0x00, 0x00, 0x00 });  // CVTPI2PS  XMM0, qword ptr[RSI + 0xa0]
			// load 720p
			InjectCode((void*)(0x140300a69), { 0x0f, 0x2a, 0x0d, 0xac, 0x80, 0x6b, 0x00 });  // CVTPI2PS  XMM1, qword ptr [0x1409b8b1c]
			// load native res (140eda8b0 is the native res version of the above 720p res info)
			InjectCode((void*)(0x140300a70), { 0x0f, 0x2a, 0x15, 0x45, 0x9e, 0xbd, 0x00 });  // CVTPI2PS  XMM2, qword ptr [0x140eda8bc]
			// math
			InjectCode((void*)(0x140300a77), { 0x0f, 0x59, 0xc1 });  // MULPS  XMM0, XMM1
			InjectCode((void*)(0x140300a7a), { 0x0f, 0x5e, 0xc2 });  // DIVPS  XMM0, XMM2
			// used this space, go elsewhere
			InjectCode((void*)(0x140300a7d), { 0xe9, 0x83, 0x06, 0x00, 0x00 });  // JMP  0x140301105

			// need to restore this instruction
			InjectCode((void*)(0x140301105), { 0x0f, 0x57, 0xe4 });  // XORPS  XMM4, XMM4
			// need to unpack XMM0 into XMM1 and XMM0
			InjectCode((void*)(0x140301108), { 0xf3, 0x0f, 0x10, 0xc8 });  // MOVSS  XMM1, XMM0
			// move to new space
			InjectCode((void*)(0x14030110c), { 0xeb, 0x07 });  // JMP  0x140301115
			// copy high float of xmm0 to low float of xmm0
			InjectCode((void*)(0x140301115), { 0x0f, 0xc6, 0xc0, 0x55 });  // SHUFPS  XMM0, XMM0, 0x55
			// finally back to original code
			InjectCode((void*)(0x140301119), { 0xe9, 0x64, 0xf9, 0xff, 0xff });  // JMP  0x140300a82

			// don't need these anymore
			InjectCode((void*)(0x140300a82), { 0x90, 0x90, 0x90 });  // NOPs
			InjectCode((void*)(0x140300a88), { 0x90, 0x90, 0x90 });  // NOPs
		}
		ApplyExPatches();
	}
	#include "PatchApplier710EX.h"
};
