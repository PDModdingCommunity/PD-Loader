FarcPack ..\rom\shader.farc shader_unpacked
"ARB Patcher Source\main.py" -i shader_unpacked -o shader_patched -g divaaft
mkdir ..\mdata\MAMD
mkdir ..\mdata\MAMD\rom
copy info.txt ..\mdata\MAMD\
FarcPack -c shader_patched ..\mdata\MAMD\rom\shader_amd.farc
del /Q shader_unpacked
rmdir shader_unpacked
del /Q shader_patched
rmdir shader_patched