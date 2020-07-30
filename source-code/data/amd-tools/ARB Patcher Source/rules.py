# regexes and config info is stored here to reduce clutter in main script
from re import compile as recompile

# inline these because...  idk
# ATTRIB and OUTPUT must be inlined for vp tho
ATTRIB_REGEX = recompile(r"(?<!#)(?<!#\s)((SHORT\s*?|LONG\s*?)?ATTRIB\s*?([^\[\]\s]*?)\s*?=\s*?([^\s].*?);)\s*")
OUTPUT_REGEX = recompile(r"(?<!#)(?<!#\s)((SHORT\s*?|LONG\s*?)?OUTPUT\s*?([^\[\]\s]*?)\s*?=\s*?([^\s].*?);)\s*")
PARAM_REGEX = recompile(r"(?<!#)(?<!#\s)((SHORT\s*?|LONG\s*?)?PARAM\s*?([^\[\]\s]*?)\s*?=\s*?([^\s].*?);)\s*")

# help reduce param count more by commenting out unused ones that weren't inlined (array params can't be inlined)
UNUSED_PARAM_REGEX = recompile(r"(?<!#)(?<!#\s)((SHORT\s*?|LONG\s*?)?PARAM\s*?([^\[\]\s]*?)(\s*?\[.*?\])?\s*?=\s*?([^\s].*?);)\s*(?![\s\S]*[\s\-\+{]\3[\s,;\.\-\+\[])")

# I think some of these instructions might still be left after other patches..  they will be commented out
BAD_INSTR_REGEX = recompile(r"((?<!#)(?<!#\s)(CVT|BUFFER4|POW|NRM|REP)\s+?.*?;|ENDREP;|[^;\n]*\s*?(p_coef2d\[idx.x\]).*?;)\s*")

# find the header tag to replace with !!ARBxx1.0 and an NV OPTION
NVFP_HEADER_REGEX = recompile(r"!!NVfp\d\.\d")
NVVP_HEADER_REGEX = recompile(r"!!NVvp\d\.\d")


# POW, NRM, and SSG sometimes(?) don't work (depending on inputs?), so just reimplement with other instructions
POW_REGEX = recompile(r"POW(\s+?|_SAT\s+?)(.*?),\s+?(.*?),\s+?(.*?);")
POW_SUB = "LG2 \\2, \\3; MUL \\2, \\2, \\4; EX2\\1\\2, \\2;"

NRM_REGEX = recompile(r"NRM\s+?(.*?),\s+?(.*?);")
NRM_SUB = "DP3 nrm_tmp.w, \\2, \\2; RSQ nrm_tmp.w, nrm_tmp.w; MUL \\1, nrm_tmp.w, \\2;"

SSG_REGEX = recompile(r"SSG\s+?(\S.*?\.\S*?),\s+?(.*?);")
SSG_SUB = "SGE ssg_tmp1, \\2, 0; SUB ssg_tmp1, ssg_tmp1, 1; SGT ssg_tmp2, \\2, 0; ADD \\1, ssg_tmp1, ssg_tmp2;"


# seems like using a TEX with pixel offsets is pretty common, but I can't make that work
# afaik there's no general way to get texture resolution so I'll approximate one here that can be refined later per-shader
# ({ 0.00078, 0.0014 } is approximately one pixel of a 720p texture/framebuffer)
SAMPLER_OFFSET_REGEX = recompile(r"TEX\s+?([^;,]*?),\s+?([^;,]*?),\s+?([^;,]*?),\s+?2D,\s+?\((.*?)\);")
SAMPLER_OFFSET_SUB = "MUL tex_offset_coord, {\\4}, { 0.00078, 0.0014 }; ADD tex_offset_coord, \\2, tex_offset_coord; TEX \\1, tex_offset_coord, \\3, 2D;"


# ret at the end of an IF statement is moved to outside of it (or won't compile)
# not a perfect solution, but works well enough
RET_IF_REGEX = recompile(r"(IF\s+?(.*?);[\s\S]*?)RET;\s*?ENDIF;")
RET_IF_SUB = "\\1ENDIF;\nRET (\\2);"


# RET behaves differently on AMD, so convert conditional rets into if/else
# (may not work properly with CAL)
CONDITIONAL_RET_REGEX = recompile(r"RET\s+?\((.*?)\);([\s\S]*?)(END\s*?$|RET;)")
CONDITIONAL_RET_SUB = "IF \\1; ELSE; \\2 ENDIF; \\3"


# CAL behaves differently on AMD, so convert conditional cals into if
CONDITIONAL_CAL_REGEX = recompile(r"CAL\s+?(.*?)\s+?\((.*?)\)\s*?;")
CONDITIONAL_CAL_SUB = "IF \\2; CAL \\1; ENDIF;"


# seems like some suffixes (eg .F, .CC1) don't work
# first regex: drop everything before the CC suffix, and convert CC suffix to appended C0/C1
INSTR_SUFFIX_REGEX = recompile(r"(;\s*?)([A-Z\d]+)[A-Z\d\.]*?\.CC(\d)")
INSTR_SUFFIX_SUB = "\\1\\2C\\3"
# second regex: remove all remaining suffixes (or all suffixes for instructions without CC)
INSTR_SUFFIX_REGEX_2 = recompile(r"(;\s*?)([A-Z\d]+)\.[A-Z\.\d]+")
INSTR_SUFFIX_SUB_2 = "\\1\\2"


MATCH_FP_FNAME = recompile(r".*fp")
MATCH_VP_FNAME = recompile(r".*vp")
MATCH_ALL_FNAME = recompile(r".*")

# fourth param: apply continuously until no more matches are found
fix_repls = [
    (MATCH_ALL_FNAME, POW_REGEX, POW_SUB, False),
    (MATCH_ALL_FNAME, NRM_REGEX, NRM_SUB, False),
    (MATCH_ALL_FNAME, SSG_REGEX, SSG_SUB, False),
    (MATCH_ALL_FNAME, SAMPLER_OFFSET_REGEX, SAMPLER_OFFSET_SUB, False),
    (MATCH_ALL_FNAME, RET_IF_REGEX, RET_IF_SUB, False),
    (MATCH_ALL_FNAME, CONDITIONAL_RET_REGEX, CONDITIONAL_RET_SUB, True),
    (MATCH_ALL_FNAME, CONDITIONAL_CAL_REGEX, CONDITIONAL_CAL_SUB, False),
    (MATCH_ALL_FNAME, INSTR_SUFFIX_REGEX, INSTR_SUFFIX_SUB, False),
    (MATCH_ALL_FNAME, INSTR_SUFFIX_REGEX_2, INSTR_SUFFIX_SUB_2, False),
]
