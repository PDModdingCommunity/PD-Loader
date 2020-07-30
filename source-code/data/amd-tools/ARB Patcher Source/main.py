from rules import *

def patch_shader(fname, f_lines, game_settings_module=None, enable_print_warnings=True):
    f_lines[0] += "#" + fname +"\n"
        
        
    # do attrib, output, param simplification and replace header tags
    attribs = {}
    outputs = {}
    params = {}

    for i in range(0, len(f_lines)):   
        # this inlining stuff is only really needed for vp, but :shrug:
        no_repl = False
        
        m = ATTRIB_REGEX.match(f_lines[i])
        while m:
            attribs[m.group(3)] = m.group(4)
            f_lines[i] = ATTRIB_REGEX.sub('#\\1 \n', f_lines[i], count=1)
            no_repl = True
            m = ATTRIB_REGEX.match(f_lines[i])
        
        m = OUTPUT_REGEX.match(f_lines[i])
        while m:
            outputs[m.group(3)] = m.group(4)
            f_lines[i] = OUTPUT_REGEX.sub('#\\1 \n', f_lines[i], count=1)
            no_repl = True
            m = OUTPUT_REGEX.match(f_lines[i])
        
        # and not needed at all for PARAMs in the end, but :shrug: again
        m = PARAM_REGEX.match(f_lines[i])
        while m:
            params[m.group(3)] = m.group(4)
            f_lines[i] = PARAM_REGEX.sub('#\\1 \n', f_lines[i], count=1)
            no_repl = True
            m = PARAM_REGEX.match(f_lines[i])
        
        if no_repl:
            continue
        
        replace_tokens = attribs.copy()
        replace_tokens.update(outputs)
        replace_tokens.update(params)
        
        # sort from longest to shortest to minimize risk of names that are substrings of others causing issues
        for k in sorted(replace_tokens.keys(), reverse=True, key=lambda s: len(s)):
            # look for specific preceding characters to not replace in the middle of a name
            f_lines[i] = f_lines[i].replace(' ' + k, ' ' + replace_tokens[k])
            f_lines[i] = f_lines[i].replace('-' + k, '-' + replace_tokens[k])
            f_lines[i] = f_lines[i].replace('{' + k, '{' + replace_tokens[k])
        
        # options are required for conditional stuff...
        # tysm nezarn for teaching me this because otherwise the patch would never be anywhere near as complete
        f_lines[i] = NVFP_HEADER_REGEX.sub("!!ARBfp1.0\nOPTION NV_fragment_program2;", f_lines[i])
        f_lines[i] = NVVP_HEADER_REGEX.sub("!!ARBvp1.0\nOPTION NV_vertex_program3;", f_lines[i])
    
    f_full = ''.join(f_lines)
    
    f_full = UNUSED_PARAM_REGEX.sub("#\\1 \n", f_full)
    
    
    # add a temp vars for NRM and SSG replacements before their first use
    f_full = f_full.replace('NRM ', 'TEMP nrm_tmp; NRM ', 1)
    f_full = f_full.replace('SSG ', 'TEMP ssg_tmp1, ssg_tmp2; SSG ', 1)
    # this needs to use the regex to match because it isn't just a single instruction
    f_full = SAMPLER_OFFSET_REGEX.sub('TEMP tex_offset_coord; \\g<0>', f_full, count=1)
    
    for r in fix_repls:
        if r[0].match(fname):
            if r[3]: # r[3]: apply until no matches
                while r[1].search(f_full):
                    f_full = r[1].sub(r[2], f_full)
            else:
                f_full = r[1].sub(r[2], f_full)
    
    if game_settings_module and game_settings_module.fix_repls:
        for r in game_settings_module.fix_repls:
            if r[0].match(fname):
                if r[3]: # r[3]: apply until no matches
                    while r[1].search(f_full):
                        f_full = r[1].sub(r[2], f_full)
                else:
                    f_full = r[1].sub(r[2], f_full)
    
            
    f_full = f_full.replace('INT TEMP ', 'TEMP ')
    
    if game_settings_module and game_settings_module.post_filter:
        f_full = game_settings_module.post_filter(fname, f_full)
    
    
    bad_instr = BAD_INSTR_REGEX.search(f_full)
    if bad_instr:
        if enable_print_warnings:
            print ('\rWarning -- unpatched known bad instruction(s) in {} -- first: {}'.format(fname, bad_instr.group(1)))
        f_full = BAD_INSTR_REGEX.sub("#\\1 \n", f_full)
    
    f_full = f_full.replace('\r', '')
    
    return f_full

if __name__ == '__main__':
    from os import listdir, makedirs, get_terminal_size
    from os.path import join as joinpath, splitext, isfile, exists, dirname
    from re import compile as recompile
    import sys
    import importlib, importlib.util
    
    if getattr(sys, 'frozen', False):
        datadir = dirname(sys.executable)
    else:
        datadir = dirname(__file__)
    
    gamesettings_dir = joinpath(datadir, 'gamesettings')
    
    def get_args():
        # make a list of game settings modules
        game_modules_list = [f[:-3] for f in listdir(gamesettings_dir) if isfile(joinpath(gamesettings_dir, f)) and splitext(f)[1] == '.py']
        
        import argparse
        parser = argparse.ArgumentParser(description='AMD ARB Patcher: An attempt at converting Nvidia-only ARB shaders to work on AMD. Patching techniques from Nezarn; implementation by somewhatlurker.')
        parser.add_argument('-i', '--in_dir', default='shader in', help='input directory containing .vp and .fp files (default: "shader in")')
        parser.add_argument('-o', '--out_dir', default='shader patched', help='output directory (default: "shader patched")')
        parser.add_argument('-f', '--file_filter', default=None, help='only process files that match this filter (regex)')
        parser.add_argument('-g', '--game_settings', default=None, choices=game_modules_list, help='game-specific settings to load')
        
        parser.add_argument('--list_games', action='store_true', help='list game names for game-specific settings options instead of patching shaders')
        
        return parser.parse_args()

    def print_game_modules():
        # make a list of game settings modules
        game_modules_list = [f[:-3] for f in listdir(gamesettings_dir) if isfile(joinpath(gamesettings_dir, f)) and splitext(f)[1] == '.py']
        
        for game in game_modules_list:
            game_settings_spec = importlib.util.spec_from_file_location(game, joinpath(gamesettings_dir, game + '.py'))
            game_settings_module = importlib.util.module_from_spec(game_settings_spec)
            game_settings_spec.loader.exec_module(game_settings_module)
            
            if game_settings_module.GAME_NAME:
                full_name = game_settings_module.GAME_NAME
            else:
                full_name = 'Unknown'
            
            print ('{}: {}'.format(game, full_name))
    
    
    args = get_args()
    
    if args.list_games:
        print_game_modules()
        sys.exit()
    
    print("AMD ARB Patcher")
    print("===============")
    print("Patching techniques from Nezarn; implementation by somewhatlurker")
    print("=================================================================")
    print("Input directory: '{}'".format(args.in_dir))
    print("Output directory: '{}'".format(args.out_dir))
    
    if args.game_settings:
        game_settings_spec = importlib.util.spec_from_file_location(args.game_settings, joinpath(gamesettings_dir, args.game_settings + '.py'))
        game_settings_module = importlib.util.module_from_spec(game_settings_spec)
        game_settings_spec.loader.exec_module(game_settings_module)
        if game_settings_module.GAME_NAME:
            print("Using game-specific settings for {}".format(game_settings_module.GAME_NAME))
        else:
            print("Using game-specific settings for {}".format(args.game_settings))
    else:
        game_settings_module = None
    
    IN_DIR = args.in_dir
    OUT_DIR = args.out_dir
    
    if not exists(IN_DIR):
        print ("Directory '{}' does not exist. Aborting".format(IN_DIR))
        sys.exit()
    elif isfile(IN_DIR):
        print ("'{}' is not a directory. Aborting".format(IN_DIR))
        sys.exit()
    
    if exists(OUT_DIR):
        if isfile(OUT_DIR):
            print ("'{}' already exists but is not a directory. Aborting".format(OUT_DIR))
            sys.exit()
    else:
        makedirs(OUT_DIR)
    
    if args.file_filter:
        file_filter = recompile(args.file_filter)
    else:
        file_filter = None
    
    indir_list = listdir(IN_DIR)
    
    if file_filter:
        indir_list = [f for f in indir_list if file_filter.match(f)]
    
    proc_count = 0
    
    last_status_len = 0
    
    for fname in indir_list:
        proc_count += 1
        progress_val = proc_count / len(indir_list)
        progress_cnt_X = int(progress_val * 20)
        
        status_str = '\r[{e:{s1}<{n1}}{e:{s2}<{n2}}]'.format(e='', s1='X', s2='-', n1=progress_cnt_X, n2=20-progress_cnt_X)
        status_str += ' {:.2%}'.format(progress_val)
        status_str += '   ' + fname
        
        try:
            terminal_width = get_terminal_size()[0]
        except:
            terminal_width = 120
        
        if len(status_str) > terminal_width:
            status_str = status_str[:terminal_width-3] + '...'
        
        # fix characters left on screen from a previous longer line
        # (without cursor staying off to the side)
        this_status_len = len(status_str)
        if this_status_len < last_status_len:
            status_str = '{: <{l}}'.format(status_str, l=last_status_len)
        last_status_len = this_status_len
        
        print (status_str, end='')
        
        if game_settings_module and game_settings_module.filename_filter:
            openname = game_settings_module.filename_filter(fname)
        else:
            openname = fname
        
        with open(joinpath(IN_DIR, openname), 'r') as f:
            f_lines = f.readlines()
        
        f_full = patch_shader(fname, f_lines, game_settings_module, True)
        
        with open(joinpath(OUT_DIR, fname), 'wb') as f:
            f.write(f_full.encode('utf-8'))
