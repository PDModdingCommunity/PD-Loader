#pragma once
void exitGameClean(int);
int hookedCreateWindow(const char* title, void(__cdecl* exit_function)(int));
__int64 hookedParseParameters(int a1, __int64* a2);
__int64 __fastcall limiterFuncNormal(__int64 a1);
__int64 __fastcall limiterFuncLight(__int64 a1);
__int64 __fastcall hookedEngineUpdate(__int64 a1);
void ApplyRender(HMODULE hModule);
