#pragma once
#ifndef MINIMALIST
void ApplyExPatches() {
    //// Graphics

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

    // Depth of Field
    if (!nDoF)
    {
        InjectCode((void*)0x00000001409476AB, { 0x90, 0x90, 0x90, 0x90, 0x90 });

        InjectCode((void*)0x00000001411AB650, { 0b00000001 });
    }

    // Reflections
    if (!nReflections)
    {
        InjectCode((void*)0x1406477C0, { 0xE9, 0xF3, 0x00, 0x00, 0x00, 0x90 });
        InjectCode((void*)0x1411ADAFC, { 0x00 });
    }
     
    // patch refract and reflect buffer sizes
    {
        // no need to really check anything before injecting, because reinjecting defaults is fine
        InjectCode((void*)(0x140a24424), std::vector<uint8_t>((uint8_t*)&nRefractResWidth, (uint8_t*)((int64_t)&nRefractResWidth + 4)));
        InjectCode((void*)(0x140a24428), std::vector<uint8_t>((uint8_t*)&nRefractResHeight, (uint8_t*)((int64_t)&nRefractResHeight + 4)));
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
    }

    // gamma
    if (nGamma != 100)
    {
        float gamma_float = (float)nGamma / 100.0f;
        InjectCode((void*)(0x1404a863b), { *((uint8_t*)(&gamma_float)), *((uint8_t*)(&gamma_float) + 1), *((uint8_t*)(&gamma_float) + 2), *((uint8_t*)(&gamma_float) + 3) });

        printf("[Patches] Gamma: %f\n", gamma_float);
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


    //// Compatibility
    
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

    // no osage play data by Skyth
    if (nNoOpd)
    {
        InjectCode((void*)(0x1404728C0), { 0xC3 });
        InjectCode((void*)(0x1404789C3), { 0x0D });
    }


    //// PV Selector

    // Unlock PSEUDO modules (which will all default to Miku, unless we also patch them to match the first performer)
    if (nUnlockPseudo)
    {
        InjectCode((void*)0x0000000140A21770, { 0x00 });
        InjectCode((void*)0x0000000140A21780, { 0x00 });
        InjectCode((void*)0x0000000140A21790, { 0x00 });
        InjectCode((void*)0x0000000140A217A0, { 0x00 });
        InjectCode((void*)0x0000000140A217B0, { 0x00 });
    }

    // Disable scrolling sound effect
    if (nNoScrollingSfx)
    {
        InjectCode((void*)0x00000001405C84B3, { 0x90, 0x90, 0x90, 0x90, 0x90 });
    }


    //// UI Elements

    // Disable timer sprite
    if (nNoTimerSprite)
    {
        InjectCode((void*)0x00000001409C0758, { 0x00 }); // time_loop
        InjectCode((void*)0x0000000140A3D3F0, { 0x00 }); // time_in
        InjectCode((void*)0x0000000140A3D3F8, { 0x00 }); // time_out
    }

    // no message bar
    if (nNoMessageBar)
    {
        InjectCode((void*)(0x1409F6470), { 0x00 });

        // no scrolling message by snakemi
        InjectCode((void*)(0x1403B98FD), { 0x00, 0x00, 0x00, 0x00 });
        InjectCode((void*)(0x1403B9904), { 0x00, 0x00, 0x00, 0x00 });
        InjectCode((void*)(0x1403B990B), { 0x00, 0x00, 0x00, 0x00 });
    }

    // no stage text
    if (nNoStageText)
    {
        InjectCode((void*)(0x140A3D4C8), { 0x00 });
        InjectCode((void*)(0x140A3D4D8), { 0x00 });
        InjectCode((void*)(0x140A3D4E8), { 0x00 });
        InjectCode((void*)(0x140A3D4F8), { 0x00 });
        InjectCode((void*)(0x140A3D508), { 0x00 });
        InjectCode((void*)(0x140A3D518), { 0x00 });
        InjectCode((void*)(0x140A3D530), { 0x00 });
        InjectCode((void*)(0x140A3D540), { 0x00 });
        InjectCode((void*)(0x140A3D550), { 0x00 });
    }

    // Enable card button by somewhatlurker (pretty much just eye candy for now)
    if (nCard)
    {
        InjectCode((void*)0x0000000140565E6B, { 0x90, 0x90 });
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

    
    // modpack redirection
    /*{
    wchar_t* mpstr = NULL;
    mpstr = (wchar_t*)malloc(256);
    memset(mpstr, 0, 256);
    GetPrivateProfileStringW(L"Mods", L"Modpack", L"", mpstr, 256, CONFIG_FILE_NAME);
    std::string modpack_name(std::filesystem::path(mpstr).string());
    free(mpstr);
    std::cout << "[Patches] Modpack name: " << modpack_name << std::endl;

    if (modpack_name != "")
    {
    // ram
    std::string modpack_path = "modpacks\\" + modpack_name;
    std::cout << "[Patches] New ram path: " << modpack_path << std::endl;
    char* mpstr_path = (char*)malloc(modpack_path.size());
    memcpy(mpstr_path, modpack_path.c_str(), modpack_path.size());
    InjectLong((void*)0x0000000014096C120, (long)mpstr_path);

    // mdata
    std::string modpack_mdata = "modpacks\\" + modpack_name + "\\mdata";
    std::cout << "[Patches] New mdata path: " << modpack_mdata << std::endl;
    char* mpstr_mdata = (char*)malloc(modpack_mdata.size());
    memcpy(mpstr_mdata, modpack_mdata.c_str(), modpack_mdata.size());
    InjectCode((void*)0x000000014066CEA0, { 0x48, 0xC7, 0xC2 });
    InjectInt((void*)0x000000014066CEA3, (int)mpstr_mdata);
    InjectCode((void*)0x000000014066CE9C, { (unsigned char)modpack_mdata.size() });
    InjectCode((void*)0x000000014066CEAE, { (unsigned char)modpack_mdata.size() });
    }
    }*/
}
#else
void ApplyExPatches() {
	return;
};
#endif