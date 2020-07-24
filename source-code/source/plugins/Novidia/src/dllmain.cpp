#include "framework.h"
#include "glStuff.h"
#include <PluginConfigApi.h>


// upload to the SSBO instead after processing
// two versions because apparently TexSubImage can cause major stuttering
void h_uploadModelTransformBuf_TexImage(DWORD* a1, int a2)
{
	uploadModelTransformBuf(a1, a2);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
	glActiveTexture(GL_TEXTURE0);
}
void h_uploadModelTransformBuf_TexSubImage(DWORD* a1, int a2)
{
	uploadModelTransformBuf(a1, a2);

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 0x3000 / sizeof(float) / 4, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
	glActiveTexture(GL_TEXTURE0);
}

// just a crash fix
void h_glBindBuffer(GLenum target, GLuint buffer)
{
	// if not okay, it's param buffer data
	if (!checkBufferTargetOk(target))
	{
		return;
	}

	glBindBuffer(target, buffer);
}

// just a crash fix
void h_glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
	// if not okay, it's param buffer data
	if (!checkBufferTargetOk(target))
	{
		return;
	}
	
	glBufferData(target, size, data, usage);
}

PROC h_wglGetProcAddress(LPCSTR Arg1)
{
	PROC res = wGlGetProcAddress_forhook(Arg1);
	if (res == nullptr)
		return (PROC)stub;
	return res;
}

// used as an entry point after GL context is created
void h_glutSetCursor(int cursor)
{
	if (!loadGlAddresses())
	{
		printf("[Novidia] Aborting loading\n");
		//MessageBoxA(NULL, "!", NULL, NULL);
	}
	else
	{

		struct {
			void* target;
			void* hook;
		} hookFuncs[] = {
			{&glBindBuffer, h_glBindBuffer},
			{&glBindBufferARB, h_glBindBuffer},
			{&glBufferData, h_glBufferData},
			{&glBufferDataARB, h_glBufferData},
			//{&glDrawElements, h_glDrawElements},
			//{&glDrawRangeElements, h_glDrawRangeElements},
		};

		for (auto fn : hookFuncs)
		{
			hookGlFunc((PROC*)fn.target, fn.hook);
		}

		//glActiveTexture(GL_TEXTURE8);
		glGenTextures(1, &buf_tex);
		printf("[Novidia] Buffer texture id: %d\n", buf_tex);
		glBindTexture(GL_TEXTURE_1D, buf_tex);
		if (use_TexSubImage)
			glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA32F, 0x3000 / sizeof(float) / 4);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	printf("[Novidia] Unhooking glutSetCursor\n");
	DetourDetach(&(PVOID&)glutSetCursor, h_glutSetCursor);
	DetourTransactionCommit();

	glutSetCursor(cursor);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		loadConfig();

		DisableThreadLibraryCalls(hModule);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		printf("[Novidia] Hooking glutSetCursor\n");
		DetourAttach(&(PVOID&)glutSetCursor, h_glutSetCursor);
		printf("[Novidia] Hooking wglGetProcAddress\n");
		DetourAttach(&(PVOID&)wGlGetProcAddress_forhook, h_wglGetProcAddress);

		printf("[Novidia] Hooking uploadModelTransformBuf\n");
		if (use_TexSubImage)
			DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf_TexSubImage);
		else
			DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf_TexImage);

		DetourTransactionCommit();
	}

	return TRUE;
}



using namespace PluginConfig;

PluginConfigOption config[] = {
	{ CONFIG_BOOLEAN, new PluginConfigBooleanData{ L"use_TexSubImage", L"general", CONFIG_FILE, L"Use glTexSubImage", L"glTexSubImage should offer higher performance, but stuttering has been reported when it is used.\nTry disabling this if you have issues.", true, false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Novidia";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Novidia by somewhatlurker\n\nPerforms some model skinning transformations in an alternate way to enable functionality on non-Nvidia hardware.\nAlso fixes crashing on non-Nvidia hardware.";
}

extern "C" __declspec(dllexport) PluginConfigArray GetPluginOptions(void)
{
	return PluginConfigArray{ _countof(config), config };
}