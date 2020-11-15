# gane-specific settings for diva arcade future tone
GAME_NAME = "Project DIVA Arcade: Future Tone"

from re import compile as recompile

# some skinning can't be done due to using NV parameter buffers, so use an otherwise unused texture instead.
VP_SKINNING_REGEX = recompile(r"INT TEMP _adr;\s*?(BUFFER4[^;]*?;)([\s\S]*?_mtxpal)")
VP_SKINNING_SUB = "TEMP _adr;\n#\\1 \nTEMP mtx_tmp, mtx_tmp1, mtx_tmp2;\\2"

# load the address temp with a texture coordinate instead
# hardcoded to assume a 768*1 pixel texture
VP_SKINNING_REGEX_2 = recompile(r"(BUFFER4[\s\S]*?)CVT(?:.S32.F32)?\s*?(.*?),\s*?(.*?);")
VP_SKINNING_SUB_2 = "\\1MUL \\2, \\3, 0.0013037809647979;" #1/767

VP_SKINNING_REGEX_3 = recompile(r"(BUFFER4[\s\S]*?)(DP4 ([^;]*?), _mtxpal\[([^\]]*?\.x) \])")
VP_SKINNING_REGEX_3b = recompile(r"(BUFFER4[\s\S]*?)(DP4 ([^;]*?), _mtxpal\[([^\]]*?\.y) \])")
VP_SKINNING_REGEX_3c = recompile(r"(BUFFER4[\s\S]*?)(DP4 ([^;]*?), _mtxpal\[([^\]]*?\.z) \])")
VP_SKINNING_REGEX_3d = recompile(r"(BUFFER4[\s\S]*?)(DP4 ([^;]*?), _mtxpal\[([^\]]*?\.w) \])")
VP_SKINNING_SUB_3 = "\\1TEX mtx_tmp, \\4, texture[8], 1D; ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp1, \\4, texture[8], 1D; ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp2, \\4, texture[8], 1D; DP4 \\3, _mtxpal[\\4+ ]"

VP_SKINNING_REGEX_4 = recompile(r"(BUFFER4[\s\S]*?)_mtxpal\[([^\]]*?\.\S)\+?(\s|\d)\]")
VP_SKINNING_SUB_4 = "\\1mtx_tmp\\3"


# some kind of effect (scrunching up or stretching?) in tights vp shaders starting with 11 that partially uses skinning of normals...
# works much the same as the regular vp skinning
# make these run before VP_SKINNING_REGEX_4
#TIGHTS_SKINNING_EFFECT_REGEX = recompile(r"(BUFFER4[\s\S]*?)SUBC .*?, vertex.attrib\[15\], .*?;\s*?SUBC1 tmp, vertex.attrib\[15\], .*?;[\s\S]*?XPD .*?, .*?, .*?;\s*?DP3_SAT (.*?), .*?, .*?;")
#TIGHTS_SKINNING_EFFECT_SUB = "\\1MOV \\2, 0;"
TIGHTS_SKINNING_EFFECT_REGEX = recompile(r"(BUFFER4[\s\S]*?)CVT(?:.S32.F32)?\s*?(.*?),\s*?(.*?);")
TIGHTS_SKINNING_EFFECT_SUB = "\\1MUL \\2, \\3, 0.0013037809647979;"
#TIGHTS_SKINNING_EFFECT_REGEX_2 = recompile(r"(BUFFER4[\s\S]*?)(IF GT1.y;[\s\S]*?|)(DP3 ([^;]*?), _mtxpal\[([^\]]*?\.x) \])(?![\s\S]*MUL  _adr)")
#TIGHTS_SKINNING_EFFECT_REGEX_2b = recompile(r"(BUFFER4[\s\S]*?)(IF GT1.y;[\s\S]*?|)(DP3 ([^;]*?), _mtxpal\[([^\]]*?\.y) \])(?![\s\S]*MUL  _adr)")
#TIGHTS_SKINNING_EFFECT_SUB_2 = "\\1TEX mtx_tmp, \\5, texture[8], 1D; ADD \\5, \\5, 0.0013037809647979; TEX mtx_tmp1, \\5, texture[8], 1D; ADD \\5, \\5, 0.0013037809647979; TEX mtx_tmp2, \\5, texture[8], 1D; \\2 DP3 \\4, _mtxpal[\\5+ ]"
TIGHTS_SKINNING_EFFECT_REGEX_2 = recompile(r"(BUFFER4[\s\S]*?)(DP3 ([^;]*?), _mtxpal\[([^\]]*?\.x) \])(?![\s\S]*MUL  _adr)")
TIGHTS_SKINNING_EFFECT_SUB_2 = "\\1TEX mtx_tmp, \\4, texture[8], 1D; ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp1, \\4, texture[8], 1D; ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp2, \\4, texture[8], 1D; DP3 \\3, _mtxpal[\\4+ ]"
TIGHTS_SKINNING_EFFECT_REGEX_3 = recompile(r"(BUFFER4[\s\S]*?)(DP3 ([^;]*?), _mtxpal\[([^\]]*?\.y) \])(?![\s\S]*MUL  _adr)")
TIGHTS_SKINNING_EFFECT_SUB_3 = "\\1TEX mtx_tmp, \\4, texture[8], 1D; DP3 \\3, _mtxpal[\\4+ ]"
TIGHTS_SKINNING_EFFECT_REGEX_4 = recompile(r"(BUFFER4[\s\S]*?)(DP3 ([^;]*?), _mtxpal\[([^\]]*?\.y)\+1\])(?![\s\S]*MUL  _adr)")
TIGHTS_SKINNING_EFFECT_SUB_4 = "\\1ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp1, \\4, texture[8], 1D; ADD \\4, \\4, 0.0013037809647979; TEX mtx_tmp2, \\4, texture[8], 1D; DP3 \\3, _mtxpal[\\4+1]"



# some skinning just needs some branching fixup for AMD BRA working different (needs a target per branch instruction apparently)
SKIN_BRA_REGEX = recompile(r"BRA skinning_(?:\S*)end (\(.*?\.y\))*?\s*?;")
SKIN_BRA_SUB = "BRA sk_end1 \\1;"
SKIN_BRA_REGEX_2 = recompile(r"BRA skinning_(?:\S*)end (\(.*?\.z\))*?\s*?;")
SKIN_BRA_SUB_2 = "BRA sk_end2 \\1;"
SKIN_BRA_REGEX_3 = recompile(r"BRA skinning_(?:\S*)end (\(.*?\.w\))*?\s*?;")
SKIN_BRA_SUB_3 = "BRA sk_end3 \\1;"

SKIN_BRA_TGT_REGEX = recompile(r"skinning_(?:\S*)end:")
SKIN_BRA_TGT_SUB = "\nsk_end1:\nsk_end2:\nsk_end3:\n"


MATCH_FP_FNAME = recompile(r".*fp")
MATCH_VP_FNAME = recompile(r".*vp")
MATCH_ALL_FNAME = recompile(r".*")
MATCH_FONT_FNAME = recompile(r"font.*fp")
MATCH_TIGHTS_FNAME = recompile(r"tights.*vp")

# fourth param: apply continuously until no more matches are found
fix_repls = [
    (MATCH_VP_FNAME, VP_SKINNING_REGEX, VP_SKINNING_SUB, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_2, VP_SKINNING_SUB_2, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_3, VP_SKINNING_SUB_3, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_3b, VP_SKINNING_SUB_3, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_3c, VP_SKINNING_SUB_3, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_3d, VP_SKINNING_SUB_3, False),
    (MATCH_TIGHTS_FNAME, TIGHTS_SKINNING_EFFECT_REGEX, TIGHTS_SKINNING_EFFECT_SUB, False),
    (MATCH_TIGHTS_FNAME, TIGHTS_SKINNING_EFFECT_REGEX_2, TIGHTS_SKINNING_EFFECT_SUB_2, False),
    (MATCH_TIGHTS_FNAME, TIGHTS_SKINNING_EFFECT_REGEX_3, TIGHTS_SKINNING_EFFECT_SUB_3, False),
    (MATCH_TIGHTS_FNAME, TIGHTS_SKINNING_EFFECT_REGEX_4, TIGHTS_SKINNING_EFFECT_SUB_4, False),
    (MATCH_VP_FNAME, VP_SKINNING_REGEX_4, VP_SKINNING_SUB_4, True),
    (MATCH_VP_FNAME, SKIN_BRA_REGEX, SKIN_BRA_SUB, False),
    (MATCH_VP_FNAME, SKIN_BRA_REGEX_2, SKIN_BRA_SUB_2, False),
    (MATCH_VP_FNAME, SKIN_BRA_REGEX_3, SKIN_BRA_SUB_3, False),
    (MATCH_VP_FNAME, SKIN_BRA_TGT_REGEX, SKIN_BRA_TGT_SUB, False),
]


# use to open a differnt file instead of the original shader (eg. to replace with an easier-to-patch variant
def filename_filter(fname):
    if fname == 'sss_filter.130.fp':
        return 'sss_filter.120.fp'
    elif fname == 'sss_filter.131.fp':
        return 'sss_filter.121.fp'
    else:
        return fname


# use to tweak reasults after patching
def post_filter(fname, f_full):
    f_full = f_full.replace('\nBUFFER4', '\n#BUFFER4')
    
    # fix tex sampler offsets
    # diva has something we can use for pixel size (program.local[0]), but it isn't in fonts so they get an approximation instead
    if MATCH_FONT_FNAME.match(fname):
        f_full = f_full.replace('{ 0.00078, 0.0014 }; ADD tex_offset_coord', '0.00035; ADD tex_offset_coord')
    else:
        f_full = f_full.replace('{ 0.00078, 0.0014 }; ADD tex_offset_coord', 'program.local[0]; ADD tex_offset_coord')
        
    return f_full