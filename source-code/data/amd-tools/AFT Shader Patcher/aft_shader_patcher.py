import sys
from os import chdir, makedirs, get_terminal_size
from os.path import join as joinpath, splitext, isfile, exists, dirname, abspath
import pyfarc
import importlib, importlib.util

if getattr(sys, 'frozen', False):
    datadir = dirname(sys.executable)
else:
    datadir = dirname(__file__)

arbpatcher_dir = joinpath(datadir, 'ARB Patcher')
arbpatcher_gamesettings_dir = joinpath(arbpatcher_dir, 'gamesettings')

sys.path = [arbpatcher_dir] + sys.path
import main as ArbPatcher
sys.path = sys.path[1:]

game_settings_spec = importlib.util.spec_from_file_location('divaaft', joinpath(arbpatcher_gamesettings_dir, 'divaaft.py'))
game_settings_module = importlib.util.module_from_spec(game_settings_spec)
game_settings_spec.loader.exec_module(game_settings_module)


# not bothering to check for main here because it's just a glue script anyway
def get_args():    
    import argparse
    parser = argparse.ArgumentParser(description='AFT Shader Patcher: Patches Nvidia-only ARB shaders from PDAFT to work on AMD. Patching techniques from Nezarn; implementation by somewhatlurker.')
    parser.add_argument('-i', '--in_farc', default='shader.farc', help='input shader farc file (default: "shader.farc")')
    parser.add_argument('-o', '--out_farc', default='shader_patched.farc', help='output shader farc file (default: "shader_patched.farc")')
    parser.add_argument('-c', '--compress', action='store_true', help='force use of farc compression (this mey produce larger vcdiff files)')
    parser.add_argument('-x', '--xdelta', action='store_true', help='generate a vcdiff patch file')
    
    return parser.parse_args()

args = get_args()

print("AFT Shader Patcher")
print("==================")
print("Patching techniques from Nezarn; implementation by somewhatlurker")
print("=================================================================")
print("Input file: '{}'".format(args.in_farc))
print("Output file: '{}'".format(args.out_farc))

if not exists(args.in_farc):
    print ("'{}' does not exist. Aborting".format(args.in_farc))
    sys.exit()
elif not isfile(args.in_farc):
    print ("'{}' is not a file. Aborting".format(args.in_farc))
    sys.exit()

if exists(args.out_farc):
    if not isfile(args.out_farc):
        print ("'{}' already exists but is a directory. Aborting".format(args.out_farc))
        sys.exit()


with open(args.in_farc, 'rb') as f:
    farcdata = pyfarc.from_stream(f)
    if args.xdelta:
        f.seek(0)
        from binascii import crc32
        in_farc_crc_str = '{:08x}'.format(crc32(f.read()))

proc_count = 0
    
last_status_len = 0

for fname in farcdata['files']:
    proc_count += 1
    progress_val = proc_count / len(farcdata['files'])
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
    
    f_lines = farcdata['files'][openname]['data'].decode('utf-8').splitlines(keepends=True)
    
    f_full = ArbPatcher.patch_shader(fname, f_lines, game_settings_module, True)
    
    farcdata['files'][fname]['data'] = f_full.encode('utf-8')

if args.compress:
    farcdata['farc_type'] = 'FArC'

with open(args.out_farc, 'wb') as f:
    pyfarc.to_stream(farcdata, f, no_copy=True)

if args.xdelta:
    import subprocess
    
    # make paths absolute before running this
    #args.in_farc = abspath(args.in_farc)
    #args.out_farc = abspath(args.out_farc)
    subprocess.run([joinpath(datadir, 'xdelta3.exe'), '-e', '-f', '-S', 'none', '-s', args.in_farc, args.out_farc, joinpath(dirname(args.out_farc), in_farc_crc_str + '.vcdiff')], )
