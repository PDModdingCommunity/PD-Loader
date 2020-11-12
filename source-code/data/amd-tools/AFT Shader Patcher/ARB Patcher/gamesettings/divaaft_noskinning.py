# gane-specific settings for diva arcade future tone
GAME_NAME = "Project DIVA Arcade: Future Tone"

from re import compile as recompile

# some skinning can't be done due to using NV parameter buffers, so just disable it.
VP_SKINNING_REGEX = recompile(r"(BUFFER4[\s\S]*?)MOV (.*?).w, 1; SUBC _tmp1, vertex.attrib\[15\], (-1|255); (.*?) IF NE.y; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].y, \2; (.*?) MAD (.*?).xyz, _tmp0, vertex.attrib\[1\].y, \7; (.*?) MAD (.*?).xyz, _tmp0, vertex.attrib\[1\].y, \9; IF NE.z; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].z, \2; (.*?) MAD \7.xyz, _tmp0, vertex.attrib\[1\].z, \7; (.*?) MAD \9.xyz, _tmp0, vertex.attrib\[1\].z, \9; IF NE.w; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].w, \2; (.*?) MAD \7.xyz, _tmp0, vertex.attrib\[1\].w, \7; (.*?) MAD \9.xyz, _tmp0, vertex.attrib\[1\].w, \9; (?:ENDIF; ENDIF; ENDIF;|.*:)")
VP_SKINNING_SUB = "\\1MOV \\2.w, 1; MOV \\2.xyz, vertex.position; MOV \\7, vertex.normal; MOV \\9, vertex.attrib[6];"

VP_SKINNING_REGEX_2 = recompile(r"(BUFFER4[\s\S]*?)MOV (.*?).w, 1; SUBC _tmp1, vertex.attrib\[15\], (-1|255); (.*?) IF NE.y; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].y, \2; (.*?) MAD (.*?).xyz, _tmp0, vertex.attrib\[1\].y, \7; IF NE.z; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].z, \2; (.*?) MAD \7.xyz, _tmp0, vertex.attrib\[1\].z, \7; IF NE.w; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].w, \2; (.*?) MAD \7.xyz, _tmp0, vertex.attrib\[1\].w, \7; (?:ENDIF; ENDIF; ENDIF;|.*:)")
VP_SKINNING_SUB_2 = "\\1MOV \\2.w, 1; MOV \\2.xyz, vertex.position; MOV \\7, vertex.normal;"

VP_SKINNING_REGEX_3 = recompile(r"(BUFFER4[\s\S]*?)MOV (.*?).w, 1; SUBC _tmp1, vertex.attrib\[15\], (-1|255); (.*?) IF NE.y; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].y, \2; IF NE.z; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].z, \2; IF NE.w; (.*?) MAD \2.xyz, _tmp0, vertex.attrib\[1\].w, \2; (?:ENDIF; ENDIF; ENDIF;|.*:)")
VP_SKINNING_SUB_3 = "\\1MOV \\2.w, 1; MOV \\2.xyz, vertex.position;"


# some kind of effect (scrunching up or stretching?) in tights shaders starting with 11 that partially uses skinning of normals...  let's just pretend it always results in 0
TIGHTS_SKINNING_EFFECT_REGEX = recompile(r"(BUFFER4[\s\S]*?)SUBC .*?, vertex.attrib\[15\], .*?;\s*?SUBC1 tmp, vertex.attrib\[15\], .*?;[\s\S]*?XPD .*?, .*?, .*?;\s*?DP3_SAT (.*?), .*?, .*?;")
TIGHTS_SKINNING_EFFECT_SUB = "\\1MOV \\2, 0;"


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
    (MATCH_TIGHTS_FNAME, TIGHTS_SKINNING_EFFECT_REGEX, TIGHTS_SKINNING_EFFECT_SUB, False),
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