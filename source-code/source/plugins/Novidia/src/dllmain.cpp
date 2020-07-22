#include "framework.h"
#include "glStuff.h"
#include <PluginConfigApi.h>



// upload to the SSBO instead after processing
void h_uploadModelTransformBuf(DWORD* a1, int a2)
{
	uploadModelTransformBuf(a1, a2);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_1D, buf_tex);
	//glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 0x3000 / sizeof(float) / 4, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
	glActiveTexture(GL_TEXTURE0);
	//glFinish();
}

/*
void h_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
	if (performTransform)
	{
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_1D, buf_tex);
		//glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 0x3000 / sizeof(float) / 4, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
		//glActiveTexture(GL_TEXTURE0);
	}
	glDrawElements(mode, count, type, indices);
}

void h_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void* indices)
{
	if (performTransform)
	{
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_1D, buf_tex);
		//glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
		glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 0x3000 / sizeof(float) / 4, GL_RGBA, GL_FLOAT, *(float**)0x1411a3330);
		//glActiveTexture(GL_TEXTURE0);
	}
	glDrawRangeElements(mode,start, end, count, type, indices);
}
*/

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
		glBindTexture(GL_TEXTURE_1D, buf_tex);
		glTexStorage1D(GL_TEXTURE_1D, 1, GL_RGBA32F, 0x3000 / sizeof(float) / 4);
		//glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 0x3000 / sizeof(float) / 4, 0, GL_RGBA, GL_FLOAT, NULL);
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
		//loadConfig();

		DisableThreadLibraryCalls(hModule);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		printf("[Novidia] Hooking glutSetCursor\n");
		DetourAttach(&(PVOID&)glutSetCursor, h_glutSetCursor);
		printf("[Novidia] Hooking uploadModelTransformBuf\n");
		DetourAttach(&(PVOID&)uploadModelTransformBuf, h_uploadModelTransformBuf);
		printf("[Novidia] Hooking wglGetProcAddress\n");
		DetourAttach(&(PVOID&)wGlGetProcAddress_forhook, h_wglGetProcAddress);
		DetourTransactionCommit();
	}

	return TRUE;
}



using namespace PluginConfig;

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"Novidia";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Novidia by somewhatlurker\n\nPerforms some model skinning transformations in an alternate way to enable functionality on non-Nvidia hardware.\nAlso fixes crashing on non-Nvidia hardware.\n\nRequires patched shaders and GLIntercept installed to work.";
}